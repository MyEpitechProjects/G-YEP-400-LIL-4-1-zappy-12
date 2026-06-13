##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_protocol.py
## File description:
## Unit tests for ai/protocol.py
##

import unittest

from ai.protocol import is_async_message, is_incantation_event, parse_broadcast, parse_inventory, parse_look, parse_gather_call

class TestIsAsyncMessage(unittest.TestCase):

    def test_broadcast_is_async(self):
        self.assertTrue(is_async_message("message 3, hello"))

    def test_eject_is_async(self):
        self.assertTrue(is_async_message("eject: 5"))

    def test_regular_response_is_not_async(self):
        self.assertFalse(is_async_message("ok"))
        self.assertFalse(is_async_message("[ food 9 ]"))

class TestIsIncantationEvent(unittest.TestCase):

    def test_elevation_underway(self):
        self.assertTrue(is_incantation_event("Elevation underway"))

    def test_current_level(self):
        self.assertTrue(is_incantation_event("Current level: 2"))

    def test_other_line(self):
        self.assertFalse(is_incantation_event("ok"))

class TestParseBroadcast(unittest.TestCase):

    def test_simple_message(self):
        self.assertEqual(parse_broadcast("message 3, hello"), (3, "hello"))

    def test_gather_token_keeps_colons(self):
        """Only the first comma is the separator, so the colon-token survives intact."""

        self.assertEqual(parse_broadcast("message 8, cerise:2:734521"), (8, "cerise:2:734521"))

    def test_direction_zero(self):
        self.assertEqual(parse_broadcast("message 0, hi"), (0, "hi"))

    def test_not_a_message(self):
        self.assertIsNone(parse_broadcast("ok"))

    def test_no_comma(self):
        self.assertIsNone(parse_broadcast("message 3 hello"))

    def test_non_digit_direction(self):
        self.assertIsNone(parse_broadcast("message abc, hello"))

class TestParseInventory(unittest.TestCase):

    def test_dead(self):
        self.assertIsNone(parse_inventory("dead"))

    def test_normal(self):
        result = parse_inventory("[ food 9, linemate 0, sibur 3 ]")
        self.assertEqual(result, {"food": 9, "linemate": 0, "sibur": 3})

    def test_malformed_items_are_skipped(self):
        """A token with no quantity, or a non-numeric quantity, is ignored."""

        result = parse_inventory("[ broken, food x, sibur 3 ]")
        self.assertEqual(result, {"sibur": 3})

class TestParseLook(unittest.TestCase):

    def test_dead(self):
        self.assertIsNone(parse_look("dead"))

    def test_empty_and_filled_tiles(self):
        result = parse_look("[ player,, food ]")
        self.assertEqual(result, [{"player": 1}, {}, {"food": 1}])

    def test_multiple_objects_are_counted(self):
        result = parse_look("[ player food, food food ]")
        self.assertEqual(result, [{"player": 1, "food": 1}, {"food": 2}])

class TestParseGatherCall(unittest.TestCase):

    def test_valid(self):
        self.assertEqual(parse_gather_call("cerise:2:734521"), ("cerise", 2, 734521))

    def test_wrong_part_count(self):
        self.assertIsNone(parse_gather_call("cerise:2"))

    def test_non_digit_level(self):
        self.assertIsNone(parse_gather_call("cerise:x:734521"))

    def test_non_digit_token(self):
        self.assertIsNone(parse_gather_call("cerise:2:abc"))

if __name__ == "__main__":
    unittest.main()
