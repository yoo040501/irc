#include "../../includes/Server.hpp"

// Command: INVITE
// Parameters: <nickname> <channel>

void sendInviteMsg(Channel &channel, Client &inviter, Client &invitee) {
    sendMsg(RPL_INVITING(inviter.getNick(), invitee.getNick(), channel.getName()), inviter.getfd());
    sendMsg(":localhost INVITE " + invitee.getNick() + " " + channel.getName() + "\r\n", invitee.getfd());
    channel.addClient(invitee);
}

void handleInviteErrors(Client &inviter, Channel *channel, Client *invitee, const std::string &chanName, const std::string &inviteeName) {
    if (!inviter.getPass() || inviter.getUser().empty() || inviter.getNick() == "*") {
        throw std::logic_error(ERR_NOTREGISTERED(inviter.getNick()));
    }
    if (!channel) {
        throw std::logic_error(ERR_NOSUCHCHANNEL(inviter.getNick(), chanName));
    }
    if (!invitee) {
        throw std::logic_error(ERR_NOSUCHNICK(inviter.getNick(), inviteeName));
    }
    if (!channel->isChannelUser(inviter.getNick())) {
        throw std::logic_error(ERR_NOTONCHANNEL(inviter.getNick(), chanName));
    }
    if (!channel->isOperator(inviter.getNick())) {
        throw std::logic_error(ERR_CHANOPRIVSNEEDED(inviter.getNick(), chanName));
    }
    if (channel->isChannelUser(invitee->getNick())) {
        throw std::logic_error(ERR_USERONCHANNEL(inviter.getNick(), inviteeName, chanName));
    }
}

void Server::inviteCommand(Client &cl, const std::string &params) {
    try {
        if (params.empty()) {
            throw std::logic_error(ERR_NEEDMOREPARAMS(cl.getNick(), "INVITE"));
        }

        // Parsing
        std::istringstream iss(params);
        std::string inviteeName;
        std::string chanName;

        iss >> inviteeName >> chanName;

        // 초대 받을 클라이언트와 채널을 찾음
        int inviteeFd = nick[inviteeName];  // 초대받을 클라이언트의 파일 디스크립터 가져오기
        Client *invitee = &client[inviteeFd]; // 파일 디스크립터를 이용해 클라이언트 포인터 가져오기

        Channel *channel = nullptr;

         // 채널 이름이 '#'으로 시작하는 경우 채널 찾기
        if (chanName[0] == '#') {
            channel = &this->channel[chanName];
        }

        // 초대 오류를 처리
        handleInviteErrors(cl, channel, invitee, chanName, inviteeName);

        // 초대 메시지 전송
        sendInviteMsg(*channel, cl, *invitee);

    } catch (const std::exception &e) {
        sendMsg(e.what(), cl.getfd()); // 오류 처리
    }
}
