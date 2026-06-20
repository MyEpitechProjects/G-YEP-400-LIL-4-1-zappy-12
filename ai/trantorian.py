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
from ai.constants import FORAGE, FOOD_CRITICAL, FOOD_LOW_LIMIT, HEARD_TIMEOUT, POP_MEMORY, KING_STABLE_ROUNDS, KING_STICKY

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
        self.token = random.randint(1, 999999999)
        self.heard = {}
        self.seen = {}
        self.tick = 0
        self.is_king = False
        self.king_stable = 0
        self.king_direction = None
        self.has_anchor = False
        self.anchor_x = 0
        self.anchor_y = 0
        self.broadcast_counter = 0
        self.x = 0
        self.y = 0
        self.orientation = 0
        self.visited = {}
        self.last_incant_failed = False
        self.food_emergency = False
        self.rally_stall = 0
        self.last_fork_tick = -9999
        self.start_time = time.time()
        self.state = FORAGE

    def update_food_emergency(self):
        """Update food emergency mode using hysteresis thresholds."""

        food = self.inventory.get("food", 0)
        if self.food_emergency:
            if food >= FOOD_LOW_LIMIT:
                self.food_emergency = False
        elif food <= FOOD_CRITICAL:
            self.food_emergency = True
        return self.food_emergency

    def note_heartbeat(self, direction, token, level, king_flag):
        """Record a coordination heartbeat from another team member."""

        if token == self.token:
            return
        self.heard[token] = [self.tick, direction, level, king_flag]
        self.seen[token] = self.tick

    def prune_heard(self):
        """Forget stale per-tile heartbeats and stale population sightings."""

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
            priority = token + (KING_STICKY if info[3] else 0)
            if priority > best_priority:
                best_priority = priority
                best_direction = info[1]
                best_is_self = False
        if best_is_self:
            if not self.is_king:
                self.king_stable = 0
            self.is_king = True
            self.king_stable += 1
            self.king_direction = 0
            if not self.has_anchor:
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

    def update_level(self, new_level):
        """Update the level after an elevation and clear stale vision."""

        if new_level == self.level:
            return
        self.level = new_level
        self.last_incant_failed = False
        self.rally_stall = 0
        self.vision = []

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
            self.vision = []
        elif action == "Right":
            if response == "ok":
                self.orientation = turn_right(self.orientation)
            self.vision = []
        elif action == "Left":
            if response == "ok":
                self.orientation = turn_left(self.orientation)
            self.vision = []
        elif action == "Eject":
            self.vision = []
        elif action == "Fork":
            self.available_slots = None
        return True
