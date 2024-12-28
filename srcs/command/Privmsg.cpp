#include "../../includes/Server.hpp"

void	getReceiver(std::string str, std::vector<std::string> &receiver){
	std::string			tmp;
	std::string			tmp_rec;
	std::istringstream	iss(str);
	
	getline(iss, tmp, ':');
	if (tmp.find(',') != std::string::npos) // receiver가 두개 이상인 경우
	{
		while (1){}
	}
	receiver[0] = "a";
}


/* 1. str에 :가 있을 경우
		1) :로 시작하면 noreceiver 에러
		2) :전에 공백이 없으면 receiver로 처리
		3) :전전에 공백이 있으면 거기부터 공백 포함 x send message ex) a, b :hello ->  a한테만 b :hello로 메세지 보냄
	2. :가 없을 경우
		1) */
void	Server::msgCheck(std::string str, Client &cl){ // ,이랑 공백이 :보다 우선순위
	std::vector<std::string> receiver;
	size_t	pos;

	// pos = str.find_first_of(" ,"); // a, b : he
	pos = str.find(':');
	if (pos != std::string::npos){ // :<text to be sent> :있으면 공백도 포함해서 다 보냄
		if (pos == 0)	//no receiver
			sendMsg(ERR_NORECIPIENT(cl.getNick(), str), cl.getfd());
		else if (str[pos - 1] != ' '){ // 없는 nick이나 channel일 경우
					// receiver : <message>      없는 nick이나 channel일 경우, mask는 아직 잘 모르겠음
			sendMsg(ERR_NOSUCHNICK(cl.getNick(), str), cl.getfd());
		}
		// else if (asd){ // send msg가 없을 경우  :앞에 공백이 없어도 error
		// 	sendMsg(ERR_NOTEXTTOSEND(cl.getNick()), cl.getfd());
		// }
		else {

		}
	}
	else{ //: 없으면 공백으로 split

	}
}
