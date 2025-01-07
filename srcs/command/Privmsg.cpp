#include "../../includes/Server.hpp"

static void     splitReceiver(std::vector<std::string>& receiverList, std::string& str){
        std::istringstream              iss(str);
        std::string                     tmp;

        while (getline(iss, tmp, ','))
        {
                if (!tmp.empty())
                        receiverList.push_back(tmp);
        }
}

static std::string    removeSpace(std::string& msg){
        bool isSpace = false;
        std::string     ret;

        for (size_t i = 0; i < msg.size(); i++){
                if (msg[i] == ' '){
                        if (isSpace == false){
                                ret += ' ';
                                isSpace = true;
                        }
                }
                else{
                        ret += msg[i];
                        isSpace = false;
                }
        }
        return ret;
}

static std::string     messageParser(std::string& msg){
        size_t          colon;
        size_t          pos;
        std::string     ret;
        std::string     remove;

        colon = count(msg.begin(), msg.end(), ':');
        if (colon){
                pos = msg.find(':');
                remove = msg.substr(0, pos);
                ret += removeSpace(remove);
                if (pos != 0 && msg[pos - 1] != ' '){
                        remove = msg.substr(pos + 1);
                        ret += removeSpace(remove);
                }
                else
                        ret += msg.substr(pos + 1);
                return ret;
        }
        else
                return msg;
}

static void    sendPrivmsgToChannelClient(Channel &ch, Client &cl, std::string& msg){
    std::map<int, Client>	tmp = ch.getClient();
    std::map<int, Client>::iterator it = tmp.begin();
    while (it != tmp.end()){
        if (it->second.getNick() != cl.getNick())
                sendMsg(RPL_AWAY(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), msg), it->second.getfd());
        ++it;
    }
}

bool hasDuplicate(std::vector<std::string>& success, std::string& target){
        std::vector<std::string>::iterator it = find(success.begin(), success.end(), target);
        if (it != success.end())
                return true;
        else
                return false;
}

void    Server::privmsgCmd(std::string str, Client &cl){
        std::string                     tmp;
        std::istringstream              iss(str);
        std::vector<std::string>        receiver;
        std::vector<std::string>        success;
        std::string                     msg;

        if (str.empty() || !getline(iss, tmp, ' ') || tmp[0] == ':'){
                sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "PRIVMSG"), cl.getfd());
                return ;
	}
        splitReceiver(receiver, tmp);
        
        if (!getline(iss, tmp)){
                sendMsg(ERR_NEEDMOREPARAMS(cl.getNick(), "PRIVMSG"), cl.getfd());
                return ;
        }
        msg = messageParser(tmp);
        
        for (std::vector<std::string>::iterator it = receiver.begin(); it != receiver.end(); it++){
                if ((*it)[0] == '#'){
                        std::transform(it->begin(), it->end(), it->begin(), ::tolower);
                        if (hasDuplicate(success, *it))
                                return ;
                        else if (isValidChname(*it)){
                                sendPrivmsgToChannelClient(getChannel(*it), cl, msg);
                                success.push_back(*it);
                        }
                        else
                                sendMsg(ERR_NOSUCHCHANNEL(cl.getNick(), *it), cl.getfd());
                }
                else{
                        if (hasDuplicate(success, *it))
                                return ;
                        else if (isServerUser(*it)){
                                sendMsg(RPL_AWAY(cl.getNick(), cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), msg), getClient(*it).getfd());
                                success.push_back(*it);
                        }
                        else
                                sendMsg(ERR_NOSUCHNICK(cl.getNick(), *it), cl.getfd());
                }
        }
}
