##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_protocol.py
## File description:
## Unit tests for ai/protocol.py
##

import unittest

from ai.protocol import is_async_message, is_incantation_event, parse_broadcast, parse_inventory, parse_look, build_heartbeat, parse_heartbeat

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

    def test_heartbeat_keeps_colons(self):
        """Only the first comma is the separator, so the colon payload survives intact."""

        self.assertEqual(parse_broadcast("message 8, Z:cerise:42:3:1:KING:7"), (8, "Z:cerise:42:3:1:KING:7"))

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
        self.assertEqual(parse_inventory("[ food 9, linemate 0, sibur 3 ]"), {"food": 9, "linemate": 0, "sibur": 3})

    def test_malformed_items_are_skipped(self):
        self.assertEqual(parse_inventory("[ broken, food x, sibur 3 ]"), {"sibur": 3})

class TestParseLook(unittest.TestCase):

    def test_dead(self):
        self.assertIsNone(parse_look("dead"))

    def test_empty_and_filled_tiles(self):
        self.assertEqual(parse_look("[ player,, food ]"), [{"player": 1}, {}, {"food": 1}])

    def test_multiple_objects_are_counted(self):
        self.assertEqual(parse_look("[ player food, food food ]"), [{"player": 1, "food": 1}, {"food": 2}])

class TestHeartbeat(unittest.TestCase):

    def test_build_format(self):
        self.assertEqual(build_heartbeat("cerise", 42, 3, 1, "KING", 7), "Z:cerise:42:3:1:KING:7")

    def test_round_trip(self):
        text = build_heartbeat("cerise", 42, 3, 1, "KING", 7)
        self.assertEqual(parse_heartbeat(text), ("cerise", 42, 3, 1, "KING", 7))

    def test_king_flag_zero(self):
        self.assertEqual(parse_heartbeat("Z:team:5:2:0:RALLY:1"), ("team", 5, 2, 0, "RALLY", 1))

    def test_wrong_part_count(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:2:0:RALLY"))

    def test_wrong_prefix(self):
        self.assertIsNone(parse_heartbeat("X:team:5:2:0:RALLY:1"))

    def test_non_digit_token(self):
        self.assertIsNone(parse_heartbeat("Z:team:x:2:0:RALLY:1"))

    def test_non_digit_level(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:x:0:RALLY:1"))

    def test_non_digit_king_flag(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:2:x:RALLY:1"))

    def test_king_flag_out_of_range(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:2:2:RALLY:1"))

    def test_empty_state(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:2:0::1"))

    def test_non_digit_sequence(self):
        self.assertIsNone(parse_heartbeat("Z:team:5:2:0:RALLY:x"))

if __name__ == "__main__":
    unittest.main()