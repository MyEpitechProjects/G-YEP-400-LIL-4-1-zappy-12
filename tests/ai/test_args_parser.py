##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_args_parser.py
## File description:
## Unit tests for ai/args_parser.py
##

import io
import unittest

from contextlib import redirect_stderr, redirect_stdout
from ai.args_parser import parse_arguments

class TestParseArguments(unittest.TestCase):

    def assert_exits_with(self, argv, code):
        """parse_arguments(argv) must raise SystemExit with the given code (output silenced)."""

        buffer = io.StringIO()
        with self.assertRaises(SystemExit) as cm, redirect_stderr(buffer), redirect_stdout(buffer):
            parse_arguments(argv)
        self.assertEqual(cm.exception.code, code)

    def test_full_arguments_are_parsed(self):
        """-p, -n and -h all provided -> (port, name, machine)."""

        self.assertEqual(parse_arguments(["./zappy_ai", "-p", "4242", "-n", "cerise", "-h", "localhost"]), (4242, "cerise", "localhost"))

    def test_help_exits_zero(self):
        """--help prints usage and exits cleanly (code 0)."""

        self.assert_exits_with(["./zappy_ai", "--help"], 0)

    def test_no_arguments_exits_84(self):
        """No argument at all is an error (code 84)."""

        self.assert_exits_with(["./zappy_ai"], 84)

    def test_unknown_arguments_exit_84(self):
        """An argument layout that matches no valid form is an error."""

        self.assert_exits_with(["./zappy_ai", "-x", "foo"], 84)

    def test_invalid_port_short_form_exits_84(self):
        """A non-integer port in the short form is rejected."""
        self.assert_exits_with(["./zappy_ai", "-p", "abc", "-n", "cerise"], 84)

    def test_invalid_port_full_form_exits_84(self):
        """A non-integer port in the full form is rejected."""

        self.assert_exits_with(["./zappy_ai", "-p", "abc", "-n", "cerise", "-h", "localhost"], 84)

    def test_graphic_team_name_is_rejected(self):
        """GRAPHIC is reserved for the GUI and cannot be used as a team name."""

        self.assert_exits_with(["./zappy_ai", "-p", "4242", "-n", "GRAPHIC"], 84)

if __name__ == "__main__":
    unittest.main()
