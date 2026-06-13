##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_navigation.py
## File description:
## Unit tests for ai/navigation.py
##

import unittest

from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound

class TestDistanceFromTileIndex(unittest.TestCase):

    def test_known_indices(self):
        cases = {0: 0, 1: 1, 2: 1, 3: 1, 4: 2, 8: 2, 9: 3, 15: 3}
        for tile_index, expected in cases.items():
            self.assertEqual(distance_from_tile_index(tile_index), expected, tile_index)

    def test_row_invariant(self):
        for tile_index in range(0, 25):
            r = distance_from_tile_index(tile_index)
            self.assertTrue(r * r <= tile_index < (r + 1) * (r + 1), tile_index)

class TestBuildMovePlan(unittest.TestCase):

    def test_current_tile_is_empty(self):
        """Tile 0 is the player's own tile: no movement needed."""

        self.assertEqual(build_move_plan(0), [])

    def test_straight_ahead_no_offset(self):
        """Tiles centered in their row only require moving forward."""

        self.assertEqual(build_move_plan(2), ["Forward"])
        self.assertEqual(build_move_plan(6), ["Forward", "Forward"])

    def test_left_of_center(self):
        """A tile left of the row center turns left, steps across, then advances."""

        self.assertEqual(build_move_plan(1), ["Left", "Forward", "Right", "Forward"])

    def test_right_of_center(self):
        """A tile right of the row center turns right, steps across, then advances."""

        self.assertEqual(build_move_plan(3), ["Right", "Forward", "Left", "Forward"])

    def test_far_left_offset(self):
        """A larger left offset crosses several tiles before advancing the row."""

        self.assertEqual(build_move_plan(4), ["Left", "Forward", "Forward", "Right", "Forward", "Forward"])

class TestMovesTowardSound(unittest.TestCase):

    def test_same_tile_returns_nothing(self):
        """Direction 0 means the sound is on the player's tile: no move."""
        
        self.assertEqual(moves_toward_sound(0), [])

    def test_front(self):
        self.assertEqual(moves_toward_sound(1), ["Forward"])

    def test_left_side(self):
        self.assertEqual(moves_toward_sound(3), ["Left", "Forward"])

    def test_behind(self):
        self.assertEqual(moves_toward_sound(5), ["Left", "Left", "Forward"])

    def test_right_side(self):
        self.assertEqual(moves_toward_sound(7), ["Right", "Forward"])

if __name__ == "__main__":
    unittest.main()
