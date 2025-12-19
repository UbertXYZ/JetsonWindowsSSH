#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <torch/torch.h>

#include "DQ_Network_CUDA.h"

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    connect(sock, (sockaddr*)&server, sizeof(server));
    std::cout << "Inicializando DQN...\n";

    DQN_Controlador dqn(3, 3);

    torch::Tensor estado = torch::tensor({0.2f, 0.5f, 0.1f});

    int accion = dqn.ElegirAccion(estado);

    std::cout << "Accion elegida por DQN: " << accion << "\n";

    std::string msg = std::to_string(accion);
    send(sock, msg.c_str(), msg.size(), 0);

    closesocket(sock);
    WSACleanup();

    return 0;
}