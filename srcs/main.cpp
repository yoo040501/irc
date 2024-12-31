#include "../includes/Server.hpp"

int main(int ac, char **av)
{
	// std::time_t a = std::time(NULL); 유닉스 타임스탬프
    if (ac == 3)
    {
        try
        {
			time_t	timer = time(NULL);
			struct tm *t;
			t = localtime(&timer);
            Server serv(av[1], av[2]); //port번호와 password있는 서버 생성
            serv.openSocket();		   //소켓 생성
			serv.active();			   //서버 활성화
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }    
    }
    return 0;
}
