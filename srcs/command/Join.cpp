#include "../../includes/Server.hpp"

bool	isValidCHname(std::string str){
	bool	flag = true;
	if (str.find_first_of("\r\n\a\0, ") != std::string::npos) // except SPACE, BELL, NUL, CR, LF and comma (',')
		flag = false;
	if (str[0] != '#' || str[0] != '&') //채널 앞에 #or&
		flag = false;
	return flag;
}

void	Server::channelCheck(std::string str, Client &cl){

	std::istringstream	iss(str);
	std::string			tmp;
	std::string			channel_tmp;
	std::vector<std::string> CH_name;
	std::vector<std::string> CH_key;

	getline(iss, tmp, ' '); // 공백 기준으로 CH  key 나뉨  #ch1, #ch2 이렇게 들어오면 ch1만 들어가짐
	iss.str(tmp);
	while (getline(iss, channel_tmp, ',')){ //channel name 검사  name과 pass 위치가 같아야되서 빈문자열이나 에러도 일단 저장
		if (channel_tmp.empty())
			CH_name.push_back(channel_tmp);
		else if (isValidCHname(channel_tmp) == false){
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), channel_tmp), cl.getfd());
			CH_name.push_back(channel_tmp);
		}
		else {
			channel_tmp.erase(0); // #, & 제거하고 임시 저장
			CH_name.push_back(channel_tmp);
		}
	}
	str.erase(0, tmp.size()); //str에서 name으로 사용했던 문자들 제거
	trimSpace(str);
	iss.str(str);
	while (getline(iss, tmp, ',')) // key 값 저장
		CH_key.push_back(tmp);
	
	for (size_t i=0; i < CH_name.size(); i++){
		if (CH_name[i].empty()) continue;
		else if (channel.find(CH_name[i]) == channel.end()){ // 방이 처음 만들어질때 key값 필없음
			Channel CH(CH_name[i], cl);
			channel.insert(std::make_pair(CH_name[i], CH));
		}
	}
}