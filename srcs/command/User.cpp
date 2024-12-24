#include "../../includes/Server.hpp"

void	Server::userCheck(std::string str, Client &cl){
	std::string 				errMsg;
	std::string					tmp;
	std::vector<std::string>	usertmp; //username hostname servername realname
	std::istringstream 			iss(str);

	while(getline(iss, tmp, ' '))
	{
		if (tmp.size() < 1)
			continue;
		usertmp.push_back(tmp);		
	}
	if (usertmp.size() < 4){
		errMsg = ERR_NEEDMOREPARAMS(cl.getNick(), "USER");
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else if (cl.getUser() != ""){
		errMsg = ERR_ALREADYREGISTRED(cl.getNick());
		send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
	}
	else{
		cl.setUser(usertmp[0], usertmp[1], usertmp[2], usertmp[3]); //user값에 \n \r \0이 들어가면안됨 아직 미구현
		if (cl.getPassCheck() == true && cl.getNick() != "*") //절차를 다 했을경우
		{
			if (cl.getPass() == false){
				passFail(cl);
				return ;
			}
			else if (cl.getAuth() == false) {
				errMsg = MSG_WELCOME("", cl.getNick());
				send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
				cl.setAuth(true);
			}
		}
	}
}
