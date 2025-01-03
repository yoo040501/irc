#include "../../includes/Server.hpp"

void	sendPartMsg(Channel &CH, Client &cl){
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end())
	{
		sendMsg(RPL_PART(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName()), it->second.getfd());
		++it;
	}
}

void	Server::partCheck(std::string str, Client &cl){
	if (str.empty())
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "PART"), cl.getfd());
	else{
		std::vector<std::string>	CH_name;
		getCHName(str, CH_name, cl);
		CH_name = changeLowerChannelname(CH_name);

		for (std::vector<std::string>::iterator it = CH_name.begin(); it != CH_name.end();){ //ch 검사
			if (it->empty())	it = CH_name.erase(it);
			else if (!isExistCH(*it, channel)){
				sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), *it), cl.getfd());
				it = CH_name.erase(it);
			}
			else if (!inCH(channel[*it], cl.getNick())){ // client가 채널에 있는지 확인
					sendMsg(ERR_NOTONCHANNEL(cl.getNick(), *it), cl.getfd()); ++it;}
			else{
				sendPartMsg(channel[*it], cl);
				channel[*it].removeClient(cl.getNick());
				channel[*it].removeOper(cl.getNick());
				if (channel[*it].getClient().empty())
					channel.erase(*it);
				++it;
			}
		}
	}
}
