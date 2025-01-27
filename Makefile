NAME =  webServ
CONFI = confi

SRCS = src/srcClasses/*.cpp  src/srcCode/*.cpp
SCONFI = src/srcClasses/confiClass.cpp

CC = g++

CFLAGS = -Wall -Wextra -I./includes -fsanitize=address -g3 #-Werror #-std=c++98

CONFI: $(SCONFI)
	$(CC) $(CFLAGS) $(SCONFI) -o $(CONFI)

all : $(NAME)

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

fclean :
		@rm -rf $(NAME)

re : fclean all