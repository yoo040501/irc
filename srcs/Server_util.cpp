#include "../includes/Server.hpp"

bool Server::isPort(char *pt) // 1 ~ 65535
{
    std::string tmp(pt);
	std::istringstream iss(tmp);
	iss >> std::noskipws >> port;
    if (!iss.eof() || iss.fail())
        return false;
    if (port < 1024 || port > 65535)
        return false;
    return true;
}

void    Server::setSockaddr(struct  sockaddr_in &_server_addr){
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(port);
}

void    Server::createEvent(int fd)
{
    struct kevent event_tmp;
    EV_SET(&event_tmp, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    changeList.push_back(event_tmp);
}

bool	Server::generateClient(){
	Client		cl;
	socklen_t   client_len = sizeof(client_addr);
	
	cl.setfd(accept(server_fd,(struct sockaddr*)&client_addr, &client_len));  // 클라이언트 연결 허용 
	if (cl.getfd() < 0)
		return false;
	cl.setaddr(client_addr);
	fcntl(cl.getfd(), F_SETFL, O_NONBLOCK);
	createEvent(cl.getfd());
	client.insert(std::make_pair(cl.getfd(), cl));
	sendMsg(WAITING(), cl.getfd());
	return true;
}

void	Server::generateBot(){
	Bot					*_bot = new Bot();
    struct sockaddr_in	bot_addr;
    socklen_t			bot_len = sizeof(bot_addr);

    // 봇 클라이언트 소켓 생성
    int bot_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (bot_fd < 0) {
        throw SocketFailException();
    }

	setSockaddr(bot_addr);
    if (connect(bot_fd, (struct sockaddr*)&bot_addr, bot_len) < 0) {
        close(bot_fd);
        throw SocketFailException();
    }
    fcntl(bot_fd, F_SETFL, O_NONBLOCK);

    _bot->setfd(bot_fd);
    _bot->setaddr(bot_addr);

    createEvent(_bot->getfd());
    bot = _bot;
    std::cout << "Bot making Successfully\n";
}

// 원래 구현되어 있던 Server.cpp 코드
Channel& Server::getChannel(std::string &chName){
    std::map<std::string, Channel>::iterator it;

    for (it = channel.begin(); it != channel.end(); ++it){
        if (it->first == chName)
            break;
    }
    return it->second;
}

bool Server::isServerUser(std::string user){
    std::map<int, Client>::iterator it;

    for (it = client.begin(); it != client.end(); ++it){
        if (it->second.getLowNick() == user)
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

Client& Server::getClient(std::string &clName){
    std::map<int, Client>::iterator it;

    for (it = client.begin(); it != client.end(); ++it){
        if (it->second.getNick() == clName)
            break;
    }
    return it->second;
}

Channel* Server::findChannel(const std::string &channelName) {
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        return NULL;  // 채널을 찾지 못한 경우 nullptr 반환
    }
    return &(it->second);  // 채널 객체의 주소 반환
}

Client* Server::findClient(const std::string &nick) {
    std::map<int, Client>::iterator it = clients.begin();
    while (it != clients.end()) {
        std::string tmpnick = it->second.getNick();
        if (tmpnick == nick) {
            return &(it->second); // Client 객체의 주소 반환
        }
        ++it;
    }
    return NULL; // 클라이언트를 찾지 못한 경우 nullptr 반환
}

void Server::sendInviteMsg(Channel &channel, Client &inviter, Client &invitee) {
    sendMsg(RPL_INVITING(inviter.getNick(), invitee.getNick(), channel.getName()), inviter.getfd());
    sendMsg(":localhost INVITE " + invitee.getNick() + " " + channel.getName() + "\r\n", invitee.getfd());
    channel.addClient(invitee);
}

std::map<std::string, Channel> Server::getChannels() { return this->channel; }
std::map<int, Client> Server::getClients() { return this->client; }
