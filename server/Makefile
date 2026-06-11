##
## EPITECH PROJECT, 2026
## zorglub
## File description:
## zorglub
##

NAME = ../zappy_server

SRC =	src/commands/err_check.c	\
		src/server_push.c			\
		src/buffer.c				\
		src/server_pop.c			\
		src/free_server.c			\
		src/arg_parser.c			\
		src/responce.c				\
		src/starter.c				\
		src/server.c				\
		src/main.c					\
		src/database/channel.c		\
		src/database/dm.c			\
		src/database/reply.c		\
		src/database/subscription.c	\
		src/database/team.c			\
		src/database/thread.c		\
		src/database/user.c			\
		src/split.c					\
		src/clock.c					\
		src/zappy_internal/timeunits.c	\
		src/sys/my_memset.c			\


OBJ = $(SRC:.c=.o)

CC = epiclang
CFLAGS  =	-Wall -Wextra -std=gnu17 -Iinclude \
			-I../common/include
LDFLAGS = -luuid

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
