#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock == INVALID_SOCKET) {
        std::cerr << "Error creando socket\n";
        return 1;
    }
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // Escucha en todas las IPs
    server.sin_port = htons(9999);

    bind(serverSock, (sockaddr*)&server, sizeof(server));
    listen(serverSock, 1);

    std::cout << "Servidor esperando conexion...\n";
    SOCKET clientSock = accept(serverSock, nullptr, nullptr);
    std::cout << "Cliente conectado\n";
    char buffer[128] = {};
    int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        int accion = std::stoi(buffer);

        std::cout << "Accion recibida del DQN: " << accion << std::endl;
    }
    closesocket(clientSock);
    closesocket(serverSock);
    WSACleanup();

    return 0;
}