#include "../includes/Client.hpp"

Client::Client(): fd(-1), pass(false), passcheck(false), auth(false), nick("*"), username(""), hostname(""), 
					servername(""), realname(""), mode(""), addr(), len(0){
}

Client::Client(Client const& copy): fd(copy.fd), pass(copy.pass), passcheck(copy.passcheck), auth(copy.auth), nick(copy.nick),
									username(copy.username), hostname(copy.hostname), servername(copy.servername),
									realname(copy.realname), mode(copy.mode), addr(copy.addr), len(copy.len){
}

Client::~Client(){}

void	Client::setfd(int f){
	fd = f;
}

void	Client::setPass(bool flag){
	pass = flag;
}

void	Client::setPassCheck(bool flag){
	passcheck = flag;
}

void	Client::setAuth(bool flag){
	auth = flag;
}

void	Client::setNick(std::string const &ni){
	nick = ni;
}

void	Client::setUser(std::string const &us, std::string const &hs, std::string const &sn, std::string const &rn){
	username = us;
	hostname = hs;
	servername = sn;
	realname = rn;
}

void	Client::setaddr(struct sockaddr_in &tmp){
	addr = tmp;
	len = sizeof(tmp);
}

int	Client::getfd() const {return this->fd;}

bool	Client::getPass() const {return this->pass;}

bool	Client::getPassCheck() const {return this->passcheck;}

bool	Client::getAuth() const {return this->auth;}

std::string	Client::getNick() const {return this->nick;}

std::string	Client::getUser() const {return this->username;}

struct sockaddr_in	Client::getaddr() const { return this->addr;}
