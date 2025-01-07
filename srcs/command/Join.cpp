#include "../../includes/Server.hpp"

void	getKey(std::string str, std::vector<std::string>&Key){
	std::istringstream	iss(str);
	std::string			tmp;

	if (str[0] == ':'){
		getline(iss, tmp, ',');
		str.erase(0, tmp.size());
		tmp.erase(0, 1);
		Key.push_back(tmp);
	}
	while (getline(iss, tmp, ',')) // key 값 저장
		Key.push_back(tmp);
}

void	getClientnick(std::vector<std::string> &client_nick, Channel &CH){
	std::map<int, Client> tmp = CH.getClient();
	std::map<int, Client>::iterator it = tmp.begin();
	std::string nick;

	while (it != tmp.end()){
		nick = it->second.getLowNick();
		if (CH.isOperator(nick) == true)
			client_nick.push_back("@" + it->second.getNick());
		else
			client_nick.push_back(it->second.getNick());	
		it++;
	}
}

void	sendJoinMsg(Channel &CH, Client &cl){
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end())
	{
		sendMsg(RPL_JOIN(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), it->second.getfd());
		++it;
	}
}

void	joinChannel(Channel &CH, Client &cl, std::vector<std::string> &client_nick, std::map<int, Client> client){
	CH.addClient(cl);
	getClientnick(client_nick, CH);
	sendJoinMsg(CH, cl);
	if (CH.getTopic() != "")
		sendTopic(CH, cl, client);
	sendMsg(RPL_NAMREPLY(cl.getNick(), "=", CH.getName(), client_nick), cl.getfd());
	sendMsg(RPL_ENDOFNAMES(cl.getNick(), CH.getName()), cl.getfd());
}

bool	alreadyFull(Channel &CH){
	long	limit_cnt = CH.getLimit();
	long	user_cnt = CH.getClient().size();
	bool	flag = false;

	if (user_cnt >= limit_cnt) // 이미 채널에 사람이 가득 차있다면 true
		flag = true;
	return flag;
}

void	Server::channelCheck(std::string str, Client &cl){

	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> CH_key;
	
	getCHName(str, CH_name, cl);
	std::vector<std::string> CH_LowName = changeLowerChannelname(CH_name);
	getKey(trimSpace(str), CH_key);

	for (size_t i=0; i < CH_name.size(); i++){
		if (CH_name[i].empty()) continue;
		else{
			std::vector<std::string>	client_nick;
			std::map<std::string, Channel>::iterator it = channel.find(CH_LowName[i]);
			if (it == channel.end()){ // 방이 처음 만들어질때 key값 필없음
				Channel CH(CH_name[i], cl);
				channel.insert(std::make_pair(CH_LowName[i], CH));
				client_nick.push_back("@" + cl.getNick());
				sendMsg(RPL_JOIN(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), cl.getfd());
				sendMsg(RPL_NAMREPLY(cl.getNick(), "=", CH.getName(), client_nick), cl.getfd());
				sendMsg(RPL_ENDOFNAMES(cl.getNick(), CH.getName()), cl.getfd());
			}
			else { //채널이 있을 경우 client만 저장
				if (it->second.getClientfd(cl.getfd()) > 0) continue; // 이미 들어가있는 경우 넘어감
				if (it->second.findMode("k")) {// channel k mode active
					if (CH_key.empty() || CH_key[i].empty() || it->second.getKey() != CH_key[i])
						sendMsg(ERR_BADCHANNELKEY(cl.getNick(), CH_name[i]), cl.getfd());
					else
						joinChannel(it->second, cl, client_nick, client);
				}
				else if (it->second.getLimit() != -1){ // channel limit active
					if (alreadyFull(it->second))
						sendMsg(ERR_CHANNELISFULL(cl.getNick(), CH_name[i]), cl.getfd());
					else
						joinChannel(it->second, cl, client_nick, client);
				}
				else if (it->second.findMode("i")){
					// sendMsg(ERR_INVITEONLYCHAN(cl.getNick(), CH_name[i]), cl.getfd());
				} //invite mode active
				else
					joinChannel(it->second, cl, client_nick, client);
			}
		}
	}
}

std::string RPL_NAMREPLY(const std::string &nickname, const std::string &type ,const std::string& channel, const std::vector<std::string> &nick) {
    std::ostringstream oss;
    oss << ":localhost 353 " << nickname << " " << type << " " << channel << " :";

	for (size_t i = 0; i < nick.size(); ++i) {
			oss << nick[i];
			if (i != nick.size() - 1) {
				oss << " ";
			}
		}
	oss << "\r\n";
    return oss.str();
}
