#pragma once

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>

class Client
{
    private:
        int         		fd;
		int					ch_cnt;
		bool				pass;		// pass가 맞으면 true
		bool				passcheck; //pass를 한번이라도 건들였으면 true
		bool				auth;
        std::string 		nick;
		std::string			lownick;
		std::string			oldnick; //nick변경할때 channel operator 권한 변경때문에 사용했음
        std::string 		username;
        std::string 		hostname;
        std::string 		servername;
        std::string 		realname;
        std::string 		mode;
		struct sockaddr_in	addr;
		socklen_t			len;

    public:
		Client();
        Client(Client const& copy);
		Client& operator=(Client const& oth);
        ~Client();

		void	setfd(int f);
		void	setPass(bool flag);
		void	setPassCheck(bool flag);
		void	setAuth(bool flag);
		void	setNick(std::string const &ni);
		void	setRealNick(std::string const &ni);
		void	setUser(std::string const &un, std::string const &hn, std::string const &sn, std::string const &rn);
		void	setaddr(struct sockaddr_in &tmp);

		int					getfd() const;
		bool				getPass() const;
		bool				getPassCheck() const;
		bool				getAuth() const;
		std::string&		getNick();
		std::string&		getLowNick();
		std::string&		getOldnick();
		std::string&		getUser();
		struct sockaddr_in	getaddr();
};

int	isSpecial(char c);
