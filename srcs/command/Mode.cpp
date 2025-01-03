#include "../../includes/Server.hpp"

static int isValidChannelFlag(char c){
    if (c == '+' || c == '-')
        return 1;
    else if (c == 'o' || c == 'l' || c == 'k')
        return 2; //뒷 인자 확인
    else if (c == 'i' || c == 't')
        return 3; //operator 권한확인 
    else
        return 0;
}

static void changeMode(Channel &ch, std::string &successFlag, char op, char c){
    std::string flag;
    bool        isSet;
    size_t      pos;
    
    flag += c;
    isSet = ch.findMode(flag);
    pos = successFlag.find_last_of("+-");
    if (op == '+' && (!isSet || c == 'l')){
        ch.setMode(flag);
        if (pos == std::string::npos || successFlag[pos] != op)
            successFlag += ('+' + flag);
        else
            successFlag += flag;
    }
    else if (op == '-' && isSet){ 
        ch.removeMode(flag);
        if (successFlag[pos] != op)
            successFlag += ('-' + flag);
        else
            successFlag += flag;
    }
}

static bool checkColon(std::istringstream& iss, std::string &argv){
    bool flag = false;
    std::string     tmp;

    if (argv[0] == ':'){
        while (getline(iss, tmp, ' '))      
            flag = true;
        if (flag)
            return false;
        argv.erase(0, 1);
        if (argv.empty() || argv[0] == ':')
            return false;
    }
    return true;
}

static void keyFlag(Channel &ch, Client &cl, std::istringstream& iss, std::pair<std::string, std::string>& success, char op){
    std::string argv;

    getline(iss, argv, ' ');
    if (argv.empty() || argv == ":")
        sendMsg(ERR_NOPARAMETER(cl.getNick(), ch.getName(), "key", "<key>"), cl.getfd());
    else if (!checkColon(iss, argv))
        return ;
    else if (!ch.isOperator(cl.getNick()))
        sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), ch.getName()), cl.getfd());
    else{
        if (op == '+' && ch.getKey().empty()){
            changeMode(ch, success.first, op, 'k');
            ch.setKey(argv);
            success.second += (' ' + argv);
        }
        else{
            std::string flag = "k";
            if (ch.findMode(flag) && argv == ch.getKey()){
                changeMode(ch, success.first, op, 'k');
                success.second += (' ' + ch.getKey());
                argv = "";
                ch.setKey(argv);
            }
        }
    }
}

static void limitFlag(Channel &ch, Client &cl, std::istringstream& iss,  std::pair<std::string, std::string>& success, char op){
    std::string argv;
    char        *end;
    long        value;

    if (op == '+'){
        getline(iss, argv, ' ');
        if (argv.empty())
            sendMsg(ERR_NOPARAMETER(cl.getNick(), ch.getName(), "limit", "<limit>"), cl.getfd());
        else if (!checkColon(iss, argv))
            return ;
        else{
            value = strtol(argv.c_str(), &end, 10);
            if (ch.getLimit() != value){
                ch.setLimit(value);
                changeMode(ch, success.first, op, 'l');
                std::stringstream oss;
                oss << value;
                success.second += (' ' + oss.str());
            }
        }
    }
    else{
        std::string flag = "l";     
        if (ch.findMode(flag)){
            changeMode(ch, success.first, op, 'l');
            ch.setLimit(-1);
        }
    }
}

void Server::operateFlag(Channel &ch, Client &cl, std::istringstream& iss, std::pair<std::string, std::string>& success, char op){
    std::string argv;
    bool        oper;
    
    getline(iss, argv, ' ');
    if (argv.empty() || argv == ":")
        sendMsg(ERR_NOPARAMETER(cl.getNick(), ch.getName(), "op", "<nick>"), cl.getfd());
    else if (!checkColon(iss, argv))
        return ;
    else if (!isServerUser(argv))
        sendMsg(ERR_NOSUCHNICK(cl.getNick(), argv), cl.getfd());
    else{
        if (ch.isChannelUser(argv)){
            oper = ch.isOperator(argv);
            if (op == '+' && !oper){
                changeMode(ch, success.first, op, 'o');
                ch.addOper(argv);
                success.second += (' ' + argv);
            }
            else if (op == '-' && oper){
                changeMode(ch, success.first, op, 'o');
                ch.removeOper(argv);
                success.second += (' ' + argv);
            }
        }
    }
}

void    sendToChannelClient(Channel &ch, Client &cl, std::string& successFlag){
	std::map<int, Client>	tmp = ch.getClient();
    std::map<int, Client>::iterator it = tmp.begin();
    while (it != tmp.end()){
        sendMsg(RPL_MODE(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), ch.getName(), successFlag), it->second.getfd());
        ++it;
    }
}

void Server::channelMode(std::map<std::string, Channel>::iterator &it, std::istringstream& iss, Client &cl){
    char        oper = '+';
    int         result = -1;
    std::string target;

    while (getline(iss, target, ' ')){
        std::string successFlag;
        std::pair<std::string, std::string> success;
        for (size_t i = 0; i < target.size(); i++){
            result = isValidChannelFlag(target[i]);
            if (result == 1)
                oper = target[i];
            else if (result == 2){
                if (target[i] == 'l')
                    limitFlag(it->second, cl, iss, success, oper);
                else if (target[i] == 'k')
                    keyFlag(it->second, cl, iss, success, oper);
                else if (target[i] == 'o')
                    operateFlag(it->second, cl, iss, success, oper);
            }
            else if (result == 3){
                if (!it->second.isOperator(cl.getNick()))
                    sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), it->first), cl.getfd()); 
                else
                    changeMode(it->second, success.first, oper, target[i]); 
            }
            else
                sendMsg(ERR_UNKNOWNMODE(cl.getNick(), std::string(1, target[i])), cl.getfd()); 
        }
        if (!success.first.empty() || !success.second.empty()){
            std::string ret = success.first + success.second;
            size_t lastSpace = ret.find_last_of(' ');
            if (lastSpace != std::string::npos)
                ret.insert(lastSpace + 1, ":");
            else
                ret.insert(0, ":");
            sendToChannelClient(it->second, cl, ret);
        }
    }
    if (result == -1){
        std::ostringstream oss;
        oss << it->second.getChTime();
        std::string chTimeStr = oss.str();
        sendMsg(RPL_CHANNELMODEIS(cl.getNick(), it->second.getName(), it->second.printMode()), cl.getfd());
        sendMsg(RPL_CHANNELTIME(cl.getNick(), it->second.getName(), chTimeStr), cl.getfd());
    }
    it->second.printMode();
}

void Server::modeCmd(std::string str, Client &cl){
    std::string         tmp;
	std::istringstream	iss;

	if (str.empty()){
		sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "MODE"), cl.getfd());
		return ;
	}
    iss.str(str);
    getline(iss, tmp, ' ');
    if (tmp[0] == '#'){
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        std::map<std::string, Channel>::iterator it = channel.find(tmp);
        if (it == channel.end())
			sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), tmp), cl.getfd());
        else
            channelMode(it, iss, cl);
    }
    else
        sendMsg(ERR_NOSUCHNICK(cl.getNick(), str), cl.getfd());
} 
