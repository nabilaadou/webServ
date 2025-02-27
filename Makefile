NAME = webserv
CPP = g++
INC = -I ./includes
SRC = $(wildcard src/*.cpp src/cgi/*.cpp src/exceptions/*.cpp src/confi/*.cpp\
        src/request/*.cpp src/response/*.cpp src/server/*.cpp src/bstring/*.cpp)
OBJ = $(SRC:.cpp=.o)
CPPFLAGS = -fsanitize=address#-Wall -Wextra -Werror -std=c++98

%.o: %.cpp
	$(CPP) $(INC) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)
	clear
	rm www/uploads/*

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean re all fclean

.SECONDARY: $(OBJ)