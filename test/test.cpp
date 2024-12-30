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
	std::string str = "abcd";

	std::cout << str.substr(1) << std::endl;
	return 0;
}