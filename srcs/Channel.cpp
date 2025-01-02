#include "../includes/Channel.hpp"

Channel::Channel() : ch_name(""), topic(""), key(""), limit(-1){
	mode.push_back("t");
}

Channel::Channel(std::string name, Client &cl) : ch_name(name), topic(""), key(""){
	client[cl.getfd()] = cl;
	ch_operator.push_back(cl.getNick());
	mode.push_back("t");
	limit = -1;
}

Channel::Channel(Channel const& copy): client(copy.client), ch_name(copy.ch_name),
										topic(copy.topic), key(copy.key), ch_operator(copy.ch_operator),
										mode(copy.mode){}

Channel::~Channel() {
	client.clear();
	mode.clear();
}

void	Channel::addClient(Client &cl){ client[cl.getfd()] = cl; }

void	Channel::addOper(std::string &oper){
	ch_operator.push_back(oper); 
}

void	Channel::addVoiceUser(std::string &user){
	voiceUser.push_back(user);
}

void	Channel::setName(std::string &n){ ch_name = n;}
void	Channel::setTopic(std::string &top, int fd){ topic = top; topic_time = time(NULL); topic_user = fd;}
void	Channel::setKey(std::string &k){ key = k; }

void	Channel::setLimit(long value){
	limit = value;
}

bool	Channel::findMode(std::string flag){
    std::vector<std::string>::iterator it = std::find(mode.begin(), mode.end(), flag);
	if (it != mode.end())
		return true;
	else
		return false;
}

void	Channel::setMode(std::string &flag){
	if (findMode(flag) == false)
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

bool	Channel::removeVoiceUser(std::string &user){
	std::vector<std::string>::iterator it = std::find(voiceUser.begin(), voiceUser.end(), user);
	if (it != voiceUser.end()){
		voiceUser.erase(it);
		return true;
	}
	else
		return false;
}

void	Channel::removeClient(std::string user){
	std::map<int, Client>::iterator it = client.begin();
	while (it != client.end()){
		if (it->second.getNick() == user){
			client.erase(it);
			break;
		}
		++it;
	}
}

bool	Channel::isChannelUser(std::string &user){
    std::map<int, Client>::iterator it;
	for (it = client.begin(); it != client.end(); ++it){
		if (it->second.getNick() == user)
			return true;
	}
	return false;
}

bool	Channel::isVoiceUser(std::string &user){
	std::vector<std::string>::iterator it = find(voiceUser.begin(), voiceUser.end(), user);
	if (it != voiceUser.end())
		return true;
	else
		return false;
}

bool	Channel::isOperatorator(std::string &user){
	std::vector<std::string>::iterator it = find(ch_operator.begin(), ch_operator.end(), user);
	if (it != ch_operator.end())
		return true;
	else
		return false;
}


int		Channel::getClientfd(int const fd) {
	std::map<int, Client>::iterator it = client.find(fd);
	if (it != client.end())
		return it->first;
	return -1;	
}


std::map<int, Client>	Channel::getClient() const {return this->client;}

std::string	Channel::getName() const {
	return this->ch_name;
}

std::string Channel::getTopic() const {return this->topic;}
std::time_t	const &Channel::getTPTime() const {return this->topic_time;}
int			Channel::getTPUser() const {return this->topic_user;}

std::string Channel::getKey() const {
	return this->key;
}

std::string Channel::getMode(std::string &flag) {
	return flag;
}

long	Channel::getLimit(){
	return limit;
}
