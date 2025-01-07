#pragma once

#include "Client.hpp"
#include "Message.hpp"
// #include "Server.hpp"

// class Server;

class Bot : public Client
{
	private:
		std::vector<Client *> cl;
	public:
		Bot();
		Bot(Bot const& copy);
		Bot& operator=(Bot const& oth);
		~Bot();

		void	botMode(std::string str, Client &cl);
};
