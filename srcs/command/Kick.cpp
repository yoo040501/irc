#include "../../includes/Server.hpp"

/*
following:
<channel>{,<channel>} <user>{,<user>} [<comment>]
상용 서버에서는  channel값을 한개만 받음*/

int	checkParam(std::string str){
	std::istringstream	iss(str);
	std::string			tmp;
	int					cnt = 0;

	if (str[0] == ':')
		return 0;
	while (getline(iss, tmp, ' ')){
		if (tmp.empty())	continue;
		cnt++;
	}
	return cnt;
}

void	sendKickMsg(Channel &CH, Client &cl, std::string msg){
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end())
	{
		sendMsg(RPL_KICK(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName(), cl.getUser(), msg), it->second.getfd());
		++it;
	}
}

void	kickUser(Channel &CH, std::string user, std::string str){
	std::transform(user.begin(), user.end(), user.begin(), ::tolower);
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end()){
		if (it->second.getLowNick() == user){
			break;
		}
		++it;
	}
	if (str.c_str()[0] == ':') {// 공백 포함 전송
		str.erase(0, 1);
		sendKickMsg(CH, it->second, str);
	}
	else{
		if (str.find(':') != std::string::npos) // 처음 나오는 : 삭제
			str.erase(str.find(':'), 1);
		std::vector<std::string> cmd;
		std::istringstream	iss(str);
		std::string			tmp;
		while (getline(iss, tmp, ' ')){
			if (tmp.empty()) continue;
			cmd.push_back(tmp);
		}
		if (cmd.empty())
			sendKickMsg(CH, it->second, it->second.getNick());
		else {
			std::string			msg = cmd[0];
			for (size_t i=1; i<cmd.size();i++){
				msg.append(" " + cmd[i]);
			}
			sendKickMsg(CH, it->second, msg);
		}
	}
	CH.removeClient(user);
	CH.removeOper(user);
}

void	Server::kickCheck(std::string str, Client &cl){
	std::istringstream	iss;
	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> USER;

	if (checkParam(str) < 2){
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "KICK"), cl.getfd());
		return ;
	}
	getCHName(str, CH_name, cl);
	CH_name = changeLowerChannelname(CH_name);
	str = trimSpace(str);
	getUserName(str, USER);
	str = trimSpace(str);
	for (std::vector<std::string>::iterator it = USER.begin(); it != USER.end();) { // nick검사
		if (it->empty())	it = USER.erase(it);
		else if (!isExistUSER(*it, nick)) {
			sendMsg(ERR_NOSUCHNICK(cl.getNick(), *it), cl.getfd());
			it = USER.erase(it); // 요소 제거 후 이터레이터 업데이트
		}
		else	++it; // 다음 요소로 이동
	}

	for (std::vector<std::string>::iterator it = CH_name.begin(); it != CH_name.end();){ //ch 검사
		if (it->empty())	it = CH_name.erase(it);
		else if (!isExistCH(*it, channel)){
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), *it), cl.getfd());
			it = CH_name.erase(it);
		}
		else{
			if (!inCH(channel[*it], cl.getNick())) // client가 채널에 있는지 확인
				sendMsg(ERR_NOTONCHANNEL(cl.getNick(), *it), cl.getfd());
			else if (!channel[*it].isOperator(cl.getNick())) //ERR_CHANOPRIVSNEEDED 채널에 들어가있는데 권한이 있는지 확인
				sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), *it), cl.getfd());
			else {
				for (std::vector<std::string>::iterator i = USER.begin(); i != USER.end();++i){
					if (!inCH(channel[*it], *i))//ERR_NOTONCHANNE  user가 채널에 들어가있는지 확인
						sendMsg(ERR_USERNOTINCHANNEL(cl.getNick(), *i, *it), cl.getfd());
					else
						kickUser(channel[*it], *i, str);
				}
			}
			if (channel[*it].getClient().empty())
				channel.erase(*it);
			++it;
		}
	}
}
