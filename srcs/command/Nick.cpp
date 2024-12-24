#include "../../includes/Server.hpp"

int	isSpecial(char c) 	// - [ ] \ ` ^ { }
{
	if (c == '-' || c == '[' || c == ']' || c == '\\' || c == '`'
		|| c == '^' || c == '{' || c == '}')
		return true;
	return false;
}

bool	isPossible(std::string str){
	const char	*tmp = str.c_str();
	for (size_t i=0;i < str.length();i++)
	{
		if (!isalpha(tmp[i]) && !isnumber(tmp[i]) && !isSpecial(tmp[i]))
			return false;
	}
	return true;
}

void	Server::nickCheck(std::string str, Client &cl){
	// trimSpace(str); 	//space를 먼저 자르고 "NICK    aa       " 
	std::string errMsg;
	if (str.size() < 1)
	{
		errMsg = ERR_NEEDMOREPARAMS(cl.getNick(), "NICK");
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else if (isPossible(str) == false || str.size() > 29){
		//nickname은 최대29자, (문서에서는 9자라고 되있는데 실제로는 29개까지 받음)
		errMsg = ERR_ERRONEUSNICKNAME(cl.getNick(), str);
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	//ERR_NONICKNAMEGIVEN 언제 나오는 에러죠 이건
	else{
		std::vector<std::string>::iterator it = find(nick.begin(), nick.end(), str);
		if (it != nick.end()) // 닉이 중복되는 값이 있으면 에러
		{
			errMsg = ERR_NICKNAMEINUSE(cl.getNick(), str);
			send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
		}
		else{
			if (cl.getNick() != "*"){
				std::vector<std::string>::iterator it = find(nick.begin(), nick.end(), cl.getNick());
				nick.erase(it);
			}
			nick.push_back(str);
			cl.setNick(str);
		}

		if (cl.getPassCheck() == true && cl.getUser() != "") //절차를 다 했을경우
		{
			if (cl.getPass() == false){
				passFail(cl);
				return ;
			}
			else if (cl.getAuth() == false){
				errMsg = MSG_WELCOME("", cl.getNick());
				send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
				cl.setAuth(true);
			}
		}
	}
}
