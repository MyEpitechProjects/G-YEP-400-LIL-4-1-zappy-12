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
TESTS_DIR_SERVER = tests/server

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
	$(MAKE) -C $(TESTS_DIR_SERVER) tests_run

clean:
	$(MAKE) -C server clean
	$(MAKE) -C $(TESTS_DIR_SERVER) clean
	rm -f .coverage
	rm -rf htmlcov
	rm -rf *.css
	rm -rf *.html
	rm -rf coverage_report
	rm -rf *.gcda
	rm -rf *.gcno
	rm -rf *.gcov
	find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	find . -type f \( -name "*.pyc" -o -name "*.pyo" \) -delete
	find . \( -name '*.gcda' -o -name '*.gcno' -o -name '*.gcov' \) -delete

fclean: clean
	$(MAKE) -C server fclean
	rm -f $(NAME_AI)

re: fclean all

.PHONY: all tests_run clean fclean re
