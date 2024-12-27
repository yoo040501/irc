#include "../../includes/Server.hpp"

bool	isValidUserInput(std::vector<std::string> &usertmp){
	bool	flag = true;

	for (size_t i = 0; i < 4; i++){
		 if (usertmp[i].find_first_of("\r\n\0") != std::string::npos) {
                flag = false;
				break;
				}
	}
	return flag;
}

void	splitString(std::string str, std::vector<std::string> &usertmp){
	std::string					tmp;
	std::istringstream 			iss(str);

	while(getline(iss, tmp, ' '))
	{
		if (tmp.size() < 1)
			continue;
		usertmp.push_back(tmp);		
	}
}

void	Server::userCheck(std::string str, Client &cl){
	std::vector<std::string>	usertmp; //username hostname servername realname

	splitString(str, usertmp);
	if (usertmp.size() < 4)
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "USER"), cl.getfd());
	else if (cl.getUser() != "")
		sendMsg(ERR_ALREADYREGISTRED(cl.getNick()), cl.getfd());
	else{
		if(isValidUserInput(usertmp) == false)//user값에 \n \r \0이 들어가면안됨 , 에러 문구 확인 안함
			sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "USER"), cl.getfd());
		else
			cl.setUser(usertmp[0], usertmp[1], usertmp[2], usertmp[3]);
	}
}
