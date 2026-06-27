##
## EPITECH PROJECT, 2026
## Zappy Server Tests
## File description:
## Makefile for Criterion unit tests
##

SERVER_DIR	= ../../server

SRC_SERVER	=	$(SERVER_DIR)/src/split.c				\
				$(SERVER_DIR)/src/buffer.c				\
				$(SERVER_DIR)/src/clock.c				\
				$(SERVER_DIR)/src/arg_parser.c			\
				$(SERVER_DIR)/src/responce.c				\
				$(SERVER_DIR)/src/server.c				\
				$(SERVER_DIR)/src/server_push.c			\
				$(SERVER_DIR)/src/server_pop.c			\
				$(SERVER_DIR)/src/free_server.c			\
				$(SERVER_DIR)/src/starter.c				\
				$(SERVER_DIR)/src/poll_timedout.c		\
				$(SERVER_DIR)/src/zappy_internal/timeunits.c	\
				$(SERVER_DIR)/src/zappy_internal/zappy.c	\
				$(SERVER_DIR)/src/zappy_internal/resources.c	\
				$(SERVER_DIR)/src/zappy_internal/list_push.c	\
				$(SERVER_DIR)/src/zappy_internal/free.c		\
				$(SERVER_DIR)/src/zappy_internal/incantation.c	\
				$(SERVER_DIR)/src/sys/my_memset.c		\
				$(SERVER_DIR)/src/sys/randint.c			\
				$(SERVER_DIR)/src/sys/minmax.c			\
				$(SERVER_DIR)/src/sys/subs.c				\
				$(SERVER_DIR)/src/string_util/create.c	\
				$(SERVER_DIR)/src/commands/function.c	\
				$(SERVER_DIR)/src/commands/common/protocol_ko.c	\
				$(SERVER_DIR)/src/commands/common/protocol_ok.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_start.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_broadcast.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_connect_nbr.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_death.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_inventory.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_fork.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_look.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_eject.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_move.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_incantation.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_rotate.c		\
				$(SERVER_DIR)/src/commands/ia/protocol_bad_param.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_set.c			\
				$(SERVER_DIR)/src/commands/ia/protocol_take.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_collect.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_drop.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_start.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_bad_param.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_map_size.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_tile.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_time_unit.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_broadcast.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_unk.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_teams.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_player_connected.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_player_pos.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_player_level.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_ejected.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_killed_egg.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_game_end.c		\
				$(SERVER_DIR)/src/commands/gui/protocol_player_inventory.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_incantation.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_egg.c			\
				$(SERVER_DIR)/src/commands/gui/protocol_player_death.c	\
				$(SERVER_DIR)/src/commands/trigger/gui_player_login.c	\
				$(SERVER_DIR)/src/commands/trigger/gui_layed_egg.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_player_death.c	\
				$(SERVER_DIR)/src/commands/trigger/gui_player_eat.c		\
				$(SERVER_DIR)/src/commands/trigger/broadcast.c			\
				$(SERVER_DIR)/src/commands/trigger/gui_player_move.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_tile_change.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_collect.c			\
				$(SERVER_DIR)/src/commands/trigger/gui_drop.c			\
				$(SERVER_DIR)/src/commands/trigger/gui_killed_egg.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_ejected.c			\
				$(SERVER_DIR)/src/commands/trigger/gui_start_incantation.c	\
				$(SERVER_DIR)/src/commands/trigger/ia_start_incantation.c	\
				$(SERVER_DIR)/src/commands/trigger/ia_end_incantation.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_game_end.c			\
				$(SERVER_DIR)/src/commands/trigger/gui_end_incantation.c		\
				$(SERVER_DIR)/src/commands/trigger/gui_player_level.c		\

SRC_TEST_SERVER	= test_server.c
SRC_TEST_GAME	= test_game.c

CC		= epiclang
CFLAGS	= -Wall -Wextra -std=gnu17 \
		  -I$(SERVER_DIR)/include	\
		  -I../../common/include
LDFLAGS	= -lcriterion -luuid --coverage

NAME_SERVER	= test_server_bin
NAME_GAME	= test_game_bin

all: $(NAME_SERVER) $(NAME_GAME)

$(NAME_SERVER): $(SRC_SERVER) $(SRC_TEST_SERVER)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(NAME_GAME): $(SRC_SERVER) $(SRC_TEST_GAME)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests_run: all
	@echo "=== Running test_server ==="
	./$(NAME_SERVER) --verbose
	@echo ""
	@echo "=== Running test_game ==="
	./$(NAME_GAME) --verbose

clean:
	rm -f $(NAME_SERVER) $(NAME_GAME)
	rm -f *.gcno *.gcda *.gcov

fclean: clean

re: fclean all

.PHONY: all clean fclean re tests_run
