#include "../../includes/Server.hpp"

int	isSpecial(char c) 	// - [ ] \ ` ^ { }
{
	if (c == '-' || c == '[' || c == ']' || c == '\\' || c == '`'
		|| c == '^' || c == '{' || c == '}')
		return true;
	return false;
}

bool	isValidNickname(std::string str){
	const char	*tmp = str.c_str();
	bool		flag = true;
	for (size_t i=0;i < str.length();i++)
	{
		if (!isalpha(tmp[i]) && !isnumber(tmp[i]) && !isSpecial(tmp[i])){
			flag = false; break; }
	}
	if (isnumber(tmp[0])) //숫자로 시작하면 안됨
		flag = false;
	return flag;
}

void	channelSet(std::map<std::string, Channel> &channel, Client &cl){
	std::map<std::string, Channel>::iterator it = channel.begin(); // channel 내부에 있는 client 값도 변경
	while (it != channel.end()){
		if (it->second.isOperator(cl.getOldnick()) == true){
			it->second.removeOper(cl.getOldnick());
			it->second.addOper(cl.getNick());
			it->second.setClient(cl.getfd(), cl);
		}
		it++;
	}
}

void	Server::nickCheck(std::string str, Client &cl){
	std::string	nick_tmp = getCMD(str);
	if (nick_tmp.empty()) 												//ERR_NONICKNAMEGIVEN로 irc프로토콜문서에는 적혀있는데 inspircd에서는 ERR_NEEDMOREPARAMS에러로 나옴
		sendMsg(ERR_NONICKNAMEGIVEN(cl.getNick()), cl.getfd());
	else if (isValidNickname(nick_tmp) == false || nick_tmp.size() > 29) //nickname은 최대29자, (문서에서는 9자라고 되있는데 inspircd는 29개까지 받음)
		sendMsg(ERR_ERRONEUSNICKNAME(cl.getNick(), nick_tmp), cl.getfd());
	else{
		std::string	nicklower(nick_tmp.size(), '\0');
		std::transform(nick_tmp.begin(), nick_tmp.end(), nicklower.begin(), ::tolower);

		if (cl.getNick() == nick_tmp)									// 기존 nick이면 지나감
			return;
		std::map<std::string, int>::iterator it = nick.find(nicklower);
		if (it != nick.end()) 											// 닉이 중복되는 값이 있으면 에러
			sendMsg(ERR_NICKNAMEINUSE(cl.getNick(), nick_tmp), cl.getfd());
		else{
			if (cl.getNick() != "*"){ 									// 닉이 이미 정해져있을 경우 기존 닉네임 기록 삭제
				std::map<std::string, int>::iterator it = nick.find(cl.getNick());
				nick.erase(it);
			}
			if (cl.getAuth() == true){ 									// 서버 접속후 닉변경 알림
				sendMsg(RPL_NICK(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), nick_tmp), cl.getfd());
			}
			nick[nicklower] = cl.getfd();
			cl.setNick(nick_tmp);
			channelSet(channel, cl);
		}
	}
}
