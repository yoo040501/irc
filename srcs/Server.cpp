#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

std::map<int, Client> Server::clients;
std::map<std::string, Channel> Server::channels;

Server::Server(char *pt, char *pw)
    : command_(*this) {  // Command 객체를 초기화하면서 Server 객체 참조 전달
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
        throw std::bad_exception();

    setSockaddr();
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_fd);
        throw std::bad_exception();
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        throw std::bad_exception();
    }
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
}

void Server::active() {
    int kq = kqueue();
    if (kq < 0) {
        close(server_fd);
        throw std::bad_exception();
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
            throw std::bad_exception();
        }
        changeList.clear();
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
                        closeClient("disconnected", clients[eventList[i].ident]);
                    } else {
                        if (clients.find(eventList[i].ident) == clients.end())
                            clients[eventList[i].ident] = Client();
                        checkCommand(buffer, clients[eventList[i].ident]);
                    }
                }
            }
        }
        if (static_cast<int>(eventList.size()) == new_events)
            eventList.resize(eventList.size() * 2);
    }
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

void Server::handleInviteCommand(Client &cl, const std::string &argsStr) {
    std::istringstream argStream(trimSpace(argsStr));
    std::vector<std::string> args;
    std::string token;

    while (argStream >> token) {
        args.push_back(token);
    }

    if (args.size() < 2) {
        sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "INVITE"), cl.getfd());
        return;
    }

    std::string targetNick = args[0];
    std::string channelName = args[1];

    Channel* channel = findChannel(channelName);
    if (!channel) {
        sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channelName), cl.getfd());
        return;
    }

    Client* targetClient = findClient(targetNick);
    if (!targetClient) {
        sendMsg(ERR_NOSUCHNICK(cl.getNick(), targetNick), cl.getfd());
        return;
}

    if (!channel->isChannelUser(cl.getNick())) {
        sendMsg(ERR_NOTONCHANNEL(cl.getNick(), channelName), cl.getfd());
        return;
    }
    if (!channel->isOperator(cl.getNick())) {
        sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), channelName), cl.getfd());
        return;
    }
    if (channel->isChannelUser(targetClient->getNick())) {
        sendMsg(ERR_USERONCHANNEL(cl.getNick(), targetNick, channelName), cl.getfd());
        return;
    }

    sendInviteMsg(*channel, cl, *targetClient);
}

std::map<std::string, Channel>& Server::getChannels() { return channels; }
std::map<int, Client>& Server::getClients() { return clients; }
