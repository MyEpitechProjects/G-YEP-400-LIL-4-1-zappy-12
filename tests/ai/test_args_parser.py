##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_args_parser.py
## File description:
## Unit tests for ai/args_parser.py
##

import unittest

from ai.args_parser import parse_arguments

class TestParseArguments(unittest.TestCase):

    def test_full_arguments_are_parsed(self):
        """-p, -n and -h all provided -> (port, name, machine)."""
        port, name, machine = parse_arguments(["./zappy_ai", "-p", "4242", "-n", "cerise", "-h", "localhost"])
        self.assertEqual(port, 4242)
        self.assertEqual(name, "cerise")
        self.assertEqual(machine, "localhost")

if __name__ == "__main__":
    unittest.main()
