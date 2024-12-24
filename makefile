CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++98 

SRCS =	$(wildcard srcs/*.cpp		\
				   srcs/command/*.cpp)

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
NAME = ircserv

RMF = rm -rf
TOUCH = touch
MAKE = make

all : $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include $(DEPS)

clean:
	$(RMF) $(OBJS) $(DEPS)

fclean:
	$(MAKE) clean
	$(RMF) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re
