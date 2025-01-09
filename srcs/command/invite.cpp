#include "../../includes/Server.hpp"
#include "../../includes/Command.hpp"
#include "../../includes/Message.hpp"

// Command: INVITE
// Parameters: <nickname> <channel>
// invite만 입력할 경우 리스트

void Command::invite(Client& client, std::vector<std::string> args) {
    // 인자 개수가 3개보다 적을 경우 처리
    if (args.size() < 2) {
        client.addToSendBuffer(ERR_UNKNOWNCOMMAND(client.getNick(), "INVITE"));
        return;
    }

    // 인자 개수가 3개보다 많을 경우 처리
    if (args.size() > 2) {
        client.addToSendBuffer(ERR_INVALIDDURATION(client.getNick()));
        return;
    }

    std::string nickname = args[1];
    std::string channel_name = args[2];

	std::transform(nickname.begin(), nickname.end(), nickname.begin(), ::tolower);
	std::transform(channel_name.begin(), channel_name.end(), channel_name.begin(), ::tolower);    
	// // 채널명이 ':'로 시작하면 제거
    // if (!channel_name.empty() && channel_name[0] == ':') {
    //     channel_name = channel_name.substr(1);
    // }

    // 채널명이 유효하지 않은 경우 처리 밑에 코드에서 다 찾을수 있지 않은지??
    if (channel_name.empty() || channel_name[0] != '#') {
        sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name), client.getfd());
        return;
    }

//    std::map<std::string, Channel>::iterator it = server_.getChannels().find(channel_name);
   std::map<std::string, Channel> channels = server_.getChannels();
    std::map<std::string, Channel>::iterator it = channels.find(channel_name);
    // 채널 존재 여부 확인
    if (it == server_.getChannels().end()) {
        sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name), client.getfd());
        return;
    }

    // 채널 내 클라이언트 존재 여부 확인
    std::vector<Client *> &clients = it->second.getClientList(); //clientList에는 아무것도 없음
    bool client_found = false;
    for (std::vector<Client *>::iterator it_client = clients.begin(); it_client != clients.end(); ++it_client) {
        if ((*it_client)->getNickname() == client.getNickname()) {
            client_found = true;
            break;
        }
    }
    if (!client_found) {
        sendMsg(ERR_NOTONCHANNEL(client.getNickname(), channel_name), client.getfd());
        return;
    }

    // 대상 클라이언트 찾기
    Client* target_client = NULL;
    std::map<int, Client>::iterator it_client;
    for (it_client = server_.getClients().begin(); it_client != server_.getClients().end(); ++it_client) {
        if (it_client->second.getNickname() == nickname) {
            target_client = &it_client->second;
            break;
        }
    }
    if (target_client == NULL) {
        sendMsg(ERR_NOSUCHNICK(client.getNickname(), nickname), client.getfd());
        return;
    }

    // 대상 클라이언트가 이미 채널에 있는지 확인
    for (std::vector<Client *>::iterator it_client = clients.begin(); it_client != clients.end(); ++it_client) {
        if ((*it_client)->getNickname() == nickname) {
            sendMsg(ERR_USERONCHANNEL(client.getNickname(), nickname, channel_name), client.getfd());
            return;
        }
    }

    // 대상 클라이언트가 초대할 채널에 이미 존재하는지 확인 위에랑 중복된 코드인지???
    if (it->second.isChannelUser(nickname)) {
        sendMsg(ERR_ALREADYONCHANNEL(client.getNickname(), nickname, channel_name), client.getfd());
        return;
    }

    // 초대 상태 업데이트
    if (!it->second.isClientInvited(*target_client)) {
        it->second.inviteClient(*target_client);
    }

    // 초대 메시지 전송
    target_client->addToSendBuffer(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " INVITE " + nickname + " " + channel_name);
    client.addToSendBuffer(":" + client.getNickname() + " INVITE " + nickname + " " + channel_name);
}
