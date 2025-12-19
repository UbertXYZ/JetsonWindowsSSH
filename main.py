import socket
tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
PORT_TCP = 9999
tcp_sock.bind(("", PORT_TCP))
tcp_sock.listen(1)
print("Esperando conexión del Jetson...")

conn, _ = tcp_sock.accept()
print("Jetson conectado")

while True:
    data = conn.recv(1024)
    if not data:
        break
    print("Jetson dice:", data)
    
conn.close()