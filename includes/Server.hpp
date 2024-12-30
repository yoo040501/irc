#pragma once

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server
{
    private:
        int								server_fd;
        struct  sockaddr_in				server_addr;
		struct	sockaddr_in				client_addr;
        std::string						password;
        int								port;
        std::vector<struct kevent>		changeList; // vector로 안해도 될듯
        std::vector<struct kevent>		eventList;
        std::map<int, Client>			client; // clientfd , Client
        std::map<std::string, int>		nick;   // nickname, clientfd
		std::map<std::string, Channel>	channel; //channelname , Channel
		std::stack<std::string>			rebuffer;

        bool	isPort(char *pt);
		void	setSockaddr();
		void	createEvent(int fd);
		bool	generateClient();

		void	checkCommand(char *buffer, Client &cl);
		void	nickCheck(std::string str, Client &cl);
		bool	passCheck(std::string str, Client &cl);
		void	passFail(Client &cl);
		void	userCheck(std::string str, Client &cl);
		void	msgCheck(std::string str, Client &cl);
		void	channelCheck(std::string str, Client &cl);
		void	kickCheck(std::string str, Client &cl);
		void	topicCheck(std::string str, Client &cl);
		void	modeCmd(std::string str, Client &cl);

        Server();
        Server(Server const& copy);
        Server& operator=(Server const& oth);
    public:
        Server(char *pt, char *pw);
        ~Server();

        void	openSocket();
        void	active();
};

std::string	trimSpace(std::string str);
void		getCHName(std::istringstream &iss, std::vector<std::string> &CH_name, Client &cl);
std::string	getCMD(std::string &str);
void		sendMsg(std::string msg, int fd);
void		sendTopic(Channel &CH, Client &cl, std::map<int, Client> cl_tmp);
bool		isExistCH(std::string name, std::map<std::string, Channel> &channel);
bool		isExistUSER(std::string name, std::map<std::string, int>&nick);
