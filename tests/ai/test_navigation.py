##
## EPITECH PROJECT, 2026
## G-YEP-400 : tests/ai/test_navigation.py
## File description:
## Unit tests for ai/navigation.py
##

import unittest

from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound, turn_right, turn_left, step_forward, turns_to_face, navigate_to

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
        self.assertEqual(build_move_plan(0), [])

    def test_straight_ahead_no_offset(self):
        self.assertEqual(build_move_plan(2), ["Forward"])
        self.assertEqual(build_move_plan(6), ["Forward", "Forward"])

    def test_left_of_center(self):
        self.assertEqual(build_move_plan(1), ["Left", "Forward", "Right", "Forward"])

    def test_right_of_center(self):
        self.assertEqual(build_move_plan(3), ["Right", "Forward", "Left", "Forward"])

    def test_far_left_offset(self):
        self.assertEqual(build_move_plan(4), ["Left", "Forward", "Forward", "Right", "Forward", "Forward"])

class TestMovesTowardSound(unittest.TestCase):

    def test_same_tile_returns_nothing(self):
        self.assertEqual(moves_toward_sound(0), [])

    def test_front(self):
        self.assertEqual(moves_toward_sound(1), ["Forward"])

    def test_left_side(self):
        self.assertEqual(moves_toward_sound(3), ["Left", "Forward"])

    def test_behind(self):
        self.assertEqual(moves_toward_sound(5), ["Left", "Left", "Forward"])

    def test_right_side(self):
        self.assertEqual(moves_toward_sound(7), ["Right", "Forward"])

class TestTurns(unittest.TestCase):

    def test_turn_right_cycle(self):
        self.assertEqual([turn_right(o) for o in range(4)], [1, 2, 3, 0])

    def test_turn_left_cycle(self):
        self.assertEqual([turn_left(o) for o in range(4)], [3, 0, 1, 2])

    def test_four_rights_is_identity(self):
        o = 2
        for _ in range(4):
            o = turn_right(o)
        self.assertEqual(o, 2)

    def test_right_then_left_is_identity(self):
        self.assertEqual(turn_left(turn_right(1)), 1)

class TestTurnsToFace(unittest.TestCase):

    def test_same_orientation(self):
        self.assertEqual(turns_to_face(0, 0), [])

    def test_quarter_right(self):
        self.assertEqual(turns_to_face(0, 1), ["Right"])

    def test_half_turn(self):
        self.assertEqual(turns_to_face(0, 2), ["Right", "Right"])

    def test_quarter_left(self):
        self.assertEqual(turns_to_face(0, 3), ["Left"])

    def test_wraps_modulo(self):
        self.assertEqual(turns_to_face(3, 0), ["Right"])

class TestStepForward(unittest.TestCase):

    def test_north_wraps(self):
        self.assertEqual(step_forward(0, 0, 0, 10, 10), (0, 9))

    def test_east(self):
        self.assertEqual(step_forward(0, 0, 1, 10, 10), (1, 0))

    def test_south(self):
        self.assertEqual(step_forward(0, 0, 2, 10, 10), (0, 1))

    def test_west_wraps(self):
        self.assertEqual(step_forward(0, 0, 3, 10, 10), (9, 0))

    def test_square_returns_to_origin(self):
        x, y, o = 0, 0, 0
        for _ in range(4):
            x, y = step_forward(x, y, o, 10, 10)
            o = turn_right(o)
        self.assertEqual((x, y, o), (0, 0, 0))

class TestNavigateTo(unittest.TestCase):

    def test_already_there(self):
        self.assertEqual(navigate_to(5, 5, 0, 5, 5, 10, 10), [])

    def test_east_short_path(self):
        self.assertEqual(navigate_to(0, 0, 0, 2, 0, 10, 10), ["Right", "Forward"])

    def test_west_when_wrap_is_shorter(self):
        self.assertEqual(navigate_to(0, 0, 0, 8, 0, 10, 10), ["Left", "Forward"])

    def test_south_short_path(self):
        self.assertEqual(navigate_to(0, 0, 0, 0, 3, 10, 10), ["Right", "Right", "Forward"])

    def test_north_when_wrap_is_shorter(self):
        self.assertEqual(navigate_to(0, 0, 0, 0, 7, 10, 10), ["Forward"])

if __name__ == "__main__":
    unittest.main()
    