##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/trantorian.py
## File description:
## Trantorian agent state (and how it integrates server responses)
##

import random
import time

from ai.protocol import parse_inventory, parse_look
from ai.navigation import step_forward, turn_right, turn_left
from ai.constants import FORAGE, FOOD_CRITICAL, FOOD_LOW_LIMIT, HEARD_TIMEOUT, POP_MEMORY, KING_STABLE_ROUNDS, KING_STICKY, EJECT_ALLY_MAX_AGE

class Trantorian:

    def __init__(self, team_name: str, map_width: int, map_height: int, initial_slots: int):
        self.team_name = team_name
        self.map_width = map_width
        self.map_height = map_height
        self.level = 1
        self.vision = []
        self.inventory = {"food": 10, "linemate": 0, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0}
        self.available_slots = initial_slots
        self.pending_commands = 0
        # Random tie-breaker that identifies this agent in broadcasts. Big range makes collisions between teammates extremely unlikely
        # the lowest/highest token is used to pick a single anchor/king deterministically
        self.token = random.randint(1, 999999999)
        # heard[token] = [tick, direction, level, king_flag, state, sequence]
        # tick : local tick when last heard 
        # direction : sound tile 0..8 the beacon came from
        # seen[token] = last tick a token was heard at all 
        self.heard = {}
        self.seen = {}
        self.tick = 0 # local action counter : notion of time for cooldowns
        self.is_king = False
        self.king_stable = 0 # consecutive rounds -> held the crown
        self.king_direction = None  # sound direction toward the king (0 = here)
        self.has_anchor = False
        self.anchor_x = 0
        self.anchor_y = 0
        self.broadcast_counter = 0
        # Dead-reckoned position/orientation relative to spawn.
        self.x = 0
        self.y = 0
        self.orientation = 0
        self.visited = {}  # (x, y) -> visit count, drives exploration
        self.last_incant_failed = False
        self.food_emergency = False
        self.rally_stall = 0
        self.last_fork_tick = -9999 # far in the past => no cooldown at startup
        self.eject_check_rounds = 0
        self.eject_check_players = 0
        self.last_eject_tick = -9999
        self.start_time = time.time()  # absolute time, for the reproduction warmup
        self.state = FORAGE

    def update_food_emergency(self):
        """Update food emergency mode using hysteresis thresholds."""

        # Hysteresis: enter panic at <= FOOD_CRITICAL, only leave once back >= FOOD_LOW_LIMIT.
        # Two different thresholds prevent flapping in/out of SURVIVE near the boundary.
        food = self.inventory.get("food", 0)
        if self.food_emergency:
            if food >= FOOD_LOW_LIMIT:
                self.food_emergency = False
        elif food <= FOOD_CRITICAL:
            self.food_emergency = True
        return self.food_emergency

    def note_heartbeat(self, direction, token, level, king_flag, state, sequence):
        """Record the newest heartbeat from another team member."""

        if token == self.token:
            return
        previous = self.heard.get(token)
        # sequence is the sender's own tick: keep only strictly newer messages (older/duplicate beacons arriving late are dropped)
        if previous is not None and sequence <= previous[5]:
            return
        self.heard[token] = [self.tick, direction, level, king_flag, state, sequence]
        self.seen[token] = self.tick

    def prune_heard(self):
        """Forget stale per-tile heartbeats and stale population sightings."""

        # Build the kill-lists first, then delete: never mutate a dict while iterating it.
        stale = [token for token, info in self.heard.items() if self.tick - info[0] > HEARD_TIMEOUT]
        for token in stale:
            del self.heard[token]
        gone = [token for token, last in self.seen.items() if self.tick - last > POP_MEMORY]
        for token in gone:
            del self.seen[token]

    def update_role(self):
        """Elect the sticky team king (an established king keeps its crown)."""

        was_stable_king = self.is_king and self.king_stable >= KING_STABLE_ROUNDS
        best_priority = self.token + (KING_STICKY if was_stable_king else 0)
        best_direction = None
        best_is_self = True
        for token, info in self.heard.items():
            priority = token + (KING_STICKY if info[3] else 0)  # info[3] = their king_flag
            if priority > best_priority:
                best_priority = priority
                best_direction = info[1] # info[1] = direction toward that king
                best_is_self = False
        if best_is_self:
            if not self.is_king:
                self.king_stable = 0  # just won the crown : reset the counter
            self.is_king = True
            self.king_stable += 1
            self.king_direction = 0 # the king is "here" from its own point of view
            if not self.has_anchor:  # pin the rally point once, where standed
                self.anchor_x = self.x
                self.anchor_y = self.y
                self.has_anchor = True
        else:
            self.is_king = False
            self.king_stable = 0
            self.king_direction = best_direction
            self.has_anchor = False

    def king_is_stable(self):
        """True once king ownership has held long enough to act on it."""

        return self.is_king and self.king_stable >= KING_STABLE_ROUNDS

    def population_estimate(self):
        """Estimate alive team members from recently-seen distinct tokens."""

        return 1 + len(self.seen)

    def same_tile_same_level_count(self):
        """Count same-level team members heard from the current tile."""

        count = 0
        for info in self.heard.values():
            if info[1] == 0 and info[2] == self.level:
                count += 1
        return count

    def is_min_token_on_tile(self):
        """True if this agent owns the lowest token among same-level peers here."""

        for token, info in self.heard.items():
            if info[1] == 0 and info[2] == self.level and token < self.token:
                return False
        return True
    
    def reset_eject_check(self):
        """Reset the current enemy verification process."""

        self.eject_check_rounds = 0
        self.eject_check_players = 0

    def same_tile_team_count(self):
        """Count recently confirmed team members on the current tile."""

        # Stricter than same_tile_* : only beacons fresh within EJECT_ALLY_MAX_AGE count -> never eject someone who just proved they are an ally.
        count = 0
        for info in self.heard.values():
            if info[1] == 0 and self.tick - info[0] <= EJECT_ALLY_MAX_AGE:
                count += 1
        return count

    def update_level(self, new_level):
        """Update the level after an elevation and clear stale vision."""

        if new_level == self.level:  # avoid clear vision for nothing
            return
        self.level = new_level 
        self.last_incant_failed = False
        self.rally_stall = 0
        self.reset_eject_check()
        self.vision = [] # vision is level-sized -> it is invalid after elevating

    def update_from_response(self, action, response, object_name=None):
        """Integrate the last server response into the agent's own state."""

        if response == "dead":
            return False
        if action == "Inventory":
            inventory = parse_inventory(response)
            if inventory is not None:
                self.inventory = inventory
        elif action == "Look":
            vision = parse_look(response)
            if vision is not None:
                self.vision = vision
        elif action == "Connect_nbr":
            if response.isdigit():
                self.available_slots = int(response)
        elif action == "Incantation":
            if "Current level:" in response:
                level = response.split("Current level:")[-1].strip()
                if level.isdigit():
                    self.update_level(int(level))
                self.last_incant_failed = False
            else:
                self.last_incant_failed = True
            self.vision = []
        elif action == "Take":
            if response == "ok" and object_name:
                self.inventory[object_name] = self.inventory.get(object_name, 0) + 1
            self.vision = []
        elif action == "Set":
            if response == "ok" and object_name:
                self.inventory[object_name] = max(0, self.inventory.get(object_name, 0) - 1)
            self.vision = []
        elif action == "Forward":
            if response == "ok":
                self.x, self.y = step_forward(self.x, self.y, self.orientation, self.map_width, self.map_height)
            self.reset_eject_check()
            self.vision = []  # any move invalidates the last Look
        elif action == "Right":
            if response == "ok":
                self.orientation = turn_right(self.orientation)
            self.reset_eject_check()
            self.vision = []
        elif action == "Left":
            if response == "ok":
                self.orientation = turn_left(self.orientation)
            self.reset_eject_check()
            self.vision = []
        elif action == "Eject":
            if response == "ok":
                self.last_eject_tick = self.tick # start the eject cooldown
            self.reset_eject_check()
            self.vision = []
        elif action == "Fork":
            self.available_slots = None # unknown until the next Connect_nbr reply
        return True
