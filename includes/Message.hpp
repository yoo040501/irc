#pragma once

#include <iostream>

#define WAITING() (":localhost NOTICE * :*** Looking up your hostname...\r\n")

#define ERR_UNKNOWNCOMMAND(nickname, command) (":localhost 421 " + nickname + " " + command + " :Unknown command\r\n")
// NICK
#define ERR_NONICKNAMEGIVEN(nickname) (":localhost 431 " + nickname + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, command) (":localhost 432 " + nickname + " " + command + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nickname, command) (":localhost 433 " + nickname + " " + command + " :Nickname is already in use.\r\n")

// PASS, USER
#define ERR_NEEDMOREPARAMS(nickname, command) (":localhost 461 " + nickname + " " + command + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(nickname) (":localhost 462 " + nickname + " : You may not reregister\r\n")

//PRIVMEG
#define ERR_NORECIPIENT(nickname, command) (":localhost 411 " + nickname + " :No recipient given (" + command + ")\r\n")
#define ERR_NOTEXTTOSEND(nickname) (":localhost 412 " + nickname + " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channel) (":localhost 404 " + nickname + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTOPLEVEL(nickname, mask) (413"<mask> :No toplevel domain specified")
#define ERR_WILDTOPLEVEL(nickname, mask) (414"<mask> :Wildcard in toplevel domain")
#define ERR_TOOMANYTARGETS(nickname, target) (407"<target> :Duplicate recipients. No message delivered")
#define ERR_NOSUCHNICK(nickname) (401"<nickname> :No such nick/channel")
#define RPL_AWAY(nickname, command) (301"<nick> :<away message>")

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

