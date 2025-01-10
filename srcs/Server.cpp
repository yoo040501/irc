#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

std::map<int, Client> Server::clients;
std::map<std::string, Channel> Server::channels;

Server::Server(char *pt, char *pw) {  // Command 객체를 초기화하면서 Server 객체 참조 전달
    if (isPort(pt) == false)
        throw std::invalid_argument("check port number");
    password = std::string(pw);
}

Server::~Server() {}

void Server::openSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (server_fd < 0)
        throw SocketFailException();

    setSockaddr(this->server_addr);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_fd);
        throw BindFailException();
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        throw ListenFailException();
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
	// 서버가 여러 클라이언트와 동시에 통신해야 하는 경우, 비차단 모드는 필수적
    // 하나의 클라이언트 작업이 완료될 때까지 기다리지 않고 다른 클라이언트의 요청을 처리할 수 있다
}

void Server::active() {
    int kq = kqueue();
    if (kq < 0) {
        close(server_fd);
        throw KqueueFailException();
    }
    eventList.resize(100);
    createEvent(server_fd);
    time_t t = time(NULL);
    servertime = ctime(&t);

    int new_events;
    while (1) {
        std::cout << "Waiting for client" << std::endl;
        new_events = kevent(kq, changeList.data(), changeList.size(), eventList.data(), eventList.size(), NULL);
        if (new_events < 0) {
            close(server_fd);
            close(kq);
            throw KeventFailException();
        }
        changeList.clear();
		if (!bot)
			generateBot();
        for (int i = 0; i < new_events; i++) {
            if (eventList[i].flags & EV_ERROR) {
                close(eventList[i].ident);
                continue;
            } else if (eventList[i].filter == EVFILT_READ) {
                if (eventList[i].ident == static_cast<unsigned long>(server_fd)) {
                    if (generateClient() == false)
                        continue;
                } else {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int recv_len = recv(eventList[i].ident, buffer, sizeof(buffer), 0);
                    if (recv_len <= 0) {
                        std::cout << "Client disconnected." << std::endl;
                        closeClient("disconnected", client[eventList[i].ident]);
                    } else {
                        if (client.find(eventList[i].ident) == client.end())
                            client[eventList[i].ident] = Client();
                        checkCommand(buffer, client[eventList[i].ident]);
                    }
                }
            }
        }
        if (static_cast<int>(eventList.size()) == new_events)
            eventList.resize(eventList.size() * 2);
    }
}
