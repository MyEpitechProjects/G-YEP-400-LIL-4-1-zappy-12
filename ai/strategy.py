##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/strategy.py
## File description:
## Decision making: state machine, tile scoring and plan construction
##

from ai.constants import FOOD_LOW_LIMIT, SURVIVE, INCANT, PREPARE_RITUAL, COLLECT_STONE, EXPLORE, GATHER, REQUIREMENTS, STONES, HIGH_FOOD_PRIORITY, NORMAL_FOOD_PRIORITY, STONE_PRIORITY, DISTANCE_PENALTY, CURRENT_TILE_BONUS
from ai.rules import has_required_stones_in_inventory, missing_stone_visible, enough_players_present, can_start_incantation
from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound
from ai.protocol import parse_gather_call
from ai.fork import should_reproduce

def find_best_tile(vision, inventory, level):
    """Find the most valuable visible tile for the current state."""

    if len(vision) == 0:
        return 0
    best_tile_index = 0
    best_score = -9999
    level_to_reach = level + 1
    requirements = REQUIREMENTS.get(level_to_reach, {})
    food = inventory.get("food", 0)
    for tile_index, tile in enumerate(vision):
        score = 0
        distance = distance_from_tile_index(tile_index)
        if food <= FOOD_LOW_LIMIT:
            score += tile.get("food", 0) * HIGH_FOOD_PRIORITY
        else:
            score += tile.get("food", 0) * NORMAL_FOOD_PRIORITY
        for stone in STONES:
            required_amount = requirements.get(stone, 0)
            current_amount = inventory.get(stone, 0)
            missing_amount = required_amount - current_amount
            if missing_amount > 0:
                score += tile.get(stone, 0) * STONE_PRIORITY
        score -= distance * DISTANCE_PENALTY
        if tile_index == 0:
            score += CURRENT_TILE_BONUS
        if score > best_score:
            best_score = score
            best_tile_index = tile_index
    return best_tile_index

def choose_state(inventory, vision, level):
    """Choose the current AI state from inventory, vision and level."""

    food = inventory.get("food", 0)
    if food <= FOOD_LOW_LIMIT:
        return SURVIVE
    if can_start_incantation(level, inventory, vision):
        return INCANT
    if has_required_stones_in_inventory(level, inventory):
        if enough_players_present(level, vision):
            return PREPARE_RITUAL
        return GATHER
    if missing_stone_visible(level, inventory, vision):
        return COLLECT_STONE
    return EXPLORE

def build_take_plan(tile, inventory, level):
    """Build a plan to take useful resources from the current tile."""

    plan = []
    if len(tile) == 0:
        return plan
    for _ in range(tile.get("food", 0)):
        plan.append("Take food")
    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return plan
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        current_amount = inventory.get(stone, 0)
        missing_amount = required_amount - current_amount
        if missing_amount <= 0:
            continue
        available_amount = tile.get(stone, 0)
        while available_amount > 0 and missing_amount > 0:
            plan.append("Take " + stone)
            available_amount -= 1
            missing_amount -= 1
    return plan

def build_prepare_ritual_plan(level, inventory, current_tile):
    """Build a plan to set required stones down before an incantation."""

    plan = []
    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:
        return plan
    requirements = REQUIREMENTS[level_to_reach]
    for stone in STONES:
        required_amount = requirements.get(stone, 0)
        amount_on_tile = current_tile.get(stone, 0)
        amount_in_inventory = inventory.get(stone, 0)
        missing_on_tile = required_amount - amount_on_tile
        if missing_on_tile <= 0:
            continue
        while missing_on_tile > 0 and amount_in_inventory > 0:
            plan.append("Set " + stone)
            missing_on_tile -= 1
            amount_in_inventory -= 1
    plan.append("Look")
    return plan

def select_best_anchor(trantorian):
    best_direction = None
    best_token = trantorian.gather_token
    for direction, text in trantorian.heard_calls:
        parsed = parse_gather_call(text)
        if parsed is None:
            continue
        other_team, other_level, other_token = parsed
        if (other_team == trantorian.team_name
                and other_level == trantorian.level
                and direction != 0
                and other_token > best_token):
            best_token = other_token
            best_direction = direction
    trantorian.heard_calls = []
    return best_direction

def build_gather_plan(trantorian):
    """Make a call and stay fixed (anchor), or converge towards a stronger anchor."""

    direction = select_best_anchor(trantorian)
    if direction is not None:
        trantorian.is_anchor = False
        return moves_toward_sound(direction) + ["Look"]
    trantorian.is_anchor = True
    call = f"{trantorian.team_name}:{trantorian.level}:{trantorian.gather_token}"
    return ["Broadcast " + call, "Look"]

def build_plan(trantorian):
    """Build the next action plan from the current AI state."""

    inventory = trantorian.inventory
    vision = trantorian.vision
    level = trantorian.level
    if not inventory:
        return ["Inventory"]
    if not vision:
        return ["Look"]
    state = choose_state(inventory, vision, level)
    current_tile = vision[0]
    if state in (EXPLORE, GATHER) and should_reproduce(trantorian):
        return ["Fork"]
    if state == SURVIVE:
        take_plan = build_take_plan(current_tile, inventory, level)
        if take_plan:
            return take_plan
        best_tile = find_best_tile(vision, inventory, level)
        if best_tile != 0:
            plan = build_move_plan(best_tile)
            plan.append("Look")
            return plan
        return ["Forward", "Look"]
    if state == INCANT:
        if trantorian.is_anchor:
            return ["Incantation"]
        return ["Look"]
    if state == PREPARE_RITUAL:
        if not trantorian.is_anchor:
            return ["Look"]
        plan = build_prepare_ritual_plan(level, inventory, current_tile)
        if plan:
            return plan
        return ["Look"]
    if state == COLLECT_STONE:
        take_plan = build_take_plan(current_tile, inventory, level)
        if take_plan:
            return take_plan
        best_tile = find_best_tile(vision, inventory, level)
        if best_tile != 0:
            plan = build_move_plan(best_tile)
            plan.append("Look")
            return plan
        return ["Forward", "Look"]
    if state == EXPLORE:
        return ["Forward", "Look"]
    if state == GATHER:
        return build_gather_plan(trantorian)
    return ["Look"]
