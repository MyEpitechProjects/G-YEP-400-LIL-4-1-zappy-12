##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/navigation.py
## File description:
## Geometry helpers: tile index -> moves, sound direction -> moves
##

# Orientation as an index: 0=N, 1=E, 2=S, 3=W. Deltas applied on a Forward.
FORWARD_DELTA = [(0, -1), (1, 0), (0, 1), (-1, 0)]

def distance_from_tile_index(tile_index):
    """Estimate distance between player and a visible tile index."""

    distance = 0
    while (distance + 1) * (distance + 1) <= tile_index:
        distance += 1
    return distance

def build_move_plan(tile_index):
    """Build a movement plan to reach a visible tile index."""

    plan = []
    if tile_index == 0:
        return plan
    row = distance_from_tile_index(tile_index)
    row_start = row * row
    row_center = row_start + row
    offset = tile_index - row_center
    if offset < 0:
        plan.append("Left")
        for _ in range(abs(offset)):
            plan.append("Forward")
        plan.append("Right")
    elif offset > 0:
        plan.append("Right")
        for _ in range(offset):
            plan.append("Forward")
        plan.append("Left")
    for _ in range(row):
        plan.append("Forward")
    return plan

def moves_toward_sound(direction):
    """SOUND direction (0-8) -> turn towards the source then move forward."""

    if direction == 1:
        return ["Forward"]
    if direction in (2, 3, 4):
        return ["Left", "Forward"]
    if direction == 5:
        return ["Left", "Left", "Forward"]
    if direction in (6, 7, 8):
        return ["Right", "Forward"]
    return []

def turn_right(orientation):
    """90 deg clockwise: N -> E -> S -> W -> N."""
    return (orientation + 1) % 4

def turn_left(orientation):
    """90 deg counter-clockwise: N -> W -> S -> E -> N."""
    return (orientation - 1) % 4

def step_forward(x, y, orientation, width, height):
    """Advance one tile in the facing direction, wrapping on the toroidal map."""
    dx, dy = FORWARD_DELTA[orientation]
    return (x + dx) % width, (y + dy) % height
