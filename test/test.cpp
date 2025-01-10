#include "../includes/Server.hpp"

int main(int ac, char **av)
{
	// char *buffer = av[1];
	// std::string str(buffer);
	// std::string	tmp;
	// //str.erase(str.end()); //개행삭제
	// std::istringstream iss(str);
	// std::string::size_type del = str.find(' ');

	// //tmp = str.substr(0, del);
	// getline(iss, tmp, ' ');
	// std::cout << tmp << std::endl;
	// std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	// std::cout << tmp << std::endl;
	// // getline(iss, tmp, ' ');
	// // std::cout << tmp << std::endl;
	// //std::cout << str.substr(tmp.size() + 1) << std::endl;
	// trimSpace(str.substr(3));
	// if (str.size() > 9)
	// 	str = str.substr(0,9);
	std::string arg = "he :hel    l    o";
	std::vector<std::string> args;
	std::istringstream argStream(arg);
	std::string token;
	bool foundColon = false;

	while (std::getline(argStream, token, ' ')) {
		if (!foundColon && token[0] == ':') {
			foundColon = true;
			std::string remaining;
			while (std::getline(argStream, remaining, ' ')) {
				token += " " + remaining;
			}
			token.erase(0, 1);
			args.push_back(token);
			break;
		} else {
			args.push_back(token);
		}
	}
	for (size_t i=0;i<args.size();i++){
		std::cout << args[i] << std::endl;
	}
	return 0;
}