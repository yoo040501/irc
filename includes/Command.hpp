





#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Message.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Command {
  public:
	Command(Server &server);
	~Command();

    void invite(Client &client, std::vector<std::string> args);
	std::string NEEDMOREPARAMS(std::string nickname, std::string command);
  
  private:
	Command &operator=(const Command&);
	Command(const Command&);
	
	Server		&server_;
	
};

#endif
