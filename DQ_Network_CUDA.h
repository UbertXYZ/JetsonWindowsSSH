#ifndef DQ_NETWORK_CUDA_H
#define DQ_NETWORK_CUDA_H
#include <string>
#include <torch/torch.h>
#include <deque>
#include <random>
struct QNetworkImpl : torch::nn::Module {
    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

    QNetworkImpl(int entrada, int ocultas, int salidas) {
        fc1 = register_module("fc1", torch::nn::Linear(entrada, ocultas));
        fc2 = register_module("fc2", torch::nn::Linear(ocultas, ocultas));
        fc3 = register_module("fc3", torch::nn::Linear(ocultas, salidas));
    }

    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(fc1(x));
        x = torch::relu(fc2(x));
        return fc3(x);
    }
};
TORCH_MODULE(QNetwork);
struct Experiencia {
    torch::Tensor state;
    int action;
    float reward;
    torch::Tensor next_state;
    bool done;
};

class DQN_Controlador {
public:
    int n_acciones;
    float gamma;
    int batch_size;
    float epsilon, epsilon_min, epsilon_decay;
    int pasos;

    QNetwork politica;
    QNetwork objetivo;

    torch::optim::Adam optimizador;
    torch::nn::MSELoss loss_fn;

    std::deque<Experiencia> replay;
    int max_buffer;

    torch::Device device;

    DQN_Controlador(int n_estados=3, int n_acciones=3, int n_ocultas=64)
        : n_acciones(n_acciones),
          gamma(0.99),
          batch_size(64),
          epsilon(1.0),
          epsilon_min(0.05),
          epsilon_decay(0.9999),
          pasos(0),
          politica(QNetwork(n_estados, n_ocultas, n_acciones)),
          objetivo(QNetwork(n_estados, n_ocultas, n_acciones)),
          optimizador(politica->parameters(), torch::optim::AdamOptions(0.0005)),
          max_buffer(5000),
          device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU)
    {
        
        politica->to(device);
        objetivo->to(device);
        CopiarPesos(politica, objetivo);
    }
    void CopiarPesos(QNetwork& origen, QNetwork& destino) {
        torch::NoGradGuard no_grad;
        auto params_origen = origen->named_parameters();
        auto params_destino = destino->named_parameters(true);

        for (const auto& item : params_origen) {
            params_destino[item.key()].copy_(item.value());
        }
    }
    int ElegirAccion(torch::Tensor state) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        if (dis(gen) < epsilon) {
            std::uniform_int_distribution<> accion(0, n_acciones - 1);
            return accion(gen);
        }

        torch::NoGradGuard no_grad;
        auto q_vals = politica->forward(state.to(device));
        return q_vals.argmax().item<int>();
    }
    void AnadirExperiencia(torch::Tensor s, int a, float r, torch::Tensor ns, bool d) {
        if (replay.size() >= max_buffer)
            replay.pop_front();

        replay.push_back({s, a, r, ns, d});
    }
     void EntrenarPaso() {
        if (replay.size() < batch_size) return;

        std::vector<Experiencia> batch;
        std::sample(replay.begin(), replay.end(),
                    std::back_inserter(batch),
                    batch_size,
                    std::mt19937{std::random_device{}()});

        std::vector<torch::Tensor> states, next_states;
        std::vector<int64_t> actions;
        std::vector<float> rewards, dones;

        for (auto& e : batch) {
            states.push_back(e.state);
            next_states.push_back(e.next_state);
            actions.push_back(e.action);
            rewards.push_back(e.reward);
            dones.push_back(e.done ? 1.0f : 0.0f);
        }

        auto S = torch::stack(states).to(device);
        auto NS = torch::stack(next_states).to(device);
        auto A = torch::tensor(actions, torch::kLong).to(device);
        auto R = torch::tensor(rewards).to(device);
        auto D = torch::tensor(dones).to(device);

        auto q_vals = politica->forward(S).gather(1, A.unsqueeze(1)).squeeze();

        torch::NoGradGuard no_grad;

        auto max_result = objetivo->forward(NS).max(1);
        auto q_next = std::get<0>(max_result);

        auto target = R + gamma * q_next * (1 - D);

        auto loss = torch::mse_loss(q_vals, target);

        optimizador.zero_grad();
        loss.backward();
        optimizador.step();

        pasos++;
        if (pasos % 100 == 0) {
            CopiarPesos(politica, objetivo);
        }

        epsilon = std::max(epsilon * epsilon_decay, epsilon_min);
    }
};
#endif