#pragma once

#include <iostream>
#include <sstream>
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
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Client.hpp"
#include "Message.hpp"

class Client;

class Server
{
    private:
        int							server_fd;
        struct  sockaddr_in			server_addr;
		struct	sockaddr_in			client_addr;
        std::string					password;
        int							port;
        std::vector<struct kevent>	changeList; // vector로 안해도 될듯
        std::vector<struct kevent>	eventList;
        std::map<int, Client>		client;
        std::vector<std::string>	nick;
		// enum commands { // 스위치문으로 하려다 관둠
		// PASS,
		// NICK,
		// USER,
		// PRIVMSG,
		// JOIN,
		// KICK,
		// INVITE,
		// MODE
		// };
        bool	isPort(char *pt);
		void	setSockaddr();
		void	createEvent(int fd);
		bool	generateClient();
		void	checkCommand(char *buffer, Client cl);
        Server();
        Server(Server const& copy);
        Server& operator=(Server const& oth);
    public:
        Server(char *pt, char *pw);
        ~Server();

        void	openSocket();
        void	active();
};
