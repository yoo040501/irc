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
	return str;
}

std::string	getCMD(std::string &str){
	std::string	tmp;
	if (str[0] == ':')
		tmp = str.substr(1);
	else {
		std::istringstream iss(str);
		std::string			t;
		while (getline(iss, t, ' ')){
			if (t.empty())	continue;
			if (tmp.empty())
				tmp = t;
			else
				tmp = tmp + " " + t;
		}
	}
	return tmp;
}
