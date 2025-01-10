#include "../includes/Channel.hpp"

Channel::Channel() : ch_name(""), low_name(""), topic_time(0), topic(""), topic_user(-1), key(""), limit(-1){
	mode.push_back("t");
	mode.push_back("o");
	channel_time = time(NULL);
}

Channel::Channel(std::string name, Client &cl) : ch_name(name), low_name(""), topic_time(0), topic(""), topic_user(-1), key(""), limit(-1){
	channel_time = time(NULL);
	client[cl.getfd()] = cl;
	ch_operator.push_back(cl.getLowNick());
	mode.push_back("t");
	mode.push_back("o");
}

Channel::Channel(Channel const& copy): client(copy.client), ch_name(copy.ch_name), low_name(copy.low_name), channel_time(copy.channel_time),
										topic_time(copy.topic_time), topic(copy.topic), topic_user(copy.topic_user),
										key(copy.key), ch_operator(copy.ch_operator), mode(copy.mode), limit(copy.limit)
										{}

Channel::~Channel() {
	client.clear();
	mode.clear();
}

void	Channel::addClient(Client &cl){ client[cl.getfd()] = cl; }

void	Channel::addOper(std::string &oper){
	std::transform(oper.begin(), oper.end(), oper.begin(), ::tolower);
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

std::string	Channel::printMode(){
	std::cout << "channel mode" << std::endl;
	std::string params;
	for(std::vector<std::string>::iterator it = mode.begin(); it < mode.end(); it++){
		params += *it;
		std::cout << *it << " ";
	}
	std::cout << std::endl;
	return params;
}

void	Channel::setClient(int fd, Client &cl){
	client[fd] = cl;
}

void	Channel::removeOper(std::string &oper) {
	std::transform(oper.begin(), oper.end(), oper.begin(), ::tolower);
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
	std::transform(user.begin(), user.end(), user.begin(), ::tolower);
	while (it != client.end()){
		if (it->second.getLowNick() == user){
			client.erase(it);
			break;
		}
		++it;
	}
}

bool	Channel::isChannelUser(std::string &user){
	std::transform(user.begin(), user.end(), user.begin(), ::tolower);
    std::map<int, Client>::iterator it;
	for (it = client.begin(); it != client.end(); ++it){
		if (it->second.getLowNick() == user)
			return true;
	}
	return false;
}

bool	Channel::isOperator(std::string &user){
	std::transform(user.begin(), user.end(), user.begin(), ::tolower);
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

std::time_t	const &Channel::getChTime() const{
	return this->channel_time;
}

void Channel::inviteClient(int fd) {
    // 초대된 클라이언트를 별도로 관리하거나 추가 작업 수행 가능
    if (client.find(fd) == client.end()) {
        client[fd] = Client(); // 임시 객체 추가
    }
}

// invite
bool Channel::isClientInvited(Client &client) {
	return std::find(invitedClients_.begin(), invitedClients_.end(), &client) != invitedClients_.end();
}

void Channel::inviteClient(Client &client) {
	if (std::find(invitedClients_.begin(), invitedClients_.end(), &client) == invitedClients_.end()) {
		invitedClients_.push_back(&client);
		client.addInviteChannel(this->getName());
	}
}

void		Channel::removeinviteClient(Client &client){
	std::vector<Client *>::iterator it = std::find(invitedClients_.begin(), invitedClients_.end(), &client);
	if (it != invitedClients_.end())
		invitedClients_.erase(it);
}
