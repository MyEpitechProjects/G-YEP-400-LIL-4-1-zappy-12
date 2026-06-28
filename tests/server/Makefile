##
## EPITECH PROJECT, 2026
## Zappy Server Tests
## File description:
## Makefile for Criterion unit tests
##

SERVER_DIR	= ../../server

SRC_SERVER	=	$(SERVER_DIR)/src/split.c \
				$(SERVER_DIR)/src/buffer.c \
				$(SERVER_DIR)/src/clock.c \
				$(SERVER_DIR)/src/arg_parser.c \
				$(SERVER_DIR)/src/responce.c \
				$(SERVER_DIR)/src/server.c \
				$(SERVER_DIR)/src/server_push.c \
				$(SERVER_DIR)/src/server_pop.c \
				$(SERVER_DIR)/src/free_server.c \
				$(SERVER_DIR)/src/starter.c \
				$(SERVER_DIR)/src/poll_timedout.c \
				$(SERVER_DIR)/src/zappy_internal/timeunits.c \
				$(SERVER_DIR)/src/zappy_internal/zappy.c \
				$(SERVER_DIR)/src/zappy_internal/resources.c \
				$(SERVER_DIR)/src/zappy_internal/list_push.c \
				$(SERVER_DIR)/src/zappy_internal/free.c	\
				$(SERVER_DIR)/src/zappy_internal/incantation.c \
				$(SERVER_DIR)/src/sys/my_memset.c \
				$(SERVER_DIR)/src/sys/randint.c \
				$(SERVER_DIR)/src/sys/minmax.c \
				$(SERVER_DIR)/src/sys/subs.c \
				$(SERVER_DIR)/src/string_util/create.c \
				$(SERVER_DIR)/src/commands/function.c \
				$(SERVER_DIR)/src/commands/common/protocol_ko.c	\
				$(SERVER_DIR)/src/commands/common/protocol_ok.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_start.c \
				$(SERVER_DIR)/src/commands/ia/protocol_broadcast.c \
				$(SERVER_DIR)/src/commands/ia/protocol_connect_nbr.c \
				$(SERVER_DIR)/src/commands/ia/protocol_death.c \
				$(SERVER_DIR)/src/commands/ia/protocol_inventory.c \
				$(SERVER_DIR)/src/commands/ia/protocol_fork.c \
				$(SERVER_DIR)/src/commands/ia/protocol_look.c \
				$(SERVER_DIR)/src/commands/ia/protocol_eject.c \
				$(SERVER_DIR)/src/commands/ia/protocol_move.c \
				$(SERVER_DIR)/src/commands/ia/protocol_incantation.c \
				$(SERVER_DIR)/src/commands/ia/protocol_rotate.c	\
				$(SERVER_DIR)/src/commands/ia/protocol_bad_param.c \
				$(SERVER_DIR)/src/commands/ia/protocol_set.c \
				$(SERVER_DIR)/src/commands/ia/protocol_take.c \
				$(SERVER_DIR)/src/commands/gui/protocol_collect.c \
				$(SERVER_DIR)/src/commands/gui/protocol_drop.c \
				$(SERVER_DIR)/src/commands/gui/protocol_start.c \
				$(SERVER_DIR)/src/commands/gui/protocol_bad_param.c \
				$(SERVER_DIR)/src/commands/gui/protocol_map_size.c \
				$(SERVER_DIR)/src/commands/gui/protocol_tile.c \
				$(SERVER_DIR)/src/commands/gui/protocol_time_unit.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_broadcast.c	\
				$(SERVER_DIR)/src/commands/gui/protocol_unk.c \
				$(SERVER_DIR)/src/commands/gui/protocol_teams.c \
				$(SERVER_DIR)/src/commands/gui/protocol_player_connected.c \
				$(SERVER_DIR)/src/commands/gui/protocol_player_pos.c \
				$(SERVER_DIR)/src/commands/gui/protocol_player_level.c \
				$(SERVER_DIR)/src/commands/gui/protocol_ejected.c \
				$(SERVER_DIR)/src/commands/gui/protocol_killed_egg.c \
				$(SERVER_DIR)/src/commands/gui/protocol_game_end.c \
				$(SERVER_DIR)/src/commands/gui/protocol_player_inventory.c \
				$(SERVER_DIR)/src/commands/gui/protocol_incantation.c \
				$(SERVER_DIR)/src/commands/gui/protocol_egg.c \
				$(SERVER_DIR)/src/commands/gui/protocol_player_death.c \
				$(SERVER_DIR)/src/commands/trigger/gui_player_login.c \
				$(SERVER_DIR)/src/commands/trigger/gui_layed_egg.c \
				$(SERVER_DIR)/src/commands/trigger/gui_player_death.c \
				$(SERVER_DIR)/src/commands/trigger/gui_player_eat.c	\
				$(SERVER_DIR)/src/commands/trigger/broadcast.c \
				$(SERVER_DIR)/src/commands/trigger/gui_player_move.c \
				$(SERVER_DIR)/src/commands/trigger/gui_tile_change.c \
				$(SERVER_DIR)/src/commands/trigger/gui_collect.c \
				$(SERVER_DIR)/src/commands/trigger/gui_drop.c \
				$(SERVER_DIR)/src/commands/trigger/gui_killed_egg.c \
				$(SERVER_DIR)/src/commands/trigger/gui_ejected.c \
				$(SERVER_DIR)/src/commands/trigger/gui_start_incantation.c \
				$(SERVER_DIR)/src/commands/trigger/ia_start_incantation.c \
				$(SERVER_DIR)/src/commands/trigger/ia_end_incantation.c	\
				$(SERVER_DIR)/src/commands/trigger/gui_game_end.c \
				$(SERVER_DIR)/src/commands/trigger/gui_end_incantation.c \
				$(SERVER_DIR)/src/commands/trigger/gui_player_level.c \

SRC_TEST_SERVER	= test_server.c
SRC_TEST_GAME	= test_game.c

CC		= clang
CFLAGS	= -Wall -Wextra -std=gnu17 -I$(SERVER_DIR)/include -I../../common/include
COV		= --coverage
LDFLAGS	= -lcriterion -luuid --coverage

NAME_SERVER	= test_server_bin
NAME_GAME	= test_game_bin

BUILD_DIR	= build
OBJ_DIR		= $(BUILD_DIR)/obj

OBJ_SHARED	= $(patsubst $(SERVER_DIR)/src/%.c,$(OBJ_DIR)/%.o,$(SRC_SERVER))
OBJ_SERVER	= $(OBJ_SHARED) $(OBJ_DIR)/$(SRC_TEST_SERVER:.c=.o)
OBJ_GAME	= $(OBJ_SHARED) $(OBJ_DIR)/$(SRC_TEST_GAME:.c=.o)

define compile_tu
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(COV) -c $< -o $@
	@$(RM) $(@:.o=.gcda)
endef

all: $(NAME_SERVER) $(NAME_GAME)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) -o $@ $^ $(LDFLAGS)

$(NAME_GAME): $(OBJ_GAME)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SERVER_DIR)/src/%.c
	$(compile_tu)

$(OBJ_DIR)/$(SRC_TEST_SERVER:.c=.o): $(SRC_TEST_SERVER)
	$(compile_tu)

$(OBJ_DIR)/$(SRC_TEST_GAME:.c=.o): $(SRC_TEST_GAME)
	$(compile_tu)

tests_run: all
	@find $(BUILD_DIR) -name '*.gcda' -delete 2>/dev/null || true
	./$(NAME_SERVER) --verbose
	./$(NAME_GAME) --verbose

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) $(NAME_SERVER) $(NAME_GAME)
	$(RM) *.gcno *.gcda *.gcov

fclean: clean

re: fclean all

.PHONY: all clean fclean re tests_run
