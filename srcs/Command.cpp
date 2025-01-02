#include "../includes/Server.hpp"

void	Server::processAuth(std::string &str, Client &cl){
	std::string					 tmp;
	std::string::size_type del = str.find(' ');

	tmp = str.substr(0, del);
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

	if (tmp == "PASS"){
		if (del != std::string::npos)
			str.erase(0, del);
		std::string::size_type pos = str.find("NICK");
		if (!passCheck(trimSpace(str.substr(0, pos)), cl)) return;
		if (pos != std::string::npos){
			str.erase(0, pos);
			del = str.find(' ');
			tmp = str.substr(0, del);
		}
	}
	if (tmp == "NICK"){
		if (del != std::string::npos)
			str.erase(0, del);
		std::string::size_type pos = str.find("USER");
		nickCheck(trimSpace(str.substr(0, pos)), cl);
		if (pos != std::string::npos){
			str.erase(0, pos);
			del = str.find(' ');
			tmp = str.substr(0, del);
		}
	}
	if (tmp == "USER"){
		str.erase(0, tmp.size());
		std::cout << "user:" << str << std::endl;
		userCheck(trimSpace(str), cl);
	}

	if (cl.getPassCheck() == true && cl.getUser() != "" && cl.getNick() != "*") //절차를 다 했을경우
	{
		if (cl.getPass() == false){
			closeClient(ERR_CLOSE(), cl);
			return ;
		}
		else if (cl.getAuth() == false){
			sendMsg(MSG_WELCOME(servertime, cl.getNick()), cl.getfd());
			cl.setAuth(true);
		}
	}
}

void	Server::checkCommand(char *buffer, Client &cl){ //ctrl + D finsh
	std::string str(buffer);
	
	rebuffer.push(str); //일단 스택에 저장 -> 개행문자가 있다? 합침
	if (str.find_first_of("\r\n") != std::string::npos){
		str.clear();
		while (!rebuffer.empty()){
			str = rebuffer.top() + str;
			rebuffer.pop();
		}
	}
	else //없으면 리턴
		return;
	while (str.find_first_of("\r\n") != std::string::npos) //개행문자 제거 \n으로 전송시킴
		str.erase(str.find_first_of("\r\n"), 1);
	std::string	tmp;
	std::istringstream iss(str);
	if (cl.getAuth() == false) //초기에 인증 절차 진행
		processAuth(str, cl);
	else{
		getline(iss, tmp, ' '); // 스페이스바로 나누고 첫 문장 가져옴
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper); //tmp 대문자 변환
		
		if (tmp == "PASS")
			passCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "NICK")
			nickCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "USER") 
			userCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "PRIVMSG")
			msgCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "JOIN")
			channelCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "PING")
			sendMsg(MSG_PONG(trimSpace(str.substr(tmp.size()))), cl.getfd());
		else if (tmp == "KICK")
			kickCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "INVITE") {

		}
		else if (tmp == "MODE")
			modeCmd(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "TOPIC")
			topicCheck(str.substr(tmp.size()), cl);
		else {
			if (!str.empty()) //인증 절차가 끝난뒤에만 전송 10.15.3.7
				sendMsg(ERR_UNKNOWNCOMMAND(cl.getNick(), tmp), cl.getfd());
		}
	}
}
