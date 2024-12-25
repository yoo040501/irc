#include "../../includes/Server.hpp"

bool	isValidUserInput(std::vector<std::string> &usertmp){
	bool	flag = true;

	for (size_t i = 0; i < 4; i++){
		 if (usertmp[i].find('\r') != std::string::npos || 
                usertmp[i].find('\n') != std::string::npos || 
                usertmp[i].find('\0') != std::string::npos) {
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
	std::string 				errMsg;
	std::vector<std::string>	usertmp; //username hostname servername realname

	splitString(str, usertmp);

	if (usertmp.size() < 4){
		errMsg = ERR_NEEDMOREPARAMS(cl.getNick(), "USER");
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else if (cl.getAuth() == true){
		errMsg = ERR_ALREADYREGISTRED(cl.getNick());
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else{
		if(isValidUserInput(usertmp) == false){  //user값에 \n \r \0이 들어가면안됨 , 에러 문구 확인 안함

		}
		cl.setUser(usertmp[0], usertmp[1], usertmp[2], usertmp[3]);
	}
}
