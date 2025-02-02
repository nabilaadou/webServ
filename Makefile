NAME =  webServ
CONFI = confi

SRCS =  src/srcCode/*.cpp src/server/*.cpp \
		src/request/*.cpp src/cgi/*.cpp src/exceptions/*.cpp \
		src/response/*.cpp src/wrappers/*.cpp

# SCONFI = src/srcClasses/confiClass.cpp src/srcClasses/confiClass_v2.cpp

CC = g++

CFLAGS = -Wall -Wextra -I./includes -fsanitize=address -w -g3 #-Werror #-std=c++98

CONFI: $(SCONFI)
	$(CC) $(CFLAGS) $(SRCS) -o $(CONFI)

all : $(NAME)

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAME)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

fclean :
		@rm -rf $(NAME) $(CONFI)

re : fclean all