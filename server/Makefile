##
## EPITECH PROJECT, 2026
## zorglub
## File description:
## zorglub
##

NAME = zappy_server

SRC =	src/server_push.c								\
		src/buffer.c									\
		src/server_pop.c								\
		src/free_server.c								\
		src/arg_parser.c								\
		src/responce.c									\
		src/starter.c									\
		src/server.c									\
		src/main.c										\
		src/split.c										\
		src/clock.c										\
		src/poll_timedout.c 							\
		src/zappy_internal/timeunits.c					\
		src/sys/my_memset.c								\
		src/sys/randint.c								\
		src/sys/minmax.c								\
		src/sys/subs.c									\
		src/commands/function.c							\
		src/commands/trigger/gui_player_login.c			\
		src/commands/trigger/gui_layed_egg.c			\
		src/commands/trigger/gui_player_death.c			\
		src/commands/trigger/gui_player_eat.c			\
		src/commands/trigger/broadcast.c				\
		src/commands/trigger/gui_player_move.c			\
		src/commands/trigger/gui_tile_change.c			\
		src/commands/trigger/gui_collect.c				\
		src/commands/trigger/gui_drop.c					\
		src/commands/trigger/gui_killed_egg.c			\
		src/commands/trigger/gui_ejected.c				\
		src/commands/trigger/gui_start_incantation.c	\
		src/commands/trigger/ia_start_incantation.c		\
		src/commands/trigger/ia_end_incantation.c		\
		src/commands/trigger/gui_game_end.c				\
		src/commands/trigger/gui_end_incantation.c		\
		src/commands/trigger/gui_player_level.c			\
		src/commands/gui/protocol_collect.c				\
		src/commands/gui/protocol_drop.c				\
		src/commands/gui/protocol_start.c				\
		src/commands/gui/protocol_bad_param.c			\
		src/commands/gui/protocol_map_size.c			\
		src/commands/gui/protocol_tile.c				\
		src/commands/gui/protocol_time_unit.c			\
		src/commands/gui/protocol_broadcast.c			\
		src/commands/gui/protocol_unk.c					\
		src/commands/gui/protocol_teams.c				\
		src/commands/gui/protocol_player_connected.c	\
		src/commands/gui/protocol_player_pos.c			\
		src/commands/gui/protocol_player_level.c		\
		src/commands/gui/protocol_ejected.c				\
		src/commands/gui/protocol_killed_egg.c			\
		src/commands/gui/protocol_game_end.c			\
		src/commands/gui/protocol_player_inventory.c	\
		src/commands/gui/protocol_incantation.c			\
		src/commands/gui/protocol_egg.c					\
		src/commands/gui/protocol_player_death.c		\
		src/commands/common/protocol_ko.c				\
		src/commands/common/protocol_ok.c				\
		src/commands/ia/protocol_start.c				\
		src/commands/ia/protocol_broadcast.c			\
		src/commands/ia/protocol_connect_nbr.c			\
		src/commands/ia/protocol_death.c				\
		src/commands/ia/protocol_inventory.c			\
		src/commands/ia/protocol_fork.c					\
		src/commands/ia/protocol_look.c					\
		src/commands/ia/protocol_eject.c				\
		src/commands/ia/protocol_move.c					\
		src/commands/ia/protocol_incantation.c			\
		src/commands/ia/protocol_rotate.c				\
		src/commands/ia/protocol_bad_param.c			\
		src/commands/ia/protocol_set.c					\
		src/commands/ia/protocol_take.c 				\
		src/zappy_internal/zappy.c						\
		src/zappy_internal/resources.c					\
		src/zappy_internal/free.c						\
		src/zappy_internal/list_push.c					\
		src/zappy_internal/incantation.c				\
		src/string_util/create.c						\


OBJ = $(SRC:.c=.o)

CC = epiclang
CFLAGS  =	-Wall -Wextra -std=gnu17 -Iinclude -g \
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
