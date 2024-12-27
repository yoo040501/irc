#include "../includes/Channel.hpp"

Channel::Channel() : ch_name(""), topic(""), key(""){}

Channel::Channel(std::string name, Client &cl) : ch_name(name), topic(""), key(""){
	client_fd.push_back(cl.getfd());
	ch_operator.push_back(cl.getNick());
}

Channel::Channel(Channel const& copy): client_fd(copy.client_fd), ch_name(copy.ch_name),
										topic(copy.topic), key(copy.key), ch_operator(copy.ch_operator),
										mode(copy.mode){}

Channel::~Channel() {
	client_fd.clear();
	mode.clear();
}

void	Channel::addClient(int fd){ client_fd.push_back(fd); }
void	Channel::addOper(std::string &oper){ ch_operator.push_back(oper); }
void	Channel::setName(std::string &n){ ch_name = n;}
void	Channel::setTopic(std::string &top){ topic = top; }
void	Channel::setKey(std::string &k){ key = k; }
void	Channel::setMode(std::string &flag){ mode.push_back(flag); }

int		Channel::getClient(int fd) {
	std::vector<int>::iterator it = std::find(client_fd.begin(), client_fd.end(), fd);
	if (it != client_fd.end())
		return *it;
	return 0;
}

bool	Channel::isOper(std::string &oper) {
	std::vector<std::string>::iterator it = std::find(ch_operator.begin(), ch_operator.end(), oper);
	if (it != ch_operator.end())
		return true;
	return false;
}

std::string	Channel::getName() const {return this->ch_name;}
std::string Channel::getTopic() const {return this->topic;}
std::string Channel::getKey() const {return this->key;}
std::string Channel::getMode(std::string &flag) {

}