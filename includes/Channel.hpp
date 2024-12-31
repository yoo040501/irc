#pragma once

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Client.hpp"

class Client;

class Channel
{
	private:
		std::map<int, Client>		client; 
		std::string					ch_name;
		std::string					topic;
		std::time_t					topic_time;
		int							topic_user;
		std::string					key;
		std::vector<std::string>	ch_operator; //string으로 하면 nick변경될때 server의 channel iterator로 돌면서 client의 oldnick확인
		std::vector<std::string>	mode; // o|p|s|i|t|n|b|v|l|k
		int 						limit;
		Channel& operator=(Channel const& oth);
	public:
		Channel();
		Channel(std::string name, Client &cl);
		Channel(Channel const &copy);
		~Channel();

		void	addClient(Client &cl);
		void	addOper(std::string &oper);
		void	removeOper(std::string &oper);
		void	removeClient(std::string nick);
		void	setClient(int fd, Client &cl);
		void	setName(std::string &n);
		void	setTopic(std::string &top, int fd);
		void	setKey(std::string &k);
		void	setMode(std::string &flag);
		void	removeMode(std::string &flag);
		bool	alreadySetMode(std::string &flag);
		void	printMode();

		int			getClientfd(int fd);
		bool		isOper(std::string &oper);
		std::map<int, Client> getClient() const;
		std::string getName() const;
		std::string getTopic() const;
		std::time_t	const& getTPTime() const;
		int			getTPUser() const;
		std::string getKey() const;
		std::string getMode(std::string &flag);

};

bool	inCH(Channel &CH, std::string user);
