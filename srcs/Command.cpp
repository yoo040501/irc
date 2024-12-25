#include "../includes/Server.hpp"

std::string	trimSpace(std::string str){
    std::string::iterator it = str.begin();				// 앞 공백 제거
    while (it != str.end() && (*(it) == ' ')) {
        ++it;
    }
    str.erase(str.begin(), it);

    it = str.end();										// 뒤 공백 제거
    while (it != str.begin() && (*(it) == ' ')) {
        --it;
    }
    str.erase(it, str.end());
	return str;
}

void	Server::checkCommand(char *buffer, Client &cl){ //ctrl + D 는 아직 생각 안함
	std::string str(buffer);
	std::string::iterator it = std::find(str.begin(), str.end(), '\n'); //개행문자 삭제
	if (it != str.end())
		str.erase(it, str.end());
	std::string	tmp;
	std::istringstream iss(str);

	// std::string::size_type del = str.find(' ');
	// tmp = str.substr(0, del);
	getline(iss, tmp, ' '); // 스페이스바로 나누고 첫 문장 가져옴
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper); //tmp 대문자 변환

	if (tmp == "PASS")
		passCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "NICK")
		nickCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "USER") 
		userCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "PRIVMSG") {
    
    }
	else if (tmp == "JOIN") {
    
    }
	else if (tmp == "KICK") {

    }
	else if (tmp == "INVITE") {

    }
	else {
		if (cl.getAuth() == true && str != "\r"){ //인증 절차가 끝난뒤에만 전송
			std::string errMsg = ERR_UNKNOWNCOMMAND(cl.getNick(), tmp);
			send(cl.getfd(), errMsg.c_str(), errMsg.length(), 0);
		}
    }
	// else if (tmp == "MODE") {

    // }
	if (cl.getPassCheck() == true && cl.getUser() != "" && cl.getNick() != "*") //절차를 다 했을경우
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
