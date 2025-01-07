#include "../includes/Server.hpp"

std::map<int, Client> Server::clients;
std::map<std::string, Channel> Server::channels;

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
		throw SocoketFailException();

    setSockaddr();
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // 소켓 주소 할당
    {
        close(server_fd);
        throw BindFailException();
    }
    if (listen(server_fd, 5) < 0){                                                  // 소켓 연결요청
        close(server_fd);
        throw ListenFailException();
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK); // 서버가 여러 클라이언트와 동시에 통신해야 하는 경우, 비차단 모드는 필수적
    // 하나의 클라이언트 작업이 완료될 때까지 기다리지 않고 다른 클라이언트의 요청을 처리할 수 있다
}

void	Server::active(){	//멀티플렉싱
	int kq = kqueue();
    if (kq < 0)
    {
        close(server_fd);
        throw KqueueFailException();
    }
    eventList.resize(100); //이벤트 크기를 100으로 먼저 설정
    createEvent(server_fd);
	time_t	t = time(NULL);
	servertime = ctime(&t);

    int new_events;
    while (1)
    {
        std::cout << "Waiting for client" << std::endl;
        new_events = kevent(kq, changeList.data(), changeList.size(), eventList.data(), eventList.size(), NULL);
        if (new_events < 0) // error
        {
            close(server_fd);
            close(kq);
            throw KeventFailException();
        }
        changeList.clear(); //kevent 함수를 호출할 때마다 changeList를 비워줘야 함
		//비우지않으면 이전에 등록된 이벤트가 다시 등록될수 있음
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
                        std::cout << "Client disconnected." << std::endl;
						closeClient("disconnected", client[eventList[i].ident]);
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

bool Server::isServerUser(std::string &user){
    std::map<int, Client>::iterator it;

    for (it = client.begin(); it != client.end(); ++it){
        if (it->second.getNick() == user)
            return true;
    }
    return false;
}

bool Server::isValidChname(std::string &chName){
    std::map<std::string, Channel>::iterator it;

    for (it = channel.begin(); it != channel.end(); ++it){
        if (it->first == chName)
            return true;
    }
    return false;
}

Channel& Server::getChannel(std::string &chName){
    std::map<std::string, Channel>::iterator it;

    for (it = channel.begin(); it != channel.end(); ++it){
        if (it->first == chName)
            break;
    }
    return it->second;
}

Client& Server::getClient(std::string &clName){
    std::map<int, Client>::iterator it;

    for (it = client.begin(); it != client.end(); ++it){
        if (it->second.getNick() == clName)
            break;
    }
    return it->second;
}

void Server::sendInviteMsg(Channel &channel, Client &inviter, Client &invitee) {
    sendMsg(RPL_INVITING(inviter.getNick(), invitee.getNick(), channel.getName()), inviter.getfd());
    sendMsg(":localhost INVITE " + invitee.getNick() + " " + channel.getName() + "\r\n", invitee.getfd());
    channel.addClient(invitee);
}

void Server::handleInviteErrors(Client &inviter, Channel *channel, Client *invitee, const std::string &chanName, const std::string &inviteeName) {
    if (!inviter.getPass() || inviter.getUser().empty() || inviter.getNick() == "*") {
        throw std::logic_error(ERR_NOTREGISTERED(inviter.getNick()));
    }
    if (!channel) {
        throw std::logic_error(ERR_NOSUCHCHANNEL(inviter.getNick(), chanName));
    }
    if (!invitee) {
        throw std::logic_error(ERR_NOSUCHNICK(inviter.getNick(), inviteeName));
    }
    if (!channel->isChannelUser(inviter.getNick())) {
        throw std::logic_error(ERR_NOTONCHANNEL(inviter.getNick(), chanName));
    }
    if (!channel->isOperator(inviter.getNick())) {
        throw std::logic_error(ERR_CHANOPRIVSNEEDED(inviter.getNick(), chanName));
    }
    if (channel->isChannelUser(invitee->getNick())) {
        throw std::logic_error(ERR_USERONCHANNEL(inviter.getNick(), inviteeName, chanName));
    }
}

// invite
std::map<std::string, Channel> &Server::getChannels() { return channels; }
std::map<int, Client> &Server::getClients() { return clients; }
