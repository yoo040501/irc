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
void	Channel::setTopic(std::string &top, int fd){ topic = top; topic_time = time(NULL); topic_user = fd;}
void	Channel::setKey(std::string &k){ key = k; }

void	Channel::setMode(std::string &flag){
    std::vector<std::string>::iterator it = std::find(mode.begin(), mode.end(), flag);
	if (it == mode.end())
		mode.push_back(flag); 
}

void	Channel::removeMode(std::string &flag){
	std::vector<std::string>::iterator it = std::find(mode.begin(), mode.end(), flag);
	if (it != mode.end())
		mode.erase(it);
}

void	Channel::printMode(){
	std::cout << "channel mode" << std::endl;
	for(std::vector<std::string>::iterator it = mode.begin(); it < mode.end(); it++){
		std::cout << *it << " ";
	}
	std::cout << std::endl;
}

void	Channel::setClient(int fd, Client &cl){
	client[fd] = cl;
}

void	Channel::removeOper(std::string &oper) {
	std::vector<std::string>::iterator it = std::find(ch_operator.begin(), ch_operator.end(), oper);
	if (it != ch_operator.end())
		ch_operator.erase(it);
}
void	Channel::removeClient(std::string nick){
	std::map<int, Client>::iterator it = client.begin();
	while (it != client.end()){
		if (it->second.getNick() == nick){
			client.erase(it);
			break;
		}
		++it;
	}
}

int		Channel::getClientfd(int const fd) {
	std::map<int, Client>::iterator it = client.find(fd);
	if (it != client.end())
		return it->first;
	return -1;	
}

bool	Channel::isOper(std::string &oper){
	std::vector<std::string>::iterator it = std::find(ch_operator.begin(), ch_operator.end(), oper);
	if (it != ch_operator.end())
		return true;
	return false;
}

std::map<int, Client>	Channel::getClient() const {return this->client;}
std::string	Channel::getName() const {return this->ch_name;}
std::string Channel::getTopic() const {return this->topic;}
std::time_t	const &Channel::getTPTime() const {return this->topic_time;}
int			Channel::getTPUser() const {return this->topic_user;}
std::string Channel::getKey() const {return this->key;}

std::string Channel::getMode(std::string &flag) {
	return flag;
}

bool Channel::alreadySetMode(std::string &flag){
    std::vector<std::string>::iterator it = std::find(mode.begin(), mode.end(), flag);
	if (it == mode.end())
		return false;
	else
		return true;
}
