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
		nick = it->second.getNick();
		if (CH.isOperator(nick) == true)
			client_nick.push_back("@" + it->second.getRealNick());
		else
			client_nick.push_back(it->second.getRealNick());	
		it++;
	}
}

void	sendJoinMsg(Channel &CH, Client &cl){
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end())
	{
		sendMsg(RPL_JOIN(cl.getRealNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), it->second.getfd());
		++it;
	}
}

void	joinChannel(Channel &CH, Client &cl, std::vector<std::string> &client_nick, std::map<int, Client> client){
	CH.addClient(cl);
	getClientnick(client_nick, CH);
	sendJoinMsg(CH, cl);
	if (CH.getTopic() != "")
		sendTopic(CH, cl, client);
	sendMsg(RPL_NAMREPLY(cl.getRealNick(), "=", CH.getName(), client_nick), cl.getfd());
	sendMsg(RPL_ENDOFNAMES(cl.getRealNick(), CH.getName()), cl.getfd());
}

void	Server::channelCheck(std::string str, Client &cl){

	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> CH_key;
	
	getCHName(str, CH_name, cl);
	getKey(trimSpace(str), CH_key);

	for (size_t i=0; i < CH_name.size(); i++){
		if (CH_name[i].empty()) continue;
		else{
			std::vector<std::string> 	client_nick;
			std::string					translower(CH_name[i].size(), '\0');
			std::transform(CH_name[i].begin(), CH_name[i].end(), translower.begin(), ::tolower);
			std::map<std::string, Channel>::iterator it = channel.find(translower);
			if (it == channel.end()){ // 방이 처음 만들어질때 key값 필없음
				Channel CH(CH_name[i], cl);
				channel.insert(std::make_pair(translower, CH));
				client_nick.push_back("@" + cl.getRealNick());
				sendMsg(RPL_JOIN(cl.getRealNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), cl.getfd());
				sendMsg(RPL_NAMREPLY(cl.getRealNick(), "=", CH.getName(), client_nick), cl.getfd());
				sendMsg(RPL_ENDOFNAMES(cl.getRealNick(), CH.getName()), cl.getfd());
			}
			else { //채널이 있을 경우 client만 저장
				if (it->second.getClientfd(cl.getfd()) > 0) continue; // 이미 들어가있는 경우 넘어감
				if (it->second.findMode("k")) {// channel에 key값이 없을 경우 그냥 들어감
					if (CH_key.empty() || CH_key[i].empty() || it->second.getKey() != CH_key[i])
						sendMsg(ERR_BADCHANNELKEY(cl.getRealNick(), CH_name[i]), cl.getfd());
					else
						joinChannel(it->second, cl, client_nick, client);
				}
				else if (it->second.findMode("i")){} //invite mode 활성화
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
