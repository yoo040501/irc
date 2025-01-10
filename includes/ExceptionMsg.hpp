#pragma once

#include <iostream>

class SocketFailException: public std::exception{
	public:
		virtual const char* what() const throw();
};

class BindFailException: public std::exception{
	public:
		virtual const char* what() const throw();
};

class ListenFailException: public std::exception{
	public:
		virtual const char* what() const throw();
};

class KqueueFailException: public std::exception{
	public:
		virtual const char* what() const throw();
};

class KeventFailException: public std::exception{
	public:
		virtual const char* what() const throw();
};
