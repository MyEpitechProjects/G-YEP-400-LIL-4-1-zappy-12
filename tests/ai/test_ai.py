#!/usr/bin/env python3

##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_ai.py
## File description
## Test about ai part
##

import unittest
from ai.brain import (parse_inventory, parse_look)

class TestInventoryParser(unittest.TestCase):

    def test_parse_inventory_valid_response(self):
        response = "[food 10, linemate 1, deraumere 0, sibur 2, mendiane 0, phiras 3, thystame 0]"
        expected = {"food": 10, "linemate": 1, "deraumere": 0, "sibur": 2, "mendiane": 0, "phiras": 3, "thystame": 0}
        self.assertEqual(parse_inventory(response), expected)

    def test_parse_inventory_dead_response(self):
        self.assertIsNone(parse_inventory("dead"))

    def test_parse_inventory_with_extra_spaces(self):
        response = "[ food 10,  linemate 1, deraumere 0 ]"
        expected = {"food": 10, "linemate": 1, "deraumere": 0}
        self.assertEqual(parse_inventory(response), expected)

    def test_parse_inventory_ignores_invalid_parts(self):
        response = "[food 10, invalid, linemate 1]"
        expected = {"food": 10, "linemate": 1}
        self.assertEqual(parse_inventory(response), expected)

class TestLookParser(unittest.TestCase):

    def test_parse_look_valid_response(self):
        response = "[player food, food linemate,,deraumere sibur]"
        expected = [{"player": 1, "food": 1}, {"food": 1, "linemate": 1}, {}, {"deraumere": 1, "sibur": 1}]
        self.assertEqual(parse_look(response), expected)

    def test_parse_look_dead_response(self):
        self.assertIsNone(parse_look("dead"))

    def test_parse_look_empty_tiles(self):
        self.assertEqual(parse_look("[,,,]"), [{}, {}, {}, {}])

    def test_parse_look_counts_multiple_same_objects(self):
        response = "[player food food linemate, sibur sibur sibur]"
        expected = [{"player": 1, "food": 2, "linemate": 1}, {"sibur": 3}]
        self.assertEqual(parse_look(response), expected)

    def test_parse_look_with_extra_spaces(self):
        response = "[ player   food ,  food   linemate , , deraumere  sibur ]"
        expected = [{"player": 1, "food": 1}, {"food": 1, "linemate": 1}, {}, {"deraumere": 1, "sibur": 1}]
        self.assertEqual(parse_look(response), expected)

if __name__ == "__main__":
    unittest.main()
