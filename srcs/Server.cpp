#include "../includes/Server.hpp"

Server::Server(char *pt, char *pw){
	if (isPort(pt) == false)
		throw std::invalid_argument("check port number");
    password = std::string (pw); //패스워드값이 무엇인지 아직 확인 X
}

Server::~Server(){}

void	Server::openSocket(){
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성
    int optval = 1; //sockopt 포트 재사용
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (server_fd < 0)
		throw std::bad_exception();
    std::cout << "Server socket created successfully." << std::endl;

    setSockaddr();
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // 소켓 주소 할당
    {
        close(server_fd);
        throw std::bad_exception();
    }
    std::cout << "Server address structure created successfully." << std::endl;

    if (listen(server_fd, 5) < 0){                                                  // 소켓 연결요청
        close(server_fd);
        throw std::bad_exception();
    }
    std::cout << "Server listening on port." << std::endl;
    fcntl(server_fd, F_SETFL, O_NONBLOCK); // 서버가 여러 클라이언트와 동시에 통신해야 하는 경우, 비차단 모드는 필수적
    // 하나의 클라이언트 작업이 완료될 때까지 기다리지 않고 다른 클라이언트의 요청을 처리할 수 있다
}

void	Server::active(){	//멀티플렉싱
	int kq = kqueue();
    if (kq < 0)
    {
        close(server_fd);
        throw std::bad_exception();
    }
    eventList.resize(100); //이벤트 크기를 100으로 먼저 설정
    createEvent(server_fd);
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
            throw std::bad_exception();
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
                if (eventList[i].ident == static_cast<unsigned long>(server_fd)) // client와 처음 연결됬을때
                {
					if (generateClient() == false)
						continue;
                }
                else // client socket
                {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));

                    int recv_len = recv(eventList[i].ident, buffer, sizeof(buffer), 0);
                    if (recv_len <= 0)
                    {
                        close(eventList[i].ident);
                        createEvent(eventList[i].ident);
						client.erase(eventList[i].ident);
                        std::cout << "Client disconnected." << std::endl;
                    }
                    else
                    {
						if (client.find(eventList[i].ident) == client.end())
							client[eventList[i].ident] = Client(); // Client 객체를 생성
						checkCommand(buffer, client[eventList[i].ident]); // 명령어 확인 / 인자로 클라이언트 정보 넣어줌
                    }
                }
            }
        }
        if (static_cast<int>(eventList.size()) == new_events) // eventList가 꽉 찼을 때
            eventList.resize(eventList.size() * 2);
    }
}
