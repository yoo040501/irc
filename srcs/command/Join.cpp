#include "../../includes/Server.hpp"

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

	std::istringstream	iss;
	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> CH_key;
	iss.str(str);
	getline(iss, tmp, ' '); // 공백 기준으로 CH  key 나뉨  #ch1, #ch2 이렇게 들어오면 ch1만 들어가짐
	str.erase(0, tmp.size()); //str에서 name으로 사용했던 문자들 제거
	iss.clear();
	iss.str(tmp);
	getCHName(iss, CH_name, cl);

	str = trimSpace(str);
	iss.clear();
	iss.str(str);
	while (getline(iss, tmp, ',')) // key 값 저장
		CH_key.push_back(tmp);
	for (size_t i=0; i < CH_name.size(); i++){
		if (CH_name[i].empty()) continue;
		else{
			//:b!b@127.0.0.1 JOIN :#ch1
			std::vector<std::string> client_nick;
			std::map<std::string, Channel>::iterator it = channel.find(CH_name[i]);
			if (it == channel.end()){ // 방이 처음 만들어질때 key값 필없음
				Channel CH(CH_name[i], cl);
				std::cout << "chName: " << CH_name[i] << std::endl;
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
