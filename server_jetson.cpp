#include <iostream>
#include <string>  
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = inet_addr("192.168.200.1");

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    std::cout << "Servidor esperando conexion en 192.168.200.1:9999...\n";

    SOCKET client = accept(server_fd, nullptr, nullptr);
    std::cout << "Cliente conectado\n";

    while (true) {
        int accion;
        recv(client, (char*)&accion, sizeof(int), 0);
        std::cout << "Accion recibida del DQN: " << accion << std::endl;
    }

    closesocket(client);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}