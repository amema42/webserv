#!/usr/bin/env python3
import socket

#Test: keep-alive connection with a GET req
def send_request(host, port, request):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(request.encode())
        response = b""
        while True:
            data = s.recv(4096)
            if not data:
                break
            response += data
        return response.decode()

if __name__ == "__main__":
    host = "127.0.0.1"
    port = 8080
    request = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"
    response = send_request(host, port, request)
    print("Risposta ricevuta:")
    print(response)
