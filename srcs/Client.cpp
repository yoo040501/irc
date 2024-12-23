#include "../includes/Client.hpp"

Client::Client(): fd(-1), pass(false), nick(""), username(""), hostname(""), 
					servername(""), realname(""), mode(""), addr(), len(0){
}

Client::Client(Client const& copy): fd(copy.fd), nick(copy.nick),
									username(copy.username), hostname(copy.hostname), servername(copy.servername),
									realname(copy.realname), mode(copy.mode), addr(copy.addr), len(copy.len){
}

Client::~Client(){}

void	Client::setfd(int f){
	fd = f;
}

void	Client::setaddr(struct sockaddr_in &tmp){
	addr = tmp;
	len = sizeof(tmp);
}

int	Client::getfd() const {return this->fd;}

bool	Client::getPass() const {return this->pass;}

std::string	Client::getNick() const {return this->nick;}

std::string	Client::getUser() const {return this->username;}

struct sockaddr_in	Client::getaddr() const { return this->addr;}
