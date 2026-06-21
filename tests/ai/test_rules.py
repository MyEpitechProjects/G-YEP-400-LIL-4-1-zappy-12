##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_rules.py
## File description:
## Unit tests for ai/rules.py
##

import unittest

from ai.rules import has_required_stones_in_inventory, missing_stone_visible, enough_players_present, can_start_incantation

def inv(**stones):
    """Build an inventory dict with the given stones (others default to 0)."""

    base = {"food": 50, "linemate": 0, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0}
    base.update(stones)
    return base

class TestHasRequiredStonesInInventory(unittest.TestCase):

    def test_level1_complete(self):
        """1->2 needs exactly one linemate (tolerance 0)."""

        self.assertTrue(has_required_stones_in_inventory(1, inv(linemate=1)))

    def test_level1_incomplete(self):
        self.assertFalse(has_required_stones_in_inventory(1, inv()))

    def test_mid_level_tolerance(self):
        """3->4 needs linemate2/sibur1/phiras2; tolerance is 2 missing for this tier."""

        self.assertTrue(has_required_stones_in_inventory(3, inv(linemate=2, sibur=1)))
        self.assertFalse(has_required_stones_in_inventory(3, inv()))

    def test_top_level_has_no_next(self):
        self.assertFalse(has_required_stones_in_inventory(8, inv(linemate=9)))

    def test_high_tier_tolerance_5(self):
        """6->7 tolerates up to 5 missing stones."""
        self.assertTrue(has_required_stones_in_inventory(6, inv(sibur=3))) 
        self.assertFalse(has_required_stones_in_inventory(6, inv()))

    def test_max_tier_tolerance_8(self):
        """7->8 tolerates up to 8 missing stones."""
        self.assertTrue(has_required_stones_in_inventory(7, inv(mendiane=2, phiras=2)))
        self.assertFalse(has_required_stones_in_inventory(7, inv()))

class TestMissingStoneVisible(unittest.TestCase):

    def test_visible_missing_stone(self):
        self.assertTrue(missing_stone_visible(1, inv(), [{}, {"linemate": 1}]))

    def test_not_visible(self):
        self.assertFalse(missing_stone_visible(1, inv(), [{}, {"food": 1}]))

    def test_already_owned_is_not_missing(self):
        self.assertFalse(missing_stone_visible(1, inv(linemate=1), [{"linemate": 1}]))

    def test_top_level_has_no_next(self):
        self.assertFalse(missing_stone_visible(8, inv(), [{"linemate": 1}]))

class TestEnoughPlayersPresent(unittest.TestCase):

    def test_enough(self):
        self.assertTrue(enough_players_present(1, [{"player": 1}]))

    def test_not_enough(self):
        self.assertFalse(enough_players_present(2, [{"player": 1}]))

    def test_empty_vision(self):
        self.assertFalse(enough_players_present(1, []))

    def test_top_level_has_no_next(self):
        self.assertFalse(enough_players_present(8, [{"player": 9}]))

class TestCanStartIncantation(unittest.TestCase):

    def test_ready(self):
        self.assertTrue(can_start_incantation(1, inv(), [{"player": 1, "linemate": 1}]))

    def test_missing_stone_on_tile(self):
        self.assertFalse(can_start_incantation(1, inv(), [{"player": 1}]))

    def test_missing_player_on_tile(self):
        self.assertFalse(can_start_incantation(1, inv(), [{"linemate": 1}]))

    def test_empty_vision(self):
        self.assertFalse(can_start_incantation(1, inv(), []))

    def test_top_level_has_no_next(self):
        self.assertFalse(can_start_incantation(8, inv(), [{"player": 9}]))

if __name__ == "__main__":
    unittest.main()