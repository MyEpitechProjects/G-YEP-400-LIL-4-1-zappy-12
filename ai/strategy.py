##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/strategy.py
## File description:
## Decision making: king-rally coordination state machine
##

from ai.constants import REQUIREMENTS, STONES, HOARD_TARGET, SURVIVE, SOLO, KING, CONVERGE, RALLY, FORAGE, FOOD_TRAVEL_TARGET, FOOD_LOW_LIMIT, FOOD_RALLY_HIGH, FOOD_RALLY_ORBIT, KING_FOOD_KEEP, HIGH_FOOD_PRIORITY, NORMAL_FOOD_PRIORITY, STONE_PRIORITY, DISTANCE_PENALTY, CURRENT_TILE_BONUS, RALLY_STALL_LIMIT, WORKER_HEARTBEAT_PERIOD, KING_HEARTBEAT_PERIOD, EJECT_ALLY_MAX_AGE, EJECT_CONFIRM_ROUNDS, EJECT_COOLDOWN
from ai.protocol import build_heartbeat
from ai.fork import should_fork
from ai.navigation import distance_from_tile_index, build_move_plan, moves_toward_sound, step_forward, turns_to_face, navigate_to

def next_requirement(level):
    """Return the requirement dict for the next elevation, or None at the top."""

    return REQUIREMENTS.get(level + 1) # .get -> None at level 8 (no level 9)

def find_best_tile(vision, inventory, level):
    """Score visible tiles for foraging the food and stones we still need."""

    if len(vision) == 0:
        return 0
    requirements = REQUIREMENTS.get(level + 1, {})
    food = inventory.get("food", 0)
    best_tile_index = 0
    best_score = -999999 # sentinel below any real score
    for tile_index, tile in enumerate(vision):
        score = 0
        distance = distance_from_tile_index(tile_index)
        if food < FOOD_TRAVEL_TARGET:
            score += tile.get("food", 0) * HIGH_FOOD_PRIORITY
        else:
            score += tile.get("food", 0) * NORMAL_FOOD_PRIORITY
        for stone in STONES:
            missing = requirements.get(stone, 0) - inventory.get(stone, 0)
            if missing > 0:  # Only credit stones needs , and only as many as we lack.
                score += min(tile.get(stone, 0), missing) * STONE_PRIORITY
        score -= distance * DISTANCE_PENALTY # farther tiles cost time -> penalise
        if tile_index == 0:
            score += CURRENT_TILE_BONUS
        if score > best_score:
            best_score = score
            best_tile_index = tile_index
    return best_tile_index

def find_best_food_tile(vision):
    """Find the best visible tile containing food."""

    best_tile_index = None # None -> no food anywhere in sight
    best_score = None
    for tile_index, tile in enumerate(vision):
        food_amount = tile.get("food", 0)
        if food_amount <= 0:
            continue
        distance = distance_from_tile_index(tile_index)
        score = food_amount * HIGH_FOOD_PRIORITY - distance * DISTANCE_PENALTY
        if tile_index == 0:
            score += CURRENT_TILE_BONUS
        if best_score is None or score > best_score:
            best_score = score
            best_tile_index = tile_index
    return best_tile_index

def explore_plan(trantorian):
    """Frontier exploration: head toward the least-visited neighbouring tile."""

    x, y, orientation = trantorian.x, trantorian.y, trantorian.orientation
    width, height = trantorian.map_width, trantorian.map_height
    trantorian.visited[(x, y)] = trantorian.visited.get((x, y), 0) + 1 # mark current tile
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

def forage_takes(trantorian, tile):
    """Take food up to the travel reserve and stones up to the hoard target."""

    plan = []
    inventory = trantorian.inventory
    food_need = FOOD_TRAVEL_TARGET - inventory.get("food", 0)
    for _ in range(min(tile.get("food", 0), max(0, food_need))):
        plan.append("Take food")
    for stone in STONES:
        want = HOARD_TARGET.get(stone, 0) - inventory.get(stone, 0)
        for _ in range(min(tile.get(stone, 0), max(0, want))):
            plan.append("Take " + stone)
    return plan

def drop_stone_plan(trantorian, tile):
    """Drop one stone the rally tile still needs and that we are carrying."""

    requirements = next_requirement(trantorian.level)
    if requirements is None:
        return None
    for stone in STONES:
        if tile.get(stone, 0) < requirements.get(stone, 0) and trantorian.inventory.get(stone, 0) > 0:
            return ["Set " + stone]
    return None

def rite_blocked_by_missing_stone(trantorian, tile):
    """True if the rite needs a stone absent from both the tile and our bag."""

    requirements = next_requirement(trantorian.level)
    if requirements is None:
        return False
    for stone in STONES:
        if tile.get(stone, 0) < requirements.get(stone, 0) and trantorian.inventory.get(stone, 0) == 0:
            return True
    return False

def can_initiate(trantorian, tile):
    """True if this agent should start the incantation on its current tile."""

    requirements = next_requirement(trantorian.level)
    if requirements is None:
        return False
    for stone in STONES:
        if tile.get(stone, 0) < requirements.get(stone, 0):
            return False
    needed_players = requirements.get("players", 1)
    # present = ourselves + same-level teammates whose beacon comes from this tile.
    present = 1 + trantorian.same_tile_same_level_count()
    if present < needed_players:  # the Look must also confirm the bodies
        return False
    if tile.get("player", 0) < needed_players:
        return False
    return trantorian.is_min_token_on_tile() # only one initiator: the lowest token

def heartbeat_command(trantorian):
    """Build the periodic coordination broadcast command."""

    king_flag = 1 if trantorian.king_is_stable() else 0
    message = build_heartbeat(trantorian.team_name, trantorian.token, trantorian.level, king_flag, trantorian.state, trantorian.tick)
    return "Broadcast " + message

def should_broadcast(trantorian):
    """Advance the broadcast clock and tell whether to emit a heartbeat now."""

    trantorian.broadcast_counter += 1
    period = KING_HEARTBEAT_PERIOD if trantorian.is_king else WORKER_HEARTBEAT_PERIOD
    if trantorian.broadcast_counter >= period:
        trantorian.broadcast_counter = 0
        return True
    return False

def eject_strategy_plan(trantorian, tile):
    """Confirm that the only other player is not a team member before ejecting."""

    player_count = tile.get("player", 0)
    if trantorian.level <= 1: # nothing to protect yet
        trantorian.reset_eject_check()
        return None
    if trantorian.is_king or trantorian.king_direction == 0: # never eject on the king tile
        trantorian.reset_eject_check()
        return None
    if trantorian.tick - trantorian.last_eject_tick < EJECT_COOLDOWN: # rate-limit
        trantorian.reset_eject_check()
        return None
    if player_count != 2: # only the "1 stranger" case
        trantorian.reset_eject_check()
        return None
    if trantorian.same_tile_team_count() > 0: # the other one is an ally
        trantorian.reset_eject_check()
        return None
    if trantorian.eject_check_players != player_count:  # 1st sighting: start counting
        trantorian.eject_check_players = player_count
        trantorian.eject_check_rounds = 1
        trantorian.broadcast_counter = 0
        return [heartbeat_command(trantorian), "Look"]
    if trantorian.eject_check_rounds < EJECT_CONFIRM_ROUNDS:  # keep confirming
        trantorian.eject_check_rounds += 1
        trantorian.broadcast_counter = 0
        return [heartbeat_command(trantorian), "Look"]
    return ["Eject"] # confirmed stranger -> push

def survive_plan(trantorian, tile):
    """Emergency: eat the current tile or rush to the nearest food."""

    if tile.get("food", 0) > 0:
        return ["Take food"]
    best_tile = find_best_food_tile(trantorian.vision)
    if best_tile is not None and best_tile != 0:
        return build_move_plan(best_tile) + ["Look"]
    return explore_plan(trantorian) # no food in sight: go look elsewhere

def solo_plan(trantorian, tile):
    """Level 1->2 needs nobody else: stand on a linemate and elevate."""

    inventory = trantorian.inventory
    if tile.get("food", 0) > 0 and inventory.get("food", 0) < FOOD_TRAVEL_TARGET:
        return ["Take food"]
    if inventory.get("linemate", 0) >= 1:
        return ["Set linemate"]
    best_tile = find_best_tile(trantorian.vision, inventory, trantorian.level)
    if best_tile != 0:
        return build_move_plan(best_tile) + ["Look"]
    return explore_plan(trantorian)

def rally_plan(trantorian, tile):
    """On the king tile: eat the depot, hold position for the cohort, orbit only if starving."""

    trantorian.state = RALLY
    food = trantorian.inventory.get("food", 0)
    if food < FOOD_LOW_LIMIT and tile.get("food", 0) > 0:
        return ["Take food"]
    if food < FOOD_RALLY_ORBIT:
        best_food = find_best_food_tile(trantorian.vision)
        if best_food is not None and best_food != 0:
            return build_move_plan(best_food) + ["Look"]
    drop = drop_stone_plan(trantorian, tile)
    if drop is not None:
        trantorian.rally_stall = 0 # progress made: reset the stall counter
        return drop
    if food > FOOD_RALLY_HIGH:
        return ["Set food"]
    if rite_blocked_by_missing_stone(trantorian, tile):
        trantorian.rally_stall += 1
        if trantorian.rally_stall > RALLY_STALL_LIMIT:
            trantorian.rally_stall = 0
            best_tile = find_best_tile(trantorian.vision, trantorian.inventory, trantorian.level)
            if best_tile != 0:
                return build_move_plan(best_tile) + ["Look"]
            return explore_plan(trantorian)
    else:
        trantorian.rally_stall = 0
    return ["Look"] # default : hold position and keep the tile under observation

def king_plan(trantorian, tile):
    """King is a strictly stationary beacon: hold the anchor, eat the depot, seed stones."""

    trantorian.state = KING
    # If pushed off the anchor walk one step back toward it.
    if trantorian.has_anchor and (trantorian.x != trantorian.anchor_x or trantorian.y != trantorian.anchor_y):
        moves = navigate_to(trantorian.x, trantorian.y, trantorian.orientation, trantorian.anchor_x, trantorian.anchor_y, trantorian.map_width, trantorian.map_height)
        if moves:
            return moves + ["Look"]
    if trantorian.inventory.get("food", 0) < KING_FOOD_KEEP and tile.get("food", 0) > 0:
        return ["Take food"]
    drop = drop_stone_plan(trantorian, tile)
    if drop is not None:
        return drop
    return ["Look"]

def worker_plan(trantorian, tile):
    """Converge onto the king while foraging, then rally on its tile."""

    direction = trantorian.king_direction
    if direction is None: # No king located yet
        takes = forage_takes(trantorian, tile)
        if takes:
            return takes + ["Look"]
        trantorian.state = FORAGE
        return explore_plan(trantorian)
    if direction != 0: # king is elsewhrere
        takes = forage_takes(trantorian, tile)
        if takes:
            return takes + ["Look"]
        trantorian.state = CONVERGE
        return moves_toward_sound(direction) + ["Look"]
    return rally_plan(trantorian, tile) # direction 0 -> King tile

def idle_plan(trantorian, tile):
    """Top-level agent: survive and keep broadcasting (beacon if king)."""

    if tile.get("food", 0) > 0 and trantorian.inventory.get("food", 0) < KING_FOOD_KEEP:
        return ["Take food"]
    return ["Look"]

def build_plan(trantorian):
    """Build the next action plan from the king-rally coordination model."""

    trantorian.tick += 1
    trantorian.prune_heard()
    trantorian.update_role()
    if not trantorian.inventory:
        return ["Inventory"]
    if not trantorian.vision:
        return ["Look"]
    tile = trantorian.vision[0]
    if trantorian.update_food_emergency():
        trantorian.state = SURVIVE
        trantorian.reset_eject_check()
        return survive_plan(trantorian, tile)
    if can_initiate(trantorian, tile):
        trantorian.last_incant_failed = False
        trantorian.reset_eject_check()
        return ["Incantation"]
    eject_plan = eject_strategy_plan(trantorian, tile)
    if eject_plan is not None:
        return eject_plan
    if should_broadcast(trantorian):
        return [heartbeat_command(trantorian)]
    if should_fork(trantorian):
        trantorian.last_fork_tick = trantorian.tick # arm the fork cooldown
        return ["Fork"]
    requirements = next_requirement(trantorian.level)
    if requirements is None: # already at the max level
        return idle_plan(trantorian, tile)
    if requirements.get("players", 1) <= 1: # 1->2 is solo : no coordination
        trantorian.state = SOLO
        return solo_plan(trantorian, tile)
    if trantorian.is_king: # group level : king anchors, worker converges
        return king_plan(trantorian, tile)
    return worker_plan(trantorian, tile)
