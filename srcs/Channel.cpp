#include "../includes/Channel.hpp"

Channel::Channel() : ch_name(""), topic(""), key(""){}

Channel::Channel(std::string name, Client &cl) : ch_name(name), topic(""), key(""){
	client[cl.getfd()] = cl;
	ch_operator.push_back(cl.getNick());
}

Channel::Channel(Channel const& copy): client(copy.client), ch_name(copy.ch_name),
										topic(copy.topic), key(copy.key), ch_operator(copy.ch_operator),
										mode(copy.mode){}

Channel::~Channel() {
	client.clear();
	mode.clear();
}

void	Channel::addClient(Client &cl){ client[cl.getfd()] = cl; }
void	Channel::addOper(std::string &oper){ ch_operator.push_back(oper); }
void	Channel::setName(std::string &n){ ch_name = n;}
void	Channel::setTopic(std::string &top){ topic = top; }
void	Channel::setKey(std::string &k){ key = k; }
void	Channel::setMode(std::string &flag){ mode.push_back(flag); }
void	Channel::removeOper(std::string &oper) {
	std::vector<std::string>::iterator it = std::find(ch_operator.begin(), ch_operator.end(), oper);
	if (it != ch_operator.end())
		ch_operator.erase(it);
}

int		Channel::getClientfd(int const fd) {
	std::map<int, Client>::iterator it = client.find(fd);
	if (it != client.end())
		return it->first;
	return -1;	
}

bool	Channel::isOper(std::string &oper) {
	std::vector<std::string>::iterator it = std::find(ch_operator.begin(), ch_operator.end(), oper);
	if (it != ch_operator.end())
		return true;
	return false;
}

std::map<int, Client>	Channel::getClient() const {return this->client;}
std::string	Channel::getName() const {return this->ch_name;}
std::string Channel::getTopic() const {return this->topic;}
std::string Channel::getKey() const {return this->key;}
std::string Channel::getMode(std::string &flag) {
	return flag;
}
