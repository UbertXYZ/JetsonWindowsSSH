import torch
import torch.nn as nn
import torch.optim as optim
import random

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print("DQN usando:", device)

class QNetwork(nn.Module):
    def __init__(self, c_entrada, c_ocultas, c_salidas):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(c_entrada, c_ocultas),
            nn.ReLU(),
            nn.Linear(c_ocultas, c_ocultas),
            nn.ReLU(),
            nn.Linear(c_ocultas, c_salidas)
        )
        
    def forward(self, x):
        return self.net(x)

class DQN_Controlador:
    def __init__(self, n_estados=3, n_acciones=3, n_ocultas=64, gamma=0.99, lr=0.0005, batch_size=64, max_buffer=5000):
        self.n_acciones = n_acciones
        self.gamma = gamma
        self.batch_size = batch_size
        self.politica = QNetwork(n_estados, n_ocultas, n_acciones).to(device)
        self.objetivo = QNetwork(n_estados, n_ocultas, n_acciones).to(device)
        self.objetivo.load_state_dict(self.politica.state_dict())
        self.optimizador = optim.Adam(self.politica.parameters(), lr=lr)
        self.func_perdida = nn.MSELoss()
        self.replay = []
        self.max_buffer = max_buffer
        self.epsilon = 1.0
        self.epsilon_min = 0.05
        self.epsilon_decay = 0.9999
        self.pasos = 0

    def ElegirAccion(self, state):
        if random.random() < self.epsilon:
            return random.randint(0, self.n_acciones - 1)
        with torch.no_grad():
            s = torch.tensor(state, dtype=torch.float32).to(device)
            return self.politica(s).argmax().item()

    def AnadirExperiencia(self, state, action, reward, next_state, done):
        self.replay.append((state, action, reward, next_state, done))
        if len(self.replay) > self.max_buffer:
            self.replay.pop(0)

    def EntrenarPaso(self):
        if len(self.replay) < self.batch_size:
            return
        batch = random.sample(self.replay, self.batch_size)
        states = torch.tensor([b[0] for b in batch], dtype=torch.float32).to(device)
        actions = torch.tensor([b[1] for b in batch]).to(device)
        rewards = torch.tensor([b[2] for b in batch], dtype=torch.float32).to(device)
        siguiente_estado = torch.tensor([b[3] for b in batch], dtype=torch.float32).to(device)
        dones = torch.tensor([b[4] for b in batch], dtype=torch.float32).to(device)
        valores_q = self.politica(states).gather(1, actions.unsqueeze(1)).squeeze()
        with torch.no_grad():
            siguiente_q = self.objetivo(siguiente_estado).max(1)[0]
            objetivo_q = rewards + self.gamma * siguiente_q * (1 - dones)
        loss = self.func_perdida(valores_q, objetivo_q)
        self.optimizador.zero_grad()
        loss.backward()
        self.optimizador.step()
        self.pasos += 1
        if self.pasos % 100 == 0:
            self.objetivo.load_state_dict(self.politica.state_dict())
        self.epsilon = max(self.epsilon * self.epsilon_decay, self.epsilon_min)
