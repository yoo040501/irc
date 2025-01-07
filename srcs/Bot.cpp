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

void	Bot::botMode(std::string str, Client &cl){
	if (str.empty()){
		return sendMsg("This bot provides 1.Today's fortune cookie, 2.Rock-Scissors-Paper Games\r\nplease enter Number or Mode(cookie, game)\r\n", cl.getfd());
	}
}
// bot