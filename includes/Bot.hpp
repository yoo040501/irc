#pragma once

#include "Client.hpp"
#include "Message.hpp"

class Bot : public Client
{
	private:
		bool		isMode(std::string str);
		std::string	getFortune();
	public:
		Bot();
		Bot(Bot const& copy);
		Bot& operator=(Bot const& oth);
		~Bot();

		void	botMode(std::string str, Client &cl);
};
