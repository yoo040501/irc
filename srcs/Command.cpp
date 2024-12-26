#include "../includes/Server.hpp"

void	sendMsg(std::string msg, int fd){
	send(fd, msg.c_str(), msg.length(), 0);
}

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
	size_t pos = str.find("\r\n");
	if (pos != std::string::npos)
		str.erase(str.find("\r\n"), 2); //개행문자 삭제
	std::string	tmp;
	std::istringstream iss(str);

	// std::string::size_type del = str.find(' ');
	// tmp = str.substr(0, del);
	getline(iss, tmp, ' '); // 스페이스바로 나누고 첫 문장 가져옴
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper); //tmp 대문자 변환
	
	if (tmp == "CAP")
		sendMsg(RPL_CAPLS(cl.getNick()), cl.getfd());
	else if (tmp == "PASS")
		passCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "NICK")
		nickCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "USER") 
		userCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "PRIVMSG" && cl.getAuth() == true)
		msgCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "JOIN" && cl.getAuth() == true)
		channelCheck(trimSpace(str.substr(tmp.size())), cl);
	else if (tmp == "KICK" && cl.getAuth() == true) {

    }
	else if (tmp == "INVITE" && cl.getAuth() == true) {

    }
	else {
		if (cl.getAuth() == true && !str.empty()) //인증 절차가 끝난뒤에만 전송 10.15.3.7
			sendMsg(ERR_UNKNOWNCOMMAND(cl.getNick(), tmp), cl.getfd());
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
			sendMsg(MSG_WELCOME("", cl.getNick()), cl.getfd());
			cl.setAuth(true);
		}
	}
}
