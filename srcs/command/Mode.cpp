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

static void keyFlag(Channel &ch, Client &cl, std::istringstream& iss, std::string &successFlag, char op){
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
            changeMode(ch, successFlag, op, 'k');
            ch.setKey(argv);
            successFlag += (' ' + argv);
        }
        else{
            std::string flag = "k";
            if (ch.findMode(flag) && argv == ch.getKey()){
                changeMode(ch, successFlag, op, 'k');
                successFlag += (' ' + ch.getKey());
                argv = "";
                ch.setKey(argv);
            }
        }
    }
}

static void limitFlag(Channel &ch, Client &cl, std::istringstream& iss, std::string &successFlag, char op){
    std::string argv;
    char        *end;
    long        value;

    if (op == '+'){
        if (!getline(iss, argv, ' '))
            sendMsg(ERR_NOPARAMETER(cl.getNick(), ch.getName(), "limit", "<limit>"), cl.getfd());
        else{
            value = strtol(argv.c_str(), &end, 10);
            if (ch.getLimit() != value){
                ch.setLimit(value);
                changeMode(ch, successFlag, op, 'l');
                successFlag += (' ' + argv);
            }
        }
    }
    else{
        std::string flag = "l";     
        if (ch.findMode(flag)){
            changeMode(ch, successFlag, op, 'l');
            ch.setLimit(-1);
        }
    }
}

//void Server::voiceFlag(Channel &ch, Client &cl, std::istringstream& iss, std::string &successFlag, char op){
//    std::string argv;
//    bool        voiceUser;
    
//    if (!getline(iss, argv, ' '))
//        sendMsg(ERR_NOPARAMETER(cl.getNick(), ch.getName(), "voice", "<nick>"), cl.getfd());
//    else if (!isServerUser(argv))
//        sendMsg(ERR_NOSUCHNICK(cl.getNick(), argv), cl.getfd());
//    else{
//        if (ch.isChannelUser(argv)){
//            voiceUser = ch.isVoiceUser(argv);
//            if (op == '+' && !voiceUser){
//                changeMode(ch, successFlag, op, 'v');
//                ch.addVoiceUser(argv);
//                successFlag += (' ' + argv);
//            }
//            else if (op == '-' && voiceUser){
//                changeMode(ch, successFlag, op, 'v');
//                ch.removeVoiceUser(argv);
//                successFlag += (' ' + argv);
//            }
//        }
//    }
//}

void Server::operateFlag(Channel &ch, Client &cl, std::istringstream& iss, std::string &successFlag, char op){
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
                changeMode(ch, successFlag, op, 'o');
                ch.addOper(argv);
                successFlag += (' ' + argv);
            }
            else if (op == '-' && oper){
                changeMode(ch, successFlag, op, 'o');
                ch.removeOper(argv);
                successFlag += (' ' + argv);
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
    int         result;
    std::string target;

    while (getline(iss, target, ' ')){
        std::string successFlag;
        for (size_t i = 0; i < target.size(); i++){
            result = isValidChannelFlag(target[i]);
            if (result == 1)
                oper = target[i];
            else if (result == 2){
                if (target[i] == 'l')
                    limitFlag(it->second, cl, iss, successFlag, oper);
                else if (target[i] == 'k')
                    keyFlag(it->second, cl, iss, successFlag, oper);
                else if (target[i] == 'o')
                    operateFlag(it->second, cl, iss, successFlag, oper);
            }
            else if (result == 3){
                if (!it->second.isOperator(cl.getNick()))
                    sendMsg(ERR_CHANOPRIVSNEEDED(cl.getNick(), it->first), cl.getfd()); 
                else
                    changeMode(it->second, successFlag, oper, target[i]); 
            }
            else
                sendMsg(ERR_UNKNOWNMODE(std::string(1, target[i])), cl.getfd()); 
        }
        if (!successFlag.empty()){
            size_t lastSpace = successFlag.find_last_of(' ');
            if (lastSpace != std::string::npos)
                successFlag.insert(lastSpace + 1, ":");
            else
                successFlag.insert(0, ":");
            sendToChannelClient(it->second, cl, successFlag);
        }
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
