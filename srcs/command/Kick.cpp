#include "../../includes/Server.hpp"

/*
 The KICK command can be  used  to  forcibly  remove  a  user  from  a
   channel.   It  'kicks  them  out'  of the channel (forced PART).

   Only a channel operator may kick another user out of a  channel.
   Each  server that  receives  a KICK message checks that it is valid
   (ie the sender is actually a  channel  operator)  before  removing
   the  victim  from  the channel.

   Numeric Replies:
           ERR_NEEDMOREPARAMS // 인자값 부족              ERR_NOSUCHCHANNEL //찾을수 없는 ch
        	ERR_CHANOPRIVSNEEDED // 권한 x			ERR_NOTONCHANNEL // channel에 안들어가있을떄
			ERR_USERNOTINCHANNEL // kick하려는 유저가 채널에 없을때
   Examples:
KICK &Melbourne Matthew         ; Kick Matthew from &Melbourne

KICK #Finnish John :Speaking English
                                ; Kick John from #Finnish using
                                "Speaking English" as the reason
                                (comment).
:WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                from channel #Finnish
NOTE:
     It is possible to extend the KICK command parameters to the
following:
<channel>{,<channel>} <user>{,<user>} [<comment>]
상용 서버에서는  channel값을 한개만 받음*/

bool	inCH(Channel &CH, std::string user){
	bool	flag = false;
	std::map<int, Client> tmp = CH.getClient();
	std::map<int, Client>::iterator it = tmp.begin();
	while (it != tmp.end()){
		if (it->second.getNick() == user){
			flag = true;
			break;
		}
		++it;
	}
	return flag;
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
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end()){
		if (it->second.getNick() == user){
			break;
		}
		++it;
	}
	if (str.c_str()[0] == ':') // 공백 포함 전송
		sendKickMsg(CH, it->second, str);
	else{
		if (str.find(':') != std::string::npos) // 처음 나오는 : 삭제
			str.erase(str.find(':'));
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

bool	isExistUSER(std::string name, std::map<std::string, int>&nick){
	bool	flag = true;
	std::map<std::string, int>::iterator it = nick.find(name);
	if (it == nick.end())
		flag = false;
	return flag;
}

bool	isExistCH(std::string name, std::map<std::string, Channel> &channel){
	bool	flag = true;
	std::map<std::string, Channel>::iterator it = channel.find(name);
	if (it == channel.end())
		flag = false;
	return flag;
}

void	Server::kickCheck(std::string str, Client &cl){
	std::istringstream	iss;
	std::string			tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> USER;

	iss.str(str);
	getline(iss, tmp, ' ');
	str.erase(0, tmp.size());
	iss.clear();
	iss.str(tmp);
	getCHName(iss, CH_name, cl);

	str = trimSpace(str);
	iss.clear();
	iss.str(str);
	getline(iss, tmp, ' ');
	str.erase(0, tmp.size());
	iss.clear();
	iss.str(tmp);
	while (getline(iss, tmp, ','))
		USER.push_back(tmp);
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
			else if (!channel[*it].isOper(cl.getNick())) //ERR_CHANOPRIVSNEEDED 채널에 들어가있는데 권한이 있는지 확인
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
