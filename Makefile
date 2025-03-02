NAME = webserv
CPP = g++
INC = -I ./includes
SRC = $(wildcard src/*.cpp src/http/cgi/*.cpp src/exceptions/*.cpp src/confi/*.cpp\
        src/http/*.cpp src/http/request/*.cpp src/http/response/*.cpp src/server/*.cpp src/bstring/*.cpp)
OBJ = $(SRC:.cpp=.o)
CPPFLAGS = -fsanitize=address#-Wall -Wextra -Werror -std=c++98

%.o: %.cpp
	$(CPP) $(INC) -c -o $@ $<

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)
	clear
	rm www/uploads/*
	clear

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)
	clear
	rm www/uploads/*
	clear

re: fclean all

.PHONY: clean re all fclean

.SECONDARY: $(OBJ)