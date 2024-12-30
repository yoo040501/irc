#include "../includes/Server.hpp"

bool	isValidCHname(std::string str){
	bool	flag = true;
	if (str.find_first_of("\r\n\a\0, ") != std::string::npos) // except SPACE, BELL, NUL, CR, LF and comma (',')
		flag = false;
	if (str[0] != '#' && str[0] != '&') //채널 앞에 #or&
		flag = false;
	return flag;
}

void	getCHName(std::istringstream &iss, std::vector<std::string> &CH_name, Client &cl){
	std::string			channel_tmp;
	while (getline(iss, channel_tmp, ',')){ //channel name 검사  name과 pass 위치가 같아야되서 빈문자열이나 에러도 일단 저장
		if (channel_tmp.empty())
			CH_name.push_back(channel_tmp);
		else if (isValidCHname(channel_tmp) == false){
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channel_tmp), cl.getfd());
			CH_name.push_back("");
		}
		else {
			CH_name.push_back(channel_tmp);
		}
	}
}

bool	isExistCH(std::string name, std::map<std::string, Channel> &channel){
	bool	flag = true;
	std::map<std::string, Channel>::iterator it = channel.find(name);
	if (it == channel.end())
		flag = false;
	return flag;
}
