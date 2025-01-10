#include "../../includes/Server.hpp"
#include "../../includes/Message.hpp"

// Command: INVITE
// Parameters: <nickname> <channel>
// invite만 입력할 경우 리스트

std::vector<std::string>	getArgs(std::string &arg){
	std::vector<std::string>	args;
	if (arg[0] == ':')
		args.push_back(getCMD(arg));
	else{
		std::istringstream argStream(arg);
		std::string token;

		while (std::getline(argStream, token, ' ')) {
            if (token[0] == ':') {
                std::string remaining;
                while (std::getline(argStream, remaining, ' ')) {
                    token += " " + remaining;
                }
				token.erase(0, 1);
                args.push_back(token);
                break;
            }
			else
				args.push_back(token);
		}
	}
	return args;
}

void Server::invite(Client& clien, std::string arg) {
    // 인자 개수가 2개보다 적을 경우 처리
	std::vector<std::string> args = getArgs(arg);
    if (args.size() < 2) {
		std::vector<std::string> List = clien.getInvitedChannel();
		std::vector<std::string>::iterator it = List.begin();
		while (it != List.end()){
			sendMsg(RPL_INVITELIST(clien.getNickname(), *it), clien.getfd());
			++it;
		}
        sendMsg(RPL_INVITEEND(clien.getNickname()), clien.getfd());
        return;
    }
    // 인자 개수가 2개보다 많을 경우 처리
    if (args.size() > 2) {
        sendMsg(ERR_INVALIDDURATION(clien.getNick()), clien.getfd());
        return;
    }
    std::string nickname = args[0];
    std::string channel_name = args[1];

	std::transform(nickname.begin(), nickname.end(), nickname.begin(), ::tolower);
	std::transform(channel_name.begin(), channel_name.end(), channel_name.begin(), ::tolower);    

    std::map<std::string, Channel>::iterator it = channel.find(channel_name);
    // 채널 존재 여부 확인
    if (it == channel.end()) {
        sendMsg(ERR_NOSUCHCHANNEL(clien.getNickname(), channel_name), clien.getfd());
        return;
    }

	if (!isExistUSER(clien.getNickname(), nick)){
		sendMsg(ERR_NOTONCHANNEL(clien.getNickname(), it->second.getName()), clien.getfd());
		return ;
	}

	if (!it->second.isOperator(clien.getNick()))
	{
		return sendMsg(ERR_CHANOPRIVSNEEDED(clien.getNick(), it->second.getName()), clien.getfd());
	}
	std::map<int, Client>::iterator it_client;
    // // 대상 클라이언트 찾기
    Client* target_client = NULL;
    for (it_client = client.begin(); it_client != client.end(); it_client++) {
        if (it_client->second.getLowNick() == nickname) {
            target_client = &it_client->second;
            break;
        }
    }
    if (target_client == NULL) {
        sendMsg(ERR_NOSUCHNICK(clien.getNickname(), nickname), clien.getfd());
        return;
    }

    //대상 클라이언트가 초대할 채널에 이미 존재하는지 확인 위에랑 중복된 코드인지???
    if (it->second.isChannelUser(nickname)) {
        sendMsg(ERR_ALREADYONCHANNEL(clien.getNickname(), nickname, it->second.getName()), clien.getfd());
        return;
    }

    //초대 상태 업데이트
    if (!it->second.isClientInvited(*target_client)) {
        it->second.inviteClient(*target_client);
    }

    // 초대 메시지 전송
	sendMsg(RPL_INVITING(clien.getNick(), target_client->getNick(), it->second.getName()), clien.getfd());
	sendMsg(RPL_AWAY_INVITE(clien.getNick(), clien.getUser(), target_client->getNick(), inet_ntoa(clien.getaddr().sin_addr), it->second.getName()), target_client->getfd());
}
