##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/trantorian.py
## File description:
## Trantorian agent state (and how it integrates server responses)
##

import random

from ai.protocol import parse_inventory, parse_look
from ai.navigation import step_forward, turn_right, turn_left
from ai.constants import EXPLORE, FOOD_LOW_LIMIT, FOOD_SAFE_LIMIT

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
        self.heard_calls = []
        self.is_anchor = False
        self.gather_token = random.randint(0, 999999)
        self.actions_since_fork = 0
        self.x = 0
        self.y = 0
        self.orientation = 0
        self.visited = {}
        self.last_incant_failed = False
        self.state = EXPLORE
        self.anchor_token = None
        self.gather_silence = 0
        self.anchor_confirmations = 0
        self.ready_calls = []
        self.ready_players = {}
        self.ready_repeat_counter = 0
        self.coordination_tick = 0
        self.food_emergency = False


    def update_food_emergency(self):
        """Update food emergency mode using hysteresis thresholds."""

        food = self.inventory.get("food", 0)
        if self.food_emergency:
            if food >= FOOD_SAFE_LIMIT:
                self.food_emergency = False
        elif food <= FOOD_LOW_LIMIT:
            self.food_emergency = True
        return self.food_emergency

    def reset_gather_state(self):
        """Reset gathering and ritual coordination state."""

        self.state = EXPLORE
        self.is_anchor = False
        self.anchor_token = None
        self.gather_silence = 0
        self.anchor_confirmations = 0
        self.heard_calls = []
        self.ready_calls = []
        self.ready_players = {}
        self.ready_repeat_counter = 0
        self.coordination_tick = 0

    def reset_election(self):
        """Start a fresh anchor election for the current level."""

        self.reset_gather_state()
        self.gather_token = random.randint(0, 999999)
        self.last_incant_failed = False

    def update_level(self, new_level):
        """Update the level and restart anchor election after elevation."""

        if new_level == self.level:
            return
        self.level = new_level
        self.reset_election()
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
