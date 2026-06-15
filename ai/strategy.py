##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/strategy.py
## File description:
## Decision making: state machine, tile scoring and plan construction
##

from ai.constants import FOOD_LOW_LIMIT, SURVIVE, INCANT, PREPARE_RITUAL, COLLECT_STONE, EXPLORE, GATHER, FOLLOW_GATHER, WAIT_RITUAL, GATHER_TIMEOUT, REQUIREMENTS, STONES, HIGH_FOOD_PRIORITY, NORMAL_FOOD_PRIORITY, STONE_PRIORITY, DISTANCE_PENALTY, CURRENT_TILE_BONUS, ANCHOR_CONFIRMATION_ROUNDS
from ai.rules import has_required_stones_in_inventory, missing_stone_visible, enough_players_present, can_start_incantation
from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound
from ai.protocol import parse_gather_call
from ai.fork import should_reproduce
from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound, step_forward, turns_to_face

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

def build_wait_ritual_plan(current_tile):
    """Wait for the ritual without taking stones from the current tile."""

    if current_tile.get("food", 0) > 0:
        return ["Take food"]
    return ["Look"]

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

def select_anchor_for_follower(trantorian):
    """Return direction and token of the best valid gathering anchor."""

    best_direction = None
    best_token = -1
    for direction, text in trantorian.heard_calls:
        parsed = parse_gather_call(text)
        if parsed is None:
            continue
        other_team, other_level, other_token = parsed
        if other_team != trantorian.team_name:
            continue
        if other_level != trantorian.level:
            continue
        if other_token == trantorian.gather_token:
            continue
        if other_token > best_token:
            best_direction = direction
            best_token = other_token
    trantorian.heard_calls = []
    if best_direction is None:
        return None
    return best_direction, best_token

def select_best_anchor(trantorian):
    """Return direction and token of a stronger valid anchor."""

    best_anchor = None
    best_token = trantorian.gather_token
    for direction, text in trantorian.heard_calls:
        parsed = parse_gather_call(text)
        if parsed is None:
            continue
        other_team, other_level, other_token = parsed
        if other_team != trantorian.team_name:
            continue
        if other_level != trantorian.level:
            continue
        if other_token > best_token:
            best_token = other_token
            best_anchor = (direction, other_token)
    trantorian.heard_calls = []
    return best_anchor

def owns_anchor_role(trantorian):
    """Check whether this agent owns the elected anchor token"""
    
    return trantorian.is_anchor and trantorian.anchor_token == trantorian.gather_token

def build_follow_gather_plan(trantorian, current_tile):
    """Follow an anchor or wait without taking ritual stones."""

    selected_anchor = select_anchor_for_follower(trantorian)
    if selected_anchor is not None:
        direction, token = selected_anchor
        trantorian.anchor_token = token
        trantorian.is_anchor = False
        trantorian.gather_silence = 0
        if direction == 0:
            trantorian.state = WAIT_RITUAL
            return build_wait_ritual_plan(current_tile)
        trantorian.state = FOLLOW_GATHER
        return moves_toward_sound(direction) + ["Look"]
    if trantorian.state in (FOLLOW_GATHER, WAIT_RITUAL):
        trantorian.gather_silence += 1
        if trantorian.gather_silence >= GATHER_TIMEOUT:
            trantorian.reset_gather_state()
            return None
        if trantorian.state == WAIT_RITUAL:
            return build_wait_ritual_plan(current_tile)
        return ["Look"]
    return None

def build_gather_plan(trantorian):
    """Elect one anchor and make weaker candidates follow it."""

    selected_anchor = select_best_anchor(trantorian)
    if selected_anchor is not None:
        direction, token = selected_anchor
        trantorian.is_anchor = False
        trantorian.anchor_token = token
        trantorian.anchor_confirmations = 0
        if direction == 0:
            trantorian.state = WAIT_RITUAL
            return ["Look"]
        trantorian.state = FOLLOW_GATHER
        return moves_toward_sound(direction) + ["Look"]
    trantorian.anchor_token = trantorian.gather_token
    trantorian.anchor_confirmations += 1
    trantorian.is_anchor = (trantorian.anchor_confirmations >= ANCHOR_CONFIRMATION_ROUNDS)
    trantorian.state = GATHER
    call = (f"{trantorian.team_name}:"f"{trantorian.level}:"f"{trantorian.gather_token}")
    return ["Broadcast " + call, "Look"]

def explore_plan(trantorian):
    """Frontier exploration: head toward the least-visited neighbouring tile."""
    x, y, o = trantorian.x, trantorian.y, trantorian.orientation
    w, h = trantorian.map_width, trantorian.map_height
    trantorian.visited[(x, y)] = trantorian.visited.get((x, y), 0) + 1
    best_orientation = o
    best_score = None
    for target_o in range(4):
        nx, ny = step_forward(x, y, target_o, w, h)
        turn_cost = (target_o - o) % 4
        if turn_cost == 3:
            turn_cost = 1
        score = (trantorian.visited.get((nx, ny), 0), turn_cost)
        if best_score is None or score < best_score:
            best_score = score
            best_orientation = target_o
    return turns_to_face(o, best_orientation) + ["Forward", "Look"]

def build_plan(trantorian):
    """Build the next action plan from the current AI state."""

    inventory = trantorian.inventory
    vision = trantorian.vision
    level = trantorian.level
    if not inventory:
        return ["Inventory"]
    if not vision:
        return ["Look"]
    current_tile = vision[0]
    food = inventory.get("food", 0)
    has_all_stones = has_required_stones_in_inventory(level, inventory)
    if trantorian.state in (FOLLOW_GATHER, WAIT_RITUAL):
        follow_plan = build_follow_gather_plan(trantorian, current_tile)
        if follow_plan is not None:
            return follow_plan
    if food > FOOD_LOW_LIMIT and not has_all_stones:
        follow_plan = build_follow_gather_plan(trantorian, current_tile)
        if follow_plan is not None:
            return follow_plan
    if food > FOOD_LOW_LIMIT and has_all_stones and not owns_anchor_role(trantorian):
        return build_gather_plan(trantorian)
    state = choose_state(inventory, vision, level)
    trantorian.state = state
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
        return explore_plan(trantorian) 
    if state == INCANT:
        if not owns_anchor_role(trantorian):
            return ["Look"]
        required = REQUIREMENTS.get(level + 1, {}).get("players", 0)
        if trantorian.last_incant_failed and current_tile.get("player", 0) > required:
            trantorian.last_incant_failed = False
            return ["Eject", "Look"]
        return ["Incantation"]
    if state == PREPARE_RITUAL:
        if not owns_anchor_role(trantorian):
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
        return explore_plan(trantorian)
    if state == EXPLORE:
        return explore_plan(trantorian)
    if state == GATHER:
        return build_gather_plan(trantorian)
    if state == WAIT_RITUAL:
        return build_wait_ritual_plan(current_tile)
    return ["Look"]
