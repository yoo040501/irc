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
		if (CH.isOper(nick) == true)
			client_nick.push_back("@" + nick);
		else
			client_nick.push_back(nick);	
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

void	Server::channelCheck(std::string str, Client &cl){

	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> CH_key;
	
	getCHName(str, CH_name, cl);
	getKey(trimSpace(str), CH_key);

	for (size_t i=0; i < CH_name.size(); i++){
		if (CH_name[i].empty()) continue;
		else{
			//:b!b@127.0.0.1 JOIN :#ch1
			std::vector<std::string> client_nick;
			std::map<std::string, Channel>::iterator it = channel.find(CH_name[i]);
			if (it == channel.end()){ // 방이 처음 만들어질때 key값 필없음
				Channel CH(CH_name[i], cl);
				channel.insert(std::make_pair(CH_name[i], CH));
				client_nick.push_back("@" + cl.getNick());
				sendMsg(RPL_JOIN(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), cl.getfd());
				sendMsg(RPL_NAMREPLY(cl.getNick(), "=", CH.getName(), client_nick), cl.getfd());
				sendMsg(RPL_ENDOFNAMES(cl.getNick(), CH.getName()), cl.getfd());
			}
			else { //채널이 있을 경우 client만 저장
				// mode는 나중에
				if (it->second.getClientfd(cl.getfd()) > 0) continue;
				it->second.addClient(cl);
				getClientnick(client_nick, it->second);
				sendJoinMsg(it->second, cl);
				if (it->second.getTopic() != "")
					sendTopic(it->second, cl, client);
				sendMsg(RPL_NAMREPLY(cl.getNick(), "=", it->second.getName(), client_nick), cl.getfd());
				sendMsg(RPL_ENDOFNAMES(cl.getNick(), it->second.getName()), cl.getfd());
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
