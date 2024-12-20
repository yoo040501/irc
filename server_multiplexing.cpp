#include <iostream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    if (server_fd < 0)
        exit(1);
    std::cout << "Server socket created successfully." << std::endl;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6667);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // 소켓 주소 할당
    {
        close(server_fd);
        exit(1);
    }
    std::cout << "Server address structure created successfully." << std::endl;

    if (listen(server_fd, 5) < 0){                                                  // 소켓 연결요청
        close(server_fd);
        exit(1);
    }
    std::cout << "Server listening on port 6667." << std::endl;
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    int kq = kqueue();
    if (kq < 0)
    {
        close(server_fd);
        exit(1);
    }
    std::vector<struct kevent> changeList; // vector로 안해도 될듯
    std::vector<struct kevent> eventList(100);

    struct kevent event_tmp;
    EV_SET(&event_tmp, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    changeList.push_back(event_tmp);
    std::cout << "Server kqueue created successfully." << std::endl;

    int new_events;
    while (1)
    {
        std::cout << "Waiting for connection" << std::endl;
        new_events = kevent(kq, changeList.data(), changeList.size(), eventList.data(), eventList.size(), NULL);
        if (new_events < 0) // error
        {
            close(server_fd);
            close(kq);
            exit(1);
        }
        changeList.clear(); //kevent 함수를 호출할 때마다 changeList를 비워줘야 함
        for (int i = 0; i < new_events; i++)
        {
            if (eventList[i].flags & EV_ERROR) // error
            {
                close(eventList[i].ident);
                continue;
            }
            else if (eventList[i].filter == EVFILT_READ) // read event
            {
                if (eventList[i].ident == server_fd) // server socket
                {
                    socklen_t   client_len = sizeof(client_addr);
                    client_fd = accept(server_fd,(struct sockaddr*)&client_addr, &client_len);  // 클라이언트 연결 허용
                    if (client_fd < 0)
                        continue;
                    fcntl(client_fd, F_SETFL, O_NONBLOCK);

                    struct kevent event_tmp;
                    EV_SET(&event_tmp, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                    changeList.push_back(event_tmp);
                    std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << std::endl;
                    const char* welcome = ":localhost 001 : Welcome to the Localnet IRC Network\n";
                    send(client_fd, welcome, strlen(welcome), 0);
                    const char* two = ":localhost 002 :Your host is irc.local, running version one!\n";
                    send(client_fd, two, strlen(two), 0);
                    const char* three = ":localhost 003 :This server was created ?!?!\n";
                    send(client_fd, three, strlen(three), 0);
                    const char* four = ":localhost 004 :localhost IRC_SERVER iosw biklmnopstv :bklov\n";
                    send(client_fd, four, strlen(four), 0);
                    const char* startt = ":localhost 375 :localhost message of the day\n";
                    send(client_fd, startt, strlen(startt), 0);
                    const char* endd = ":localhost 376 a :End of message of the day.\n";
                    send(client_fd, endd, strlen(endd), 0);
                }

                else // client socket
                {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));

                    int recv_len = recv(eventList[i].ident, buffer, sizeof(buffer), 0);
                    if (recv_len <= 0)
                    {
                        close(eventList[i].ident);
                        struct kevent event_tmp;
                        EV_SET(&event_tmp, eventList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                        changeList.push_back(event_tmp);
                        std::cout << "Client disconnected." << std::endl;
                    }
                    else
                    {
                        // if (strncmp(buffer, "PING" , 4) == 0)
                        // {
                        //     send(eventList[i].ident, "PONG", 4, 0);
                        // }
                        if (strncmp(buffer, "JOIN", 4) == 0)
                            send(eventList[i].ident, "HELLO!", 6, 0);
                        else if (strncmp(buffer, "NICK", 4) == 0)
                            send(eventList[i].ident, "SEtNickName", 11, 0);
                        send(eventList[i].ident, buffer, recv_len, 0); // 에코서버
                    }
                }
            }
            else if (eventList[i].filter == EVFILT_WRITE) // 아마 쓸일은 없을듯 함
            {
                //write event
            }
        }
        if (static_cast<int>(eventList.size()) == new_events) // eventList가 꽉 찼을 때
            eventList.resize(eventList.size() * 2);
    }
}