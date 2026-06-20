##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/protocol.py
## File description:
## Decode raw server responses into structured data
##

def is_async_message(response):
    """Unsolicited line: does not correspond to ANY order sent."""

    return response.startswith("message ") or response.startswith("eject:")

def is_incantation_event(line):
    return line == "Elevation underway" or line.startswith("Current level:")

def parse_broadcast(line):
    """Parse 'message K, text' into (direction, text). None if malformed."""

    if not line.startswith("message "):
        return None
    body = line[len("message "):]
    if "," not in body:
        return None
    direction_str, text = body.split(",", 1)
    direction_str = direction_str.strip()
    text = text.strip()
    if not direction_str.isdigit():
        return None
    return int(direction_str), text

def parse_inventory(response):
    """Parse an inventory response into a resource dictionary."""

    inventory = {}
    if response == "dead":
        return None
    response = response[1:-1]
    items = response.split(",")
    for item in items:
        item = item.strip()
        parts = item.split()
        if len(parts) != 2 or not parts[1].isdigit():
            continue
        resource_name = parts[0]
        quantity = int(parts[1])
        inventory[resource_name] = quantity
    return inventory

def parse_look(response):
    """Parse a look response into a list of visible tiles."""

    if response == "dead":
        return None
    list_tiles = []
    response = response[1:-1]
    raw_tiles = response.split(",")
    for raw_tile in raw_tiles:
        tile = {}
        raw_tile = raw_tile.strip()
        if raw_tile == "":
            list_tiles.append(tile)
            continue
        objects = raw_tile.split()
        for object_name in objects:
            if object_name not in tile:
                tile[object_name] = 0
            tile[object_name] += 1
        list_tiles.append(tile)
    return list_tiles

def build_heartbeat(team_name, token, level, king_flag, state, sequence):
    """Build the universal coordination heartbeat message."""

    return (f"Z:{team_name}:{token}:{level}:"f"{king_flag}:{state}:{sequence}")

def parse_heartbeat(text):
    """Parse a heartbeat into structured coordination data."""

    parts = text.split(":")
    if len(parts) != 7:
        return None
    if parts[0] != "Z":
        return None
    if not parts[2].isdigit():
        return None
    if not parts[3].isdigit():
        return None
    if not parts[4].isdigit():
        return None
    if not parts[6].isdigit():
        return None
    king_flag = int(parts[4])
    if king_flag not in (0, 1):
        return None
    if parts[5] == "":
        return None
    return parts[1], int(parts[2]), int(parts[3]), king_flag, parts[5], int(parts[6])
