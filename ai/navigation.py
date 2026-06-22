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

    # Vision is a triangle of rows; row r spans indices r^2 .. (r+1)^2 - 1.
    distance = 0
    while (distance + 1) * (distance + 1) <= tile_index:
        distance += 1
    return distance

def build_move_plan(tile_index):
    """Build a movement plan to reach a visible tile index."""

    plan = []
    if tile_index == 0:
        return plan
    row = distance_from_tile_index(tile_index)  # how many tiles forward
    row_start = row * row
    row_center = row_start + row
    offset = tile_index - row_center  # <0 = to the left, >0 = to the right
    if offset < 0:
         # Step left, walk |offset| tiles sideways, face forward again.
        plan.append("Left")
        for _ in range(abs(offset)): 
            plan.append("Forward")
        plan.append("Right")
    elif offset > 0:
        plan.append("Right")
        for _ in range(offset):
            plan.append("Forward")
        plan.append("Left")
    for _ in range(row): # then advance the 'row' tiles forward
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
    # The map is a donut : % width/height wraps an edge exit back to the far side.
    return (x + dx) % width, (y + dy) % height

def turns_to_face(current, target):
    """Minimal list of 'Right'/'Left' to rotate from current orientation to target."""

    diff = (target - current) % 4
    if diff == 0:
        return []
    if diff == 1:
        return ["Right"]
    if diff == 2:
        return ["Right", "Right"] # half turn
    return ["Left"]

def navigate_to(x, y, orientation, target_x, target_y, width, height):
    """One step along the toroidal shortest path toward a target tile."""

    # Forward wrap distances: dx going East, (width - dx) going West (same for y).
    dx = (target_x - x) % width
    dy = (target_y - y) % height
    if dx != 0:
        # Pick the cheaper horizontal way around the torus: East(1) vs West(3).
        target_orientation = 1 if dx <= width - dx else 3
        return turns_to_face(orientation, target_orientation) + ["Forward"]
    if dy != 0:
        # Same idea vertically: South(2) vs North(0).
        target_orientation = 2 if dy <= height - dy else 0
        return turns_to_face(orientation, target_orientation) + ["Forward"]
    return []
