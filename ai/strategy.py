##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/strategy.py
## File description:
## Decision making: state machine, tile scoring and plan construction
##

from ai.constants import FOOD_LOW_LIMIT, SURVIVE, INCANT, PREPARE_RITUAL, COLLECT_STONE, EXPLORE, GATHER, FOLLOW_GATHER, WAIT_RITUAL, GATHER_TIMEOUT, REQUIREMENTS, STONES, HIGH_FOOD_PRIORITY, NORMAL_FOOD_PRIORITY, STONE_PRIORITY, DISTANCE_PENALTY, CURRENT_TILE_BONUS, ANCHOR_CONFIRMATION_ROUNDS, READY_REPEAT_PERIOD, READY_TIMEOUT
from ai.rules import has_required_stones_in_inventory, missing_stone_visible, enough_players_present, can_start_incantation
from ai.protocol import parse_gather_call, parse_ready_call, build_gather_message, build_ready_message
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

def build_gather_command(trantorian):
    """Build the anchor gathering broadcast command."""

    message = build_gather_message(trantorian.team_name, trantorian.level, trantorian.gather_token)
    return "Broadcast " + message

def build_ready_command(trantorian):
    """Build a READY command targeted at the selected anchor."""

    message = build_ready_message(trantorian.team_name, trantorian.level, trantorian.anchor_token, trantorian.gather_token)
    return "Broadcast " + message

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
    """Check whether this agent owns the elected anchor token."""

    return trantorian.is_anchor and trantorian.anchor_token == trantorian.gather_token

def update_ready_players(trantorian):
    """Register valid READY heartbeats received by the elected anchor."""

    if not owns_anchor_role(trantorian):
        trantorian.ready_calls = []
        trantorian.ready_players = {}
        return
    for direction, text in trantorian.ready_calls:
        parsed = parse_ready_call(text)
        if parsed is None:
            continue
        team, level, anchor_token, player_token = parsed
        if direction != 0:
            continue
        if team != trantorian.team_name:
            continue
        if level != trantorian.level:
            continue
        if anchor_token != trantorian.gather_token:
            continue
        if player_token == trantorian.gather_token:
            continue
        trantorian.ready_players[player_token] = trantorian.coordination_tick
    trantorian.ready_calls = []
    expired_players = []
    for player_token, last_seen in trantorian.ready_players.items():
        age = trantorian.coordination_tick - last_seen
        if age > READY_TIMEOUT:
            expired_players.append(player_token)
    for player_token in expired_players:
        del trantorian.ready_players[player_token]

def has_ready_group(trantorian, current_tile):
    """Check if enough valid same-level players are on the anchor tile."""

    required_players = REQUIREMENTS.get(trantorian.level + 1, {}).get("players", 0)
    ready_players = 1 + len(trantorian.ready_players)
    visible_players = current_tile.get("player", 0)
    return ready_players >= required_players and visible_players >= required_players

def build_wait_ritual_plan(trantorian, current_tile):
    """Wait on the anchor tile and regularly send READY heartbeats."""

    plan = []
    if current_tile.get("food", 0) > 0:
        plan.append("Take food")
    if trantorian.anchor_token is None:
        plan.append("Look")
        return plan
    if trantorian.ready_repeat_counter <= 0:
        plan.append(build_ready_command(trantorian))
        trantorian.ready_repeat_counter = READY_REPEAT_PERIOD - 1
    else:
        trantorian.ready_repeat_counter -= 1
    plan.append("Look")
    return plan

def build_follow_gather_plan(trantorian, current_tile):
    """Follow an anchor and announce readiness on its ritual tile."""

    selected_anchor = select_anchor_for_follower(trantorian)
    if selected_anchor is not None:
        direction, token = selected_anchor
        anchor_changed = trantorian.anchor_token != token
        trantorian.anchor_token = token
        trantorian.is_anchor = False
        trantorian.gather_silence = 0
        trantorian.ready_players = {}
        if direction == 0:
            if anchor_changed or trantorian.state != WAIT_RITUAL:
                trantorian.ready_repeat_counter = 0
            trantorian.state = WAIT_RITUAL
            return build_wait_ritual_plan(trantorian, current_tile)
        trantorian.state = FOLLOW_GATHER
        trantorian.ready_repeat_counter = 0
        return moves_toward_sound(direction) + ["Look"]
    if trantorian.state in (FOLLOW_GATHER, WAIT_RITUAL):
        if trantorian.state == WAIT_RITUAL and current_tile.get("player", 0) > 1:
            trantorian.gather_silence = 0
        else:
            trantorian.gather_silence += 1
        if trantorian.gather_silence >= GATHER_TIMEOUT:
            trantorian.reset_gather_state()
            return None
        if trantorian.state == WAIT_RITUAL:
            return build_wait_ritual_plan(trantorian, current_tile)
        return ["Look"]
    return None

def build_gather_plan(trantorian, current_tile):
    """Elect one anchor and make weaker candidates follow it."""

    selected_anchor = select_best_anchor(trantorian)
    if selected_anchor is not None:
        direction, token = selected_anchor
        trantorian.is_anchor = False
        trantorian.anchor_token = token
        trantorian.anchor_confirmations = 0
        trantorian.ready_players = {}
        trantorian.ready_repeat_counter = 0
        if direction == 0:
            trantorian.state = WAIT_RITUAL
            return build_wait_ritual_plan(trantorian, current_tile)
        trantorian.state = FOLLOW_GATHER
        return moves_toward_sound(direction) + ["Look"]
    trantorian.anchor_token = trantorian.gather_token
    trantorian.anchor_confirmations += 1
    trantorian.is_anchor = (trantorian.anchor_confirmations >= ANCHOR_CONFIRMATION_ROUNDS)
    trantorian.state = GATHER
    return [build_gather_command(trantorian), "Look"]

def explore_plan(trantorian):
    """Frontier exploration: head toward the least-visited neighbouring tile."""

    x, y, orientation = trantorian.x, trantorian.y, trantorian.orientation
    width, height = trantorian.map_width, trantorian.map_height
    trantorian.visited[(x, y)] = trantorian.visited.get((x, y), 0) + 1
    best_orientation = orientation
    best_score = None
    for target_orientation in range(4):
        next_x, next_y = step_forward(x, y, target_orientation, width, height)
        turn_cost = (target_orientation - orientation) % 4
        if turn_cost == 3:
            turn_cost = 1
        score = (trantorian.visited.get((next_x, next_y), 0), turn_cost)
        if best_score is None or score < best_score:
            best_score = score
            best_orientation = target_orientation
    return turns_to_face(orientation, best_orientation) + ["Forward", "Look"]

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
    trantorian.coordination_tick += 1
    update_ready_players(trantorian)
    food = inventory.get("food", 0)
    has_all_stones = has_required_stones_in_inventory(level, inventory)
    if food <= FOOD_LOW_LIMIT:
        if trantorian.state in (GATHER, FOLLOW_GATHER, WAIT_RITUAL) or trantorian.is_anchor:
            trantorian.reset_gather_state()
    else:
        if trantorian.state in (FOLLOW_GATHER, WAIT_RITUAL):
            follow_plan = build_follow_gather_plan(trantorian, current_tile)
            if follow_plan is not None:
                return follow_plan
        if not has_all_stones:
            follow_plan = build_follow_gather_plan(trantorian, current_tile)
            if follow_plan is not None:
                return follow_plan
        if has_all_stones and not owns_anchor_role(trantorian):
            return build_gather_plan(trantorian, current_tile)
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
        if not has_ready_group(trantorian, current_tile):
            return [build_gather_command(trantorian), "Look"]
        if trantorian.last_incant_failed:
            trantorian.last_incant_failed = False
            return [build_gather_command(trantorian), "Look"]
        return ["Incantation"]
    if state == PREPARE_RITUAL:
        if not owns_anchor_role(trantorian):
            return ["Look"]
        if not has_ready_group(trantorian, current_tile):
            return [build_gather_command(trantorian), "Look"]
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
        return build_gather_plan(trantorian, current_tile)
    if state == WAIT_RITUAL:
        return build_wait_ritual_plan(trantorian, current_tile)
    return ["Look"]
