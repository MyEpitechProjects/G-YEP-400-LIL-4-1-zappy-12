##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/rules.py
## File description:
## Incantation prerequisites (stones / players present)
##

from ai.constants import REQUIREMENTS, STONES

def has_required_stones_in_inventory(level, inventory):
    """Check if inventory contains enough stones to trigger a gather (pooling)."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS: # already at level 8
        return False
    requirements = REQUIREMENTS[level_to_reach]
    missing = 0
    for stone in STONES:
        if inventory.get(stone, 0) < requirements.get(stone, 0):
            missing += requirements.get(stone, 0) - inventory.get(stone, 0)
    if level_to_reach <= 3:
        return missing == 0
    elif level_to_reach <= 5:
        return missing <= 2
    elif level_to_reach <= 7:
        return missing <= 5
    else:
        return missing <= 8

def missing_stone_visible(level, inventory, vision):
    """Check if a missing stone is visible in current vision."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        current_amount = inventory.get(stone, 0)
        missing_amount = required_amount - current_amount
        if missing_amount <= 0:
            continue
        for tile in vision:
            if tile.get(stone, 0) > 0:
                return True
    return False

def enough_players_present(level, vision):
    """True if current tile has enough players for the next incantation."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    if len(vision) == 0:
        return False
    return vision[0].get("player", 0) >= REQUIREMENTS[level_to_reach].get("players", 0)

def can_start_incantation(level, inventory, vision):
    """Check if current tile meets incantation requirements."""

    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return False
    if len(vision) == 0:
        return False
    requirements = REQUIREMENTS[level_to_reach]
    current_tile = vision[0]
    if current_tile.get("player", 0) < requirements.get("players", 0):
        return False
    for stone in STONES: # every required stone must physically be on the tile
        if current_tile.get(stone, 0) < requirements.get(stone, 0):
            return False
    return True
