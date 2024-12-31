#include "../../includes/Server.hpp"

bool	isValidUserInput(std::vector<std::string> &usertmp){
	bool	flag = true;

	for (size_t i = 0; i < 4; i++){
		if (usertmp[i].find_first_of("\r\n\0") != std::string::npos) {
			flag = false;
			break;
		}
		for (size_t j = 0; j < usertmp[i].length(); j++){
			if (!isalpha(usertmp[i][j]) && !isnumber(usertmp[i][j]) && !isSpecial(usertmp[i][j]) && usertmp[i][j] != ' ' && usertmp[i][j] != '.'){
				flag = false;
				break;
			}
		}
	}
	return flag;
}

void	splitName(std::string str, std::vector<std::string> &usertmp){
	std::string					tmp;
	std::istringstream 			iss(str);
	bool						colon = false;

	while(getline(iss, tmp, ' ')) //ex) a :b dasd eqw
	{
		if (tmp.empty()){str.erase(0, 1); continue;} // 공백이면 지움
		str.erase(0, tmp.size());					 // delete a
		if (tmp[0] == ':'){colon = true; break;}
		str = trimSpace(str);
		usertmp.push_back(tmp);		
	}
	if (colon == true){
		tmp.erase(0, 1);
		usertmp.push_back(tmp + str);
	}
}

void	Server::userCheck(std::string str, Client &cl){
	std::vector<std::string>	usertmp; //username hostname servername realname

	splitName(str, usertmp); //: 뒤에는 무조건 realname
	if (usertmp.size() < 4)
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "USER"), cl.getfd());
	else if (cl.getUser() != "")
		sendMsg(ERR_ALREADYREGISTRED(cl.getNick()), cl.getfd());
	else{
		if(isValidUserInput(usertmp) == false)
			sendMsg(ERR_ERRONEUSUSER(cl.getNick()), cl.getfd());
		else
			cl.setUser(usertmp[0], usertmp[1], usertmp[2], usertmp[3]);
	}
}
