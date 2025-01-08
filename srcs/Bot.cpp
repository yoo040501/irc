#include "../includes/Bot.hpp"

Bot::Bot():Client(){
	nick = "bot";
	lownick = "bot";
	oldnick = "bot";
	passcheck = true;
	pass = true;
	auth = true;
}

Bot::Bot(Bot const& copy):Client(copy){}

Bot&	Bot::operator=(Bot const& oth){
	if (this != &oth){
		Client::operator=(oth);
	}
	return *this;
}

Bot::~Bot(){
}

std::string	Bot::getFortune(){
	std::string Fortunes[10] = {
		"If you overlook it, you might miss something big.\r\n",
		"Worrying won't change anything, so be confident.\r\n",
		"Good fortune will come if you wait patiently.\r\n",
		"Positive thoughts lead to positive outcomes.\r\n",
		"Helping others will bring rewards in the future.\r\n",
		"Your sincerity and dedication will earn praise from others.\r\n",
		"You might hear unexpected news, so stay prepared.\r\n",
		"Strive to gain recognition from your superiors.\r\n",
		"Sticking to the basics is the right answer.\r\n",
		"Unexpected luck will find you when you least expect it.\r\n"
	};
	int	idx = rand() % 10;
	return Fortunes[idx];
}

bool	Bot::isMode(std::string str){
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	if (str == "1" || str == "fortune")
		return true;
	return false;
}

void	Bot::botMode(std::string str, Client &cl){
	if (cl.getBot() == false){ // bot mode 처음 들어올때
		cl.setBot(true);
		if (str == "1" || str == "fortune"){
			std::string message = getFortune();
			sendMsg(message, cl.getfd());
			cl.setBot(false);
		}
		else {
			sendMsg("Hello " + cl.getNick() + "!!!\r\n", cl.getfd());
			sendMsg("This bot provides 1.Today's fortune.\r\nplease enter Number or Mode(fortune)\r\n", cl.getfd());
		}
	}
	else{
		if (!isMode(str))
			sendMsg("This bot only takes 1 and fortune. Try again!\r\n", cl.getfd());
		else{
			std::string message = getFortune();
			sendMsg(message, cl.getfd());
			cl.setBot(false);
		}
	}
}
