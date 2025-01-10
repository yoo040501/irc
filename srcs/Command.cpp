#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Message.hpp"

void Server::processAuth(std::string &str, Client &cl) {
    std::string tmp;
    std::string::size_type del = str.find(' ');

    tmp = str.substr(0, del);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	if (tmp == "QUIT"){
		quitCheck(trimSpace(str.substr(tmp.size())), cl);
		return; }
    if (tmp == "PASS") {
        if (del != std::string::npos)
            str.erase(0, del);
        std::string::size_type pos = str.find("NICK");
        if (!passCheck(trimSpace(str.substr(0, pos)), cl))
            return;
        if (pos != std::string::npos) {
            str.erase(0, pos);
            del = str.find(' ');
            tmp = str.substr(0, del);
        }
    }
    if (tmp == "NICK") {
        if (del != std::string::npos)
            str.erase(0, del);
        std::string::size_type pos = str.find("USER");
        nickCheck(trimSpace(str.substr(0, pos)), cl);
        if (pos != std::string::npos) {
            str.erase(0, pos);
            del = str.find(' ');
            tmp = str.substr(0, del);
        }
    }
    if (tmp == "USER") {
        str.erase(0, tmp.size());
        userCheck(trimSpace(str), cl);
    }

    if (cl.getPassCheck() == true && cl.getUser() != "" && cl.getNick() != "*") {
        if (cl.getPass() == false) {
            closeClient(ERR_CLOSE(cl.getUser(), inet_ntoa(cl.getaddr().sin_addr), "Access denied by configuration"), cl);
            return;
        } else if (cl.getAuth() == false) {
            sendMsg(MSG_WELCOME(servertime, cl.getNick()), cl.getfd());
            cl.setAuth(true);
        }
    }
}

void Server::checkCommand(char *buffer, Client &cl) {
    std::string str(buffer);

    rebuffer.push(str); // 스택에 저장 후 개행문자가 있으면 합침
    if (str.find_first_of("\r\n") != std::string::npos) {
        str.clear();
        while (!rebuffer.empty()) {
            str = rebuffer.top() + str;
            rebuffer.pop();
        }
    } else {
        return;
    }

    while (str.find_first_of("\r\n") != std::string::npos)
        str.erase(str.find_first_of("\r\n"), 1);
    str = trimSpace(str);

    std::string tmp;
    if (str[0] == ':') {
        str.erase(0, str.find(' '));
        str = trimSpace(str);
    }

    std::istringstream iss(str);
    if (cl.getAuth() == false) {
        processAuth(str, cl);
    } else {
        getline(iss, tmp, ' ');
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

        if (tmp == "PRIVMSG")
            privmsgCmd(trimSpace(str.substr(tmp.size())), cl);
		else if (cl.getBot() == true)
			bot->botMode(str, cl); // bot active
        else if (tmp == "PASS")
            passCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "NICK")
            nickCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "USER")
            userCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "JOIN")
            channelCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "PING")
            sendMsg(MSG_PONG(trimSpace(str.substr(tmp.size()))), cl.getfd());
        else if (tmp == "KICK")
            kickCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "INVITE") {

            invite(cl, trimSpace(str.substr(tmp.size())));
        }
        else if (tmp == "MODE")
            modeCmd(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "TOPIC")
            topicCheck(trimSpace(str.substr(tmp.size())), cl);
        else if (tmp == "PART")
            partCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "QUIT")
			quitCheck(trimSpace(str.substr(tmp.size())), cl);
		else if (tmp == "BOT")
			bot->botMode(trimSpace(str.substr(tmp.size())), cl);
        else {
            if (!str.empty() && tmp != "WHO")
                sendMsg(ERR_UNKNOWNCOMMAND(cl.getNick(), tmp), cl.getfd());
        }
    }
}
