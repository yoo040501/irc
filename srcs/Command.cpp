#include "../includes/Server.hpp"

void	Server::checkCommand(char *buffer, Client cl){ //ctrl + D 는 아직 생각 안함
	std::string str(buffer);
	std::string	tmp;
	str.erase(str.end() - 1);
	std::istringstream iss(str);

	// std::string::size_type del = str.find(' ');
	// tmp = str.substr(0, del);
	getline(iss, tmp, ' '); // 스페이스바로 나누고 첫 문장 가져옴
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper); //tmp 대문자 변환

	if (tmp == "PASS") {
		std::cout << "This is Pass cmd\n";
    }
	else if (tmp == "NICK") {
    
    }
	else if (tmp == "USER") {
    
    }
	else if (tmp == "PRIVMSG") {
    
    }
	else if (tmp == "JOIN") {
    
    }
	else if (tmp == "KICK") {

    }
	else if (tmp == "INVITE") {

    }
	else if (tmp == "MODE") {

    }
	else {
		if (cl.getPass() != false && cl.getNick() != "" && cl.getUser() != ""){ //인증 절차가 끝난뒤에만 전송
			std::string errMsg = ERR_UNKNOWNCOMMAND(tmp, cl.getNick());
			send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
		}
    }

}
