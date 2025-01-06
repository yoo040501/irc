#include "../../includes/Server.hpp"
/* Command: PRIVMSG
   Parameters: <receiver>{,<receiver>} <text to be sent>

   PRIVMSG is used to send private messages between users.  <receiver>
   is the nickname of the receiver of the message.  <receiver> can also
   be a list of names or channels separated with commas.

   The <receiver> parameter may also me a host mask  (#mask)  or  server
   mask  ($mask).   In  both cases the server will only send the PRIVMSG
   to those who have a server or host matching the mask.  The mask  must
   have at  least  1  (one)  "."  in it and no wildcards following the
   last ".".  This requirement exists to prevent people sending messages
   to  "#*"  or "$*",  which  would  broadcast  to  all  users; from
   experience, this is abused more than used responsibly and properly.
   Wildcards are  the  '*' and  '?'   characters.   This  extension  to
   the PRIVMSG command is only available to Operators.
   
   Numeric Replies:
           ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
           ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
           ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
           ERR_NOSUCHNICK
           RPL_AWAY
   Examples:
:Angel PRIVMSG Wiz :Hello are you receiving this message ?
                                ; Message from Angel to Wiz.
PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br ;
                                Message to Angel.
PRIVMSG jto@tolsun.oulu.fi :Hello !
                                ; Message to a client on server
PRIVMSG $*.fi :Server tolsun.oulu.fi rebooting.
                                ; Message to everyone on a server which
                                has a name matching *.fi.

PRIVMSG #*.edu :NSFNet is undergoing work, expect interruptions
                                ; Message to all users who come from a
                                host which has a name matching *.edu.*/
								
// void	getReceiver(std::string str, std::vector<std::string> &receiver){
// 	std::string			tmp;
// 	std::string			tmp_rec;
// 	std::istringstream	iss(str);
	
// 	getline(iss, tmp, ':');
// 	if (tmp.find(',') != std::string::npos) // receiver가 두개 이상인 경우
// 	{
// 		while (1){}
// 	}
// 	receiver[0] = "a";
// }
// /* 1. str에 :가 있을 경우
// 		1) :로 시작하면 noreceiver 에러
// 		2) :전에 공백이 없으면 receiver로 처리
// 		3) :전전에 공백이 있으면 거기부터 공백 포함 x send messag       e ex) a, b :hello ->  a한테만 b :hello로 메세지 보냄
// 	2. :가 없을 경우
// 		1) */
// void	Server::msgCheck(std::string str, Client &cl){ // ,이랑 공백이 :보다 우선순위
// 	std::vector<std::string> receiver;
// 	size_t	pos;
// 	return ;
// 	// pos = str.find_first_of(" ,"); // a, b : he
// 	pos = str.find(':');
// 	if (pos != std::string::npos){ // :<text to be sent> :있으면 공백도 포함해서 다 보냄
// 		if (pos == 0)	//no receiver
// 			sendMsg(ERR_NORECIPIENT(cl.getNick(), str), cl.getfd());
// 		else if (str[pos - 1] != ' '){ // 없는 nick이나 channel일 경우
// 					// receiver : <message>      없는 nick이나 channel일 경우, mask는 아직 잘 모르겠음
// 			sendMsg(ERR_NOSUCHNICK(cl.getNick(), str), cl.getfd());
// 		}
// 		// else if (asd){ // send msg가 없을 경우  :앞에 공백이 없어도 error
// 		// 	sendMsg(ERR_NOTEXTTOSEND(cl.getNick()), cl.getfd());
// 		// }
// 		else {

// 		}
// 	}
// 	else{ //: 없으면 공백으로 split

// 	}
// }

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

static bool hasDuplicate(std::vector<std::string>& success, std::string& target){
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
