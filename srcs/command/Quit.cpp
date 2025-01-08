#include "../../includes/Server.hpp"

void	Server::quitCheck(std::string str, Client &cl){
	std::vector<std::string>	msg;
	std::string					user = cl.getUser();

	if (user.empty())
		user = "user";
	if (str.empty())
		closeClient(ERR_CLOSE(user, inet_ntoa(cl.getaddr().sin_addr), "Client exited"), cl);
	else{
		str = getCMD(str);
		closeClient(ERR_CLOSE(user, inet_ntoa(cl.getaddr().sin_addr), str), cl);}
}
