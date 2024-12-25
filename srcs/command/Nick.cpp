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
	if (!isalpha(tmp[0]))
		flag = false;
	return flag;
}

void	Server::nickCheck(std::string str, Client &cl){
	std::string errMsg;
	if (str.size() < 1)
	{
		errMsg = ERR_NEEDMOREPARAMS(cl.getNick(), "NICK");
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else if (isValidNickname(str) == false || str.size() > 29){
		//nickname은 최대29자, (문서에서는 9자라고 되있는데 실제로는 29개까지 받음)
		errMsg = ERR_ERRONEUSNICKNAME(cl.getNick(), str);
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	//ERR_NONICKNAMEGIVEN 언제 나오는 에러죠 이건
	else{
		if (cl.getNick() == str)
			return;
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
	}
}
