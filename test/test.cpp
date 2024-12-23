#include "includes/Server.hpp"

int main(int ac, char **av)
{
	char *buffer = av[1];
	std::string str(buffer);
	std::string	tmp;
	std::istringstream iss(str);
	std::string::size_type del = str.find(' ');

	tmp = str.substr(0, del);
	// getline(iss, tmp, ' ');
	std::cout << tmp << std::endl;
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	std::cout << tmp << std::endl;
	//getline(iss, str, ' ');
	//std::cout << tmp << std::endl;
	str.erase(0, tmp.size() + 1);
	std::cout << str << std::endl;

	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    std::cout << str << std::endl;
	return 0;
}