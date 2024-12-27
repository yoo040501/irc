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


/* 첫 방 생성자는 무조건 오퍼레이터, 오퍼레이터가 다른 유저에게 오퍼레이터 권한을 줄수 있음
	인자를 뭐로 설정해야될지 머리 터지겠ddarjwleryt*/
class Channel
{
	private:
		std::map<int, Client>		client;
		std::string					ch_name;
		std::string					topic;
		std::string					key;
		std::vector<Client>			ch_operator;
		std::vector<std::string>	mode; // o|p|s|i|t|n|b|v
		Channel& operator=(Channel const& oth);
	public:
		Channel();
		Channel(std::string name, Client &cl);
		Channel(Channel const &copy);
		~Channel();

		void	addClient(int fd);
		void	addOper(std::string &oper);
		void	setName(std::string &n);
		void	setTopic(std::string &top);
		void	setKey(std::string &k);
		void	setMode(std::string &flag);

		int			getClient(int fd);
		bool		isOper(std::string &oper);
		std::string getName() const;
		std::string getTopic() const;
		std::string getKey() const;
		std::string getMode(std::string &flag);

};
