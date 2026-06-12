##
## EPITECH PROJECT, 2026
## G-YEP-400 : Zappy - Makefile
## File description:
## Generic Makefile
##

CC = epiclang
PYTHON = python3

NAME_SERVER = zappy_server
NAME_AI = zappy_ai

CFLAGS = -Wall -Wextra -Werror -I server/include

SRC_SERVER = server/src/main.c

OBJ_SERVER = $(SRC_SERVER:.c=.o)

TESTS_DIR_AI = tests/ai

all: $(NAME_SERVER) $(NAME_AI)

$(NAME_SERVER): $(OBJ_SERVER)
	$(CC) $(OBJ_SERVER) -o $(NAME_SERVER) $(CFLAGS)

$(NAME_AI):
	cp ai/main.py $(NAME_AI)
	chmod +x $(NAME_AI)

tests_run:
	PYTHONPATH=. $(PYTHON) -m coverage run --source=ai -m unittest discover -s $(TESTS_DIR_AI) -p "test_*.py" -v

clean:
	rm -f $(OBJ_SERVER)
	rm -f .coverage
	rm -rf htmlcov
	find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find . -type f \( -name "*.pyc" -o -name "*.pyo" \) -delete
	
fclean: clean
	rm -f $(NAME_SERVER) $(NAME_AI)

re: fclean all
