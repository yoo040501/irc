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
			flag = false; break;
		}
	}
	if (isnumber(tmp[0])) //숫자로 시작하면 안됨
		flag = false;
	return flag;
}
void	Server::nickCheck(std::string str, Client &cl){
	if (str.size() < 1) //	//ERR_NONICKNAMEGIVEN로 irc프로토콜문서에는 적혀있는데 nc에서는 ERR_NEEDMOREPARAMS에러로 나옴
	{
		//errMsg = ERR_NEEDMOREPARAMS(cl.getNick(), "NICK");
		sendMsg(ERR_NONICKNAMEGIVEN(cl.getNick()), cl.getfd());
	}
	else if (isValidNickname(str) == false || str.size() > 29) //nickname은 최대29자, (문서에서는 9자라고 되있는데 실제로는 29개까지 받음)
		sendMsg(ERR_ERRONEUSNICKNAME(cl.getNick(), str), cl.getfd());
	else{
		if (cl.getNick() == str)
			return;
		std::map<std::string, int>::iterator it = find(nick.begin(), nick.end(), str);
		if (it != nick.end()) // 닉이 중복되는 값이 있으면 에러
			sendMsg(ERR_NICKNAMEINUSE(cl.getNick(), str), cl.getfd());
		else{
			if (cl.getNick() != "*"){
				std::map<std::string, int>::iterator it = find(nick.begin(), nick.end(), cl.getNick());
				nick.erase(it);
			}
			if (cl.getAuth() == true){
				std::string rplMsg = RPL_NICK(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), str);
				send(cl.getfd(), rplMsg.c_str(), rplMsg.length(), 0);
			}
			nick[str] = cl.getfd();
			cl.setNick(str);
		}
	}
}
