#include "../../includes/Server.hpp"
#include "../../includes/Command.hpp"
#include "../../includes/Message.hpp"

// Command: INVITE
// Parameters: <nickname> <channel>
// 인자 3개가 아닌 경우 에러 처리

void Command::invite(Client& client, std::vector<std::string> args) {
    if (args.size() < 3) {
        // 두 번째 인자로 args[0]을 전달
        client.addToSendBuffer(NEEDMOREPARAMS(client.getNick(), "INVITE"));
        return;
    }
	std::string nickname = args[1];
	std::string channel_name = args[2];
	std::map<std::string, Channel>::iterator it = server_.getChannels().find(channel_name);
	if (it == server_.getChannels().end()) {
		sendMsg(ERR_NOSUCHCHANNEL(client.getNickname(), channel_name), client.getfd());
		return;
	}
	// channel안에 해당 client가 없으면 에러 처리
	std::vector<Client *> &clients = it->second.getClientList();
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

	// 초대 상태 업데이트
	if (!it->second.isClientInvited(*target_client)) {
		it->second.inviteClient(*target_client);
	}

	// 초대 메시지 전송
	target_client->addToSendBuffer(":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " INVITE " + nickname + " " + channel_name);
	client.addToSendBuffer(":" + client.getNickname() + " INVITE " + nickname + " " + channel_name);
}
