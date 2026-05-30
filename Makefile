##
## EPITECH PROJECT, 2026
## G-YEP-400 : Zappy - Makefile
## File description:
## Generic Makefile
##

CC = epiclang

NAME_SERVER = zappy_server
NAME_AI = zappy_ai

CFLAGS = -Wall -Wextra -I server/include

SRC_SERVER = server/src/main.c

OBJ_SERVER = $(SRC_SERVER:.c=.o)

all: $(NAME_SERVER) $(NAME_AI)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) $(OBJ_SERVER) -o $(NAME_SERVER) $(CFLAGS)

$(NAME_AI):
	cp ai/main.py $(NAME_AI)
	chmod +x $(NAME_AI)

clean:
	rm -f $(OBJ_SERVER)

fclean: clean
	rm -f $(NAME_SERVER) $(NAME_AI)

re: fclean all
