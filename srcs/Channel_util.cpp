#include "../includes/Server.hpp"

bool	isValidCHname(std::string str){
	bool	flag = true;
	if (str.find_first_of(" \r\n\a\0,") != std::string::npos) // except SPACE, BELL, NUL, CR, LF and comma (',')
		flag = false;
	if (str[0] != '#' && str[0] != '&') //채널 앞에 #or&
		flag = false;
	return flag;
}

void	getCHName(std::string &str, std::vector<std::string> &CH_name, Client &cl){
	std::string			channel_tmp;
	std::istringstream	iss(str);

	if (str[0] == ':'){ // :#ch1 a,#ch2         :은 처음 값에만 적용 ,다음부터는 인자로 적용
		getline(iss, channel_tmp, ',');
		str.erase(0, channel_tmp.size());
		channel_tmp.erase(0, 1);
		if (!isValidCHname(channel_tmp)){
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channel_tmp), cl.getfd());
			CH_name.push_back("");
		}
		else
			CH_name.push_back(channel_tmp);
	}
	if (str[0] == ',')
		str.erase(0, 1);

	while (getline(iss, channel_tmp, ',')){ //channel name 검사  name과 pass 위치가 같아야되서 빈문자열이나 에러도 일단 저장
		if (channel_tmp.empty()){
			CH_name.push_back(channel_tmp); str.erase(0, 1);}
		else if (channel_tmp.find(' ') != std::string::npos){
			str.erase(0, channel_tmp.find(' '));
			channel_tmp = channel_tmp.substr(0, channel_tmp.find(' '));
			if (channel_tmp.empty()) break;
			if (isValidCHname(channel_tmp) == false){
				sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channel_tmp), cl.getfd());
				CH_name.push_back("");
			}
			else
				CH_name.push_back(channel_tmp);
			break;
		}
		else if (isValidCHname(channel_tmp) == false){
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channel_tmp), cl.getfd());
			CH_name.push_back("");
		}
		else
			CH_name.push_back(channel_tmp);
		str.erase(0, channel_tmp.size());
		if (str[0] == ',')
			str.erase(0, 1);
	}
}

void	getUserName(std::string &str, std::vector<std::string> &USER){
	std::istringstream	iss;
	std::string			tmp;

	if (str[0] == ':'){ //no comment
		str.erase(0, 1);
		iss.str(str);
		while (getline(iss, tmp, ',')){
			if (tmp.empty()) continue;
			USER.push_back(tmp);
		}
		str.clear();
	}
	else{
		iss.str(str);
		getline(iss, tmp, ' ');
		str.erase(0, tmp.size());
		iss.clear();
		iss.str(tmp);
		while (getline(iss, tmp, ','))
			USER.push_back(tmp);
	}

}

bool	isExistCH(std::string name, std::map<std::string, Channel> &channel){
	bool	flag = true;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	std::map<std::string, Channel>::iterator it = channel.find(name);
	if (it == channel.end())
		flag = false;
	return flag;
}

bool	isExistUSER(std::string name, std::map<std::string, int>&nick){
	bool	flag = true;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	std::map<std::string, int>::iterator it = nick.find(name);
	if (it == nick.end())
		flag = false;
	return flag;
}

bool	inCH(Channel &CH, std::string user){
	bool	flag = false;
	std::map<int, Client> tmp = CH.getClient();
	std::map<int, Client>::iterator it = tmp.begin();
	while (it != tmp.end()){
		if (it->second.getNick() == user){
			flag = true;
			break;
		}
		++it;
	}
	return flag;
}
