#include "../includes/Server.hpp"

bool Server::isPort(char *pt) // 1 ~ 65535
{
    std::string tmp(pt);
	std::istringstream iss(tmp);
	iss >> std::noskipws >> port;
    if (!iss.eof() || iss.fail())
        return false;
    if (port < 1 || port > 65535)
        return false;
    return true;
}

void    Server::setSockaddr(){
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
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
