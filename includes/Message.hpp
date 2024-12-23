#pragma once
/* 서버에서 클라이언트한테 보내는 메세지 넘버 */

// PASS, NICK, USER 입력후 서버 접속 메세지
#define W() (":localhost 001 : Welcome to the Localnet IRC Network\r\n")
#define E() (":localhost 002 : Your host is localhost, running version one!\r\n")
#define L(time) (":localhost 003 :This server was created " + time + "\r\n")
#define C() (":localhost 004 :localhost IRC_SERVER iosw biklmnopstv :bklov\r\n") // 마지막 저거 뭔지 모름 (iosw biklmnopstv :bklov)
#define O() (":localhost 375 :localhost message of the day.\r\n")
#define M() (":localhost 376 :End of message of the day.\r\n")

#define ERR_UNKNOWNCOMMAND(command, nickname) (":localhost 421 " + nickname + " " + command + " :Unknown command\r\n")
// NICK
#define ERR_NONICKNAMEGIVEN(client) (":localhost 431 " + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nickname) (":localhost 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nickname) (":localhost 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")

// PASS, USER
#define ERR_NEEDMOREPARAMS(command) (":localhost 461 " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED() (":localhost 462 : You may not reregister\r\n")

//PRIVMEG
#define ERR_NORECIPIENT
#define ERR_NOTEXTTOSEND
#define ERR_CANNOTSENDTOCHAN
#define ERR_NOTOPLEVEL
#define ERR_WILDTOPLEVEL
#define ERR_TOOMANYTARGETS
#define ERR_NOSUCHNICK
#define RPL_AWAY

// JOIN
#define ERR_BANNEDFROMCHAN
#define ERR_INVITEONLYCHAN
#define ERR_BADCHANNELKEY
#define ERR_CHANNELISFULL
#define ERR_BADCHANMASK
#define ERR_NOSUCHCHANNEL
#define ERR_TOOMANYCHANNELS
#define RPL_TOPIC

// INVITE
#define ERR_USERONCHANNEL
#define RPL_INVITING

// TOPIC
#define ERR_NOTONCHANNEL
#define RPL_NOTOPIC
#define ERR_CHANOPRIVSNEEDED

// MODE
#define RPL_CHANNELMODEIS
#define ERR_KEYSET
#define RPL_BANLIST
#define RPL_ENDOFBANLIST
#define ERR_UNKNOWNMODE

#define ERR_USERSDONTMATCH
#define RPL_UMODEIS
#define ERR_UMODEUNKNOWNFLAG
