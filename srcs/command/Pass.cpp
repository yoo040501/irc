#include "../../includes/Server.hpp"

/* nick, user가 등록되있을때 pass등록하면 에러나야함 or pass가 틀렸을때 나오는 문구*/
void	Server::closeClient(std::string msg, Client &cl){
	sendMsg(msg, cl.getfd());
	for (std::map<std::string, Channel>::iterator it = channel.begin(); it != channel.end();){ //채널에 들어가있는 경우에도 채널에서의 클라이언트 정보 삭제
		if (inCH(it->second, cl.getNick())){
			it->second.removeClient(cl.getNick());
			it->second.removeOper(cl.getNick());
		}
		if (it->second.getClient().empty())
			it = channel.erase(it);
		else	it++;
	}
	close(cl.getfd());
	createEvent(cl.getfd());
	nick.erase(cl.getNick());
	client.erase(cl.getfd());
}

bool	Server::passCheck(std::string str, Client &cl){
	std::string pass = getCMD(str);

	if (cl.getAuth() == true) // 이미 서버 접속 후
		sendMsg(ERR_ALREADYREGISTRED(cl.getNick()), cl.getfd());
	else if (pass.size() < 1) // no param
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "PASS"), cl.getfd());
	else{
		if (cl.getUser() != "" && cl.getNick() != "*") // user와 nick이 우선 등록되있을 경우
		{
			closeClient(ERR_CLOSE(cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), "Access denied by configuration"), cl);
			return false;
		}
		cl.setPassCheck(true); //pass 입력을 한번이라도 할 경우 true
		if (pass == password)
			cl.setPass(true);
		else
			cl.setPass(false);
	}
	return true;
}
