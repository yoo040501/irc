#include "../../includes/Server.hpp"

void	sendTopic(Channel &CH, Client &cl, std::map<int, Client> cl_tmp){
		std::ostringstream oss;
        oss << CH.getTPTime();
        std::string tpTimeStr = oss.str();
		sendMsg(RPL_TOPIC(cl.getNick(), CH.getName(), CH.getTopic()), cl.getfd());
		sendMsg(RPL_TIMETOPIC(cl.getNick(), CH.getName(), cl_tmp[CH.getTPUser()].getNick(), cl_tmp[CH.getTPUser()].getUser(),
										inet_ntoa(cl_tmp[CH.getTPUser()].getaddr().sin_addr), tpTimeStr), cl.getfd());
}

void	sendTopicMsg(Channel &CH, Client &cl){
	std::map<int, Client> cl_tmp = CH.getClient();
	std::map<int, Client>::iterator it = cl_tmp.begin();
	while (it != cl_tmp.end())
	{
		if (it->second.getfd() == cl.getfd())
			sendMsg(RPL_SETTOPIC(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), CH.getName(), CH.getTopic()), cl.getfd());
		else
			sendTopic(CH, it->second, cl_tmp);
		++it;
	}
}

void	Server::topicCheck(std::string str, Client &cl){
	std::istringstream			iss;
	std::string					CH;
	std::vector<std::string>	topic;
	std::string					topic_str;

	if (str.empty()){
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "TOPIC"), cl.getfd()); // t
		return;
	}
	iss.str(str);
	while (getline(iss, CH, ' ')){
		if (CH.empty())	{str.erase(0, 1); continue;}
		if (CH[0] == ':')	CH = str;
		else	str.erase(0, CH.size());
		break;
	}
	if (CH == str) // no setTopic only getTopic
		CH.erase(0, 1);
	else{ // topic 주제
		while (getline(iss, topic_str, ' ')){
			if (topic_str.empty()) {str.erase(0, 1); continue;}
			if (topic_str[0] == ':'){
				topic.push_back(str.substr(1)); break; }
			else{
				str.erase(0, topic_str.size());
				topic.push_back(topic_str);
			}
		}
	}
	std::transform(CH.begin(), CH.end(), CH.begin(), ::tolower);
	if (isExistCH(CH, channel) == false) // Channel Check
		sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), CH), cl.getfd());
	else if(!inCH(channel[CH], cl.getLowNick())) // User in Channel Check
		sendMsg(ERR_NOTONCHANNEL(cl.getNick(), CH), cl.getfd());
	else
	{
		if (topic.empty()){ //getTopic
			if (channel[CH].getTopic() == "")
				sendMsg(RPL_NOTOPIC(cl.getNick(), CH), cl.getfd());
			else
				sendTopic(channel[CH], cl, client);
		}
		else{
			if (!channel[CH].isOperator(cl.getNick()) && channel[CH].findMode("t"))
				sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), CH), cl.getfd());
			else{
				topic_str = topic[0];
				for (size_t i = 1; i < topic.size(); i++)
					topic_str = topic_str + " " + topic[i];
				channel[CH].setTopic(topic_str, cl.getfd());
				sendTopicMsg(channel[CH], cl);
			}
		}
	}
}
