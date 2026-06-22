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

TESTS_DIR_AI = tests/ai

all:
	$(MAKE) -C server
	$(MAKE) $(NAME_AI)

$(NAME_AI):
	cp ai/main.py $(NAME_AI)
	chmod +x $(NAME_AI)

$(NAME_SERVER):
	$(MAKE) -C server

tests_run:
	PYTHONPATH=. $(PYTHON) -m coverage run --source=ai -m unittest discover -s $(TESTS_DIR_AI) -p "test_*.py" -v

clean:
	$(MAKE) -C server clean
	rm -f .coverage
	rm -rf htmlcov
	find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find . -type f \( -name "*.pyc" -o -name "*.pyo" \) -delete
	
fclean: clean
	$(MAKE) -C server fclean
	rm -f $(NAME_AI)

re: fclean all
