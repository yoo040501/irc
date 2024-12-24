#pragma once

#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "Client.hpp"

class Client;

class Channel
{
	private:
		/* data */
	public:
		Channel(/* args */);
		~Channel();
};
