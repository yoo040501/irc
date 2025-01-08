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
#include "Bot.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "ErrorMsg.hpp"

class Bot;
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
		std::string						servertime;
        std::vector<struct kevent>		changeList;
        std::vector<struct kevent>		eventList;
        std::map<int, Client>			client; // clientfd , Client
        std::map<std::string, int>		nick;   // nickname(lower), clientfd
		std::map<std::string, Channel>	channel; //channelname(lower), Channel
		std::stack<std::string>			rebuffer;
		static std::map<int, Client> 	clients;
  		static std::map<std::string, Channel> channels;
		Bot								*bot;

        bool	isPort(char *pt);
		void	setSockaddr(struct  sockaddr_in	&server_addr);
		void	createEvent(int fd);
		bool	generateClient();
		void	generateBot();

		void	checkCommand(char *buffer, Client &cl);
		void	processAuth(std::string &str, Client &cl);
		void	nickCheck(std::string str, Client &cl);
		bool	passCheck(std::string str, Client &cl);
		void	closeClient(std::string msg, Client &cl);
		void	userCheck(std::string str, Client &cl);
		void	channelCheck(std::string str, Client &cl);
		void	kickCheck(std::string str, Client &cl);
		void	topicCheck(std::string str, Client &cl);
		void	partCheck(std::string str, Client &cl);
		void	quitCheck(std::string str, Client &cl);
		void	operateFlag(Channel &ch, Client &cl, std::istringstream& iss, std::pair<std::string, std::string>& success, char op);
		
		// Mode
		void	modeCmd(std::string str, Client &cl);
		void	checkFlag(std::map<std::string, Channel>::iterator &it, std::istringstream& iss,  Client &cl, std::string& target);
		void	channelMode(std::map<std::string, Channel>::iterator &it, std::istringstream& iss, Client &cl);
		bool	isServerUser(std::string user);

		// Bot
		bool    isBot(std::istringstream& iss, Client &cl, std::string& str);
		
		// Privmsg
		void    	privmsgCmd(std::string str, Client &cl);
		bool		isValidChname(std::string& chName);
		Channel& 	getChannel(std::string& chName);
		Client&		getClient(std::string &clName);

		// Invite
		void	handleInviteErrors(Client &inviter, Channel *channel, Client *invitee, const std::string &chanName, const std::string &inviteeName);
		void	sendInviteMsg(Channel &channel, Client &inviter, Client &invitee);

        Server();
        Server(Server const& copy);
        Server& operator=(Server const& oth);
    public:
        Server(char *pt, char *pw);
        ~Server();

        void	openSocket();
        void	active();
		static std::map<int, Client>& getClients();
  		static std::map<std::string, Channel>& getChannels();
};

std::string	trimSpace(std::string str);
void		getCHName(std::string &str, std::vector<std::string> &CH_name, Client &cl);
void		getUserName(std::string &str, std::vector<std::string> &USER);
std::string	getCMD(std::string &str);
void		sendTopic(Channel &CH, Client &cl, std::map<int, Client> cl_tmp);
bool		isExistCH(std::string name, std::map<std::string, Channel> &channel);
bool		isExistUSER(std::string name, std::map<std::string, int>&nick);
bool		hasDuplicate(std::vector<std::string>& success, std::string& target);
std::vector<std::string>	changeLowerChannelname(std::vector<std::string> CH_name);
