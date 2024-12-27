#include "../../includes/Server.hpp"

/* nick, user가 등록되있을때 pass등록하면 에러나야함 아직 미구현 or pass가 틀렸을때 나오는 문구*/
void	Server::passFail(Client &cl){
	sendMsg("ERROR :Closing link: (a@127.0.0.1) [Access denied by configuration]", cl.getfd());
	close(cl.getfd());
	createEvent(cl.getfd());
	client.erase(cl.getfd());
	nick.erase(cl.getNick());
}

void	Server::passCheck(std::string str, Client &cl){
	if (cl.getAuth() == true)
		sendMsg(ERR_ALREADYREGISTRED(cl.getNick()), cl.getfd());
	else if (str.size() < 1)
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "PASS"), cl.getfd());
	else{
		if (cl.getUser() != "" && cl.getNick() !="*") // user와 nick이 우선 등록되있을 경우
		{
			passFail(cl);
			return ;
		}
		cl.setPassCheck(true);
		if (str == password)
			cl.setPass(true);
		else
			cl.setPass(false);
	}
}
