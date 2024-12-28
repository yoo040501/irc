#include "../../includes/Server.hpp"

static int isValidChannelFlag(char c){
    if (c == '+' || c == '-')
        return 1;
    else if (c == 'o' || c == 'v' || c == 'l')
        return 2; //뒷 인자 확인
    else if (c == 'p' || c == 's' || c == 'i' || c == 't' || c == 'n' || c == 'm' || c == 'b')
        return 3; //operator 권한확인
    else
        return 0;
}

static bool isValidUserFlag(char c){
    if (c == '+' || c == '-')
        return true;
    else if (c == 'o' || c == 's' || c == 'i' || c == 'w')
        return true;
    else
        return false;
}

static void channelMode(std::map<std::string, Channel>::iterator &it, std::istringstream& iss, Client &cl){
    char        oper = '+';
    int         result;
    std::string target;
    std::string argv;

    while (getline(iss, target, ' ')){
        for (int i = 0; i < target.size(); i++){
            result = isValidChannelFlag(target[i]);
            if (result == 1)
                oper = target[i];
            else if (result == 2){
                if (!getline(iss, argv, ' ')){
                    if (target[i] == 'l')
                        sendMsg(ERR_NOPARAMETER(cl.getNick(), it->first, "limit", "<limit>"), cl.getfd());
                    else if (target[i] == 'v')
                        sendMsg(ERR_NOPARAMETER(cl.getNick(), it->first, "voice", "<nick>"), cl.getfd());
                    else if (target[i] == 'o')
                        sendMsg(ERR_NOPARAMETER(cl.getNick(), it->first, "op", "<nick>"), cl.getfd());
                }
                else{
                    // 옵션 저장
                }
            }
            else if (result == 3){
                if (!it->second.isOper(cl.getNick()))
                    sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), it->first), cl.getfd());
                else{
                    //각 옵션저장
                }
            }
            else
                sendMsg(ERR_UNKNOWNMODE(target[i]), cl.getfd());
        }
    }
}

void Server::modeCmd(std::string str, Client &cl){
    std::string         tmp;
	std::istringstream	iss;
	
    iss.str(str);
    getline(iss, tmp, ' ');
    if (tmp == cl.getNick())
        userMode(tmp, iss);
    else if (tmp[0] == '#'){
        tmp.erase(0, 1);
        std::map<std::string, Channel>::iterator it = channel.find(tmp);
        if (it == channel.end())
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), tmp), cl.getfd());
        channelMode(it, iss, cl);
    }
    else
        sendMsg(ERR_NOSUCHNICK(cl.getNick()), cl.getfd());
} 