#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 6667              // IRC 기본 포트
#define BUFFER_SIZE 1024       // 버퍼 크기

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 1. 소켓 생성
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Socket created successfully." << std::endl;

    // 2. 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. 소켓에 주소 할당 (bind)
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        close(server_fd);
        return EXIT_FAILURE;
    }
    std::cout << "Bind successful." << std::endl;

    // 4. 연결 대기
    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed." << std::endl;
        close(server_fd);
        return EXIT_FAILURE;
    }
    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        std::cout << "Waiting for a connection..." << std::endl;
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::cerr << "Accept failed." << std::endl;
            continue;
        }
        std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // 초기 IRC 핸드셰이크
        const char* welcome = ":localhost 001 irssi :Welcome to the Echo IRC Server\n";
        send(client_fd, welcome, strlen(welcome), 0);

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_read <= 0) {
                std::cout << "Client disconnected." << std::endl;
                break;
            }

            std::cout << "Received: " << buffer;

            // PING 처리
            if (strncmp(buffer, "PING", 4) == 0) {
                char pong_response[BUFFER_SIZE];
                snprintf(pong_response, BUFFER_SIZE, "PONG %s", buffer + 5);
                send(client_fd, pong_response, strlen(pong_response), 0);
                std::cout << "Sent: " << pong_response;
            } else {
                // 에코 메시지 전송
                send(client_fd, buffer, bytes_read, 0);
            }
        }
        close(client_fd);
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
