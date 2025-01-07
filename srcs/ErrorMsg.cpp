#include "../includes/ErrorMsg.hpp"

const char* SocketFailException::what() const throw() {
    return "\033[0;31mSocket Created Fail.\033[0m";
}

const char* BindFailException::what() const throw() {
    return "\033[0;31mAddress Structure Created Fail.\033[0m";
}

const char* ListenFailException::what() const throw() {
    return "\033[0;31mListening on Port Fail.\033[0m";
}

const char* KqueueFailException::what() const throw() {
    return "\033[0;31mKqueue Created Fail.\033[0m";
}

const char* KeventFailException::what() const throw() {
    return "\033[0;31mKevent Created Fail.\033[0m";
}
