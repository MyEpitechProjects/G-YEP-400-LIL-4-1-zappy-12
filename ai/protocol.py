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

    if not line.startswith("message "): # drop the leading "message " prefix
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
    response = response[1:-1] # strip the surrounding "[ ... ]" brackets
    items = response.split(",")
    for item in items:
        item = item.strip()
        parts = item.split() # "food 345" -> ["food", "345"]
        if len(parts) != 2 or not parts[1].isdigit():
            continue # skip anything that isn't "name number"
        resource_name = parts[0]
        quantity = int(parts[1])
        inventory[resource_name] = quantity
    return inventory

def parse_look(response):
    """Parse a look response into a list of visible tiles."""

    if response == "dead":
        return None
    list_tiles = []
    response = response[1:-1] # strip the "[ ... ]" brackets
    raw_tiles = response.split(",") # tiles are comma-separated
    for raw_tile in raw_tiles:
        tile = {}
        raw_tile = raw_tile.strip()
        if raw_tile == "": # empty tile -> empty dict
            list_tiles.append(tile)
            continue
        objects = raw_tile.split() # space-separated objects on the tile
        for object_name in objects:
            # Count duplicates: "player player food" -> {"player":2, "food":1}
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
    if len(parts) != 7: # must have exactly the 7 expected fields
        return None
    if parts[0] != "Z": # not one of our beacons
        return None
    if not parts[2].isdigit(): # token
        return None
    if not parts[3].isdigit(): # level
        return None
    if not parts[4].isdigit(): # king_flag
        return None
    if not parts[6].isdigit(): # sequence
        return None
    king_flag = int(parts[4]) 
    if king_flag not in (0, 1): # boolean flag, reject anything else
        return None
    if parts[5] == "": # state label must be non-empty
        return None
    return parts[1], int(parts[2]), int(parts[3]), king_flag, parts[5], int(parts[6])
