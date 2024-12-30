CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++98 -fsanitize=address -g3

SRCS =	$(wildcard srcs/*.cpp		\
				   srcs/command/*.cpp)

OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
OBJ_DIR = obj

DEPS = $(SRCS:.cpp=.d)
NAME = ircserv

RMF = rm -rf

all : $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)


$(OBJ_DIR) :
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/srcs
	mkdir -p $(OBJ_DIR)/srcs/command

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

-include $(DEPS)

clean:
	$(RMF) $(OBJ_DIR)


fclean:
	make clean
	$(RMF) $(NAME)

re:
	make fclean
	make all

.PHONY: all clean fclean re
