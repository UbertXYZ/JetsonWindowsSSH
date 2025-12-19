#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup fallo\n";
        return 1;
    }
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error creando socket\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // IP COMO TEXTO

    if (connect(sock, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Error conectando\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    const char* msg = "ADELANTE";
    send(sock, msg, strlen(msg), 0);

    closesocket(sock);
    WSACleanup();

    return 0;
}