#include <torch/torch.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "DQ_Network_CUDA.h"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.200.1", &server.sin_addr);

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "No se pudo conectar al servidor\n";
        return 1;
    }
    std::cout << "Conectado al servidor 192.168.200.1\n";
    DQN_Controlador dqn;
    while (true) {
        torch::Tensor state = torch::rand({3});
        int accion = dqn.ElegirAccion(state);
        send(sock, &accion, sizeof(int), 0);
        std::cout << "Accion enviada: " << accion << std::endl;
        sleep(1);
    }
    close(sock);
    return 0;
}