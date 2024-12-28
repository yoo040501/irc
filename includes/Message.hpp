#pragma once

#include <iostream>
#include <cstdarg>
#include <vector>

#define WAITING() (":localhost NOTICE * :*** Looking up your hostname...\r\n")

#define RPL_CAPLS(nickname) (":localhost 410 " + nickname +  "LS :Invalid CAP command\r\n")

#define ERR_UNKNOWNCOMMAND(nickname, command) (":localhost 421 " + nickname + " " + command + " :Unknown command\r\n")
// NICK
#define ERR_NONICKNAMEGIVEN(nickname) (":localhost 431 " + nickname + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, command) (":localhost 432 " + nickname + " " + command + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nickname, command) (":localhost 433 " + nickname + " " + command + " :Nickname is already in use.\r\n")
#define RPL_NICK(oldnick, username, ipaddr, newnick) (":" + oldnick + "!" + username + "@"+ ipaddr + " NICK :" + newnick + "\r\n")
// PASS, USER
#define ERR_NEEDMOREPARAMS(nickname, command) (":localhost 461 " + nickname + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(nickname) (":localhost 462 " + nickname + " : You may not reregister\r\n")

//PRIVMEG
#define ERR_NORECIPIENT(nickname, command) (":localhost 411 " + nickname + " :No recipient given (" + command + ")\r\n")
#define ERR_NOTEXTTOSEND(nickname) (":localhost 412 " + nickname + " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channel) (":localhost 404 " + nickname + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTOPLEVEL(nickname, mask) (":localhost 413 " + nickname + "<mask> :No toplevel domain specified\r\n")
#define ERR_WILDTOPLEVEL(nickname, mask) (":localhost 414 " + nickname + "<mask> :Wildcard in toplevel domain\r\n")
#define ERR_TOOMANYTARGETS(nickname, target) (":localhost 407 " + nickname +"<target> :Duplicate recipients. No message delivered\r\n")
#define ERR_NOSUCHNICK(nickname, nick) (":localhost 401 " + nickname + " " + nick + " :No such nick/channel\r\n")
#define RPL_AWAY(nickname, username, ipaddr, message) (":" + nickname + "!" + username + "@" + ipaddr + " PRIVMSG" + nickname + " :" + message + "\r\n")

// JOIN
#define ERR_CHANNELISFULL(nickname, channel) (":localhost 471 " + nickname + " " + channel + ":Cannot join channel (l)\r\n")
#define ERR_INVITEONLYCHAN(nickname, channel) (":localhost 473 " + nickname + " " + channel + ":Cannot join channel (i)\r\n")
#define ERR_BANNEDFROMCHAN(nickname, channel) (":localhost 474 " + nickname + " " + channel + ":Cannot join channel (b)\r\n")
#define ERR_BADCHANNELKEY(nickname, channel) (":localhost 475 " + nickname + " " + channel + ":Cannot join channel (incorrect channel key)\r\n")
#define ERR_NOSUCHCHANNEL(nickname, channel) (":localhost 403 " + nickname + " " + channel + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(nickname, channel) (":localhost 405 " + nickname + " " + channel + " :you have joined too many channels\r\n")
#define RPL_TOPIC(nickname, username, ipaddr, channel, topic) (":" + nickname + "!" + username + "@" + ipaddr + " " + channel + " :" + topic + "\r\n")
std::string RPL_NAMREPLY(const std::string &nickname, const std::string &type, const std::string &channel, const std::vector<std::string>& nick);
/*=: 채널 타입
	=: 공개 채널(public channel).
	*: 비밀 채널(private channel).
	@: 비공개 채널(secret channel).
	private && secret -> @	
	*/

#define RPL_ENDOFNAMES(nickname, channel) (":localhost 366 " + nickname + " " + channel + " :End of /NAMES list.\r\n")
#define ERR_USERNOTINCHANNEL(nickname, user, channel) (":localhost 441 " + nickname + " " + user + " " + channel + " :They aren't on that channel\r\n")
#define	RPL_JOIN(nickname, username, ipaddr, channel) (":" + nickname + "!" + username + "@" + ipaddr + " JOIN :" + channel + "\r\n")
#define RPL_KICK(nickname, username, ipaddr, channel, user, msg) (":" + nickname + "!" + username + "@" + ipaddr + " KICK " + channel + " " + user + " :" + msg + "\r\n")
// PING
# define MSG_PONG(cmd) (":irc.local PONG irc.local :" + cmd + "\r\n")

// INVITE
#define ERR_USERONCHANNEL
#define RPL_INVITING

// TOPIC
#define ERR_NOTONCHANNEL(nickname, channel) (":localhost 442 " + nickname + " " + channel + " :You're not on that channel\r\n")
#define RPL_NOTOPIC
#define ERR_CHANOPRIVSNEEDED(nickname, channel) (":localhost 482 " + nickname + " " + channel + " :You're not channel operator\r\n")

// MODE
#define RPL_CHANNELMODEIS
#define ERR_KEYSET
#define RPL_BANLIST
#define RPL_ENDOFBANLIST
#define ERR_UNKNOWNMODE(c) (c + " :is unknown mode char to me\r\n") 
#define ERR_CHANOPRIVSNEEDED(nickname, channel) (":localhost 482 " + nickname + " #" + channel + " :You're not channel operator\r\n")
#define ERR_NOPARAMETER(nickname, channel, mode, parameter) (":localhost 696 " + nickname + " #" + channel + ":You must specify a parameter for the " + mode + " mode. Syntax:" + parameter + "\r\n")
#define ERR_USERSDONTMATCH
#define RPL_UMODEIS
#define ERR_UMODEUNKNOWNFLAG


// PASS, NICK, USER 입력후 서버 접속 메세지
#define MSG_WELCOME(Time, nickname) (":localhost 001 " + nickname + " : Welcome to the Localnet IRC Network\r\n" + \
						":localhost 002 " + nickname + " : Your host is localhost!\r\n" + \
						":localhost 003 " + nickname + " :This server was created " + Time + "\r\n" + \
						":localhost 004 " + nickname + " :localhost IRC_SERVER iosw biklmnopstv :bklov\r\n" + \
						":localhost 375 " + nickname + " :localhost message of the day.\r\n" + \
						":localhost 372 " + nickname + " :- **************************************************\r\n" + \
						":localhost 372 " + nickname + " :- *             H    E    L    L    O              *\r\n" + \
						":localhost 372 " + nickname + " :- *  This is a irc subject server. Please don`t    *\r\n" + \
						":localhost 372 " + nickname + " :- *  contact the admin of the server for any       *\r\n" + \
						":localhost 372 " + nickname + " :- *  questions or issues.                          *\r\n" + \
						":localhost 372 " + nickname + " :- **************************************************\r\n" + \
						":localhost 376 " + nickname + " :End of message of the day.\r\n")

void	sendMsg(std::string msg, int fd);
