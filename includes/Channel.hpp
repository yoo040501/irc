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
		std::time_t					channel_time;
		int							topic_user;
		std::string					key;
		std::vector<std::string>	ch_operator; //string으로 하면 nick변경될때 server의 channel iterator로 돌면서 client의 oldnick확인
		std::vector<std::string>	voiceUser; //string으로 하면 nick변경될때 server의 channel iterator로 돌면서 client의 oldnick확인
		std::vector<std::string>	mode; // o|p|s|i|t|n|b|v|l|k
		long						limit;
		Channel& operator=(Channel const& oth);
	public:
		Channel();
		Channel(std::string name, Client &cl);
		Channel(Channel const &copy);
		~Channel();

		void	addClient(Client &cl);
		void	addOper(std::string &oper);
		void	addVoiceUser(std::string &user);
		void	removeOper(std::string &oper);
		void	removeClient(std::string nick);
		bool	removeVoiceUser(std::string &user);
		void	setClient(int fd, Client &cl);
		void	setName(std::string &n);
		void	setTopic(std::string &top, int fd);
		void	setKey(std::string &k);
		void	setLimit(long value);
		void	setMode(std::string &flag);
		void	removeMode(std::string &flag);
		
		bool	findMode(std::string flag);
		void	printMode();
		
		bool	isChannelUser(std::string &user);
		bool	isVoiceUser(std::string &user);
		bool	isOperator(std::string &user);

		int		getClientfd(int fd);
		bool	isOper(std::string &oper);
		std::map<int, Client> getClient() const;
		std::string getName() const;
		std::string getTopic() const;
		std::time_t	const& getTPTime() const;
		int			getTPUser() const;
		std::string getKey() const;
		std::string getMode(std::string &flag);
		long		getLimit();
		void 		inviteClient(int fd); // 초대 클라이언트 관리용 메서드 추가



};

bool	inCH(Channel &CH, std::string user);
