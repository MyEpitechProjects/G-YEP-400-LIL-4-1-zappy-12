##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/trantorian.py
## File description:
## Trantorian agent state (and how it integrates server responses)
##

import random

from ai.protocol import parse_inventory, parse_look

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
        self.last_broadcast = None
        self.gather_token = random.randint(0, 999999)
        self.actions_since_fork = 0

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
                    self.level = int(level)
            self.vision = []
        elif action == "Take":
            if response == "ok" and object_name:
                self.inventory[object_name] = self.inventory.get(object_name, 0) + 1
            self.vision = []
        elif action == "Set":
            if response == "ok" and object_name:
                self.inventory[object_name] = max(0, self.inventory.get(object_name, 0) - 1)
            self.vision = []
        elif action in ["Forward", "Right", "Left", "Eject"]:
            self.vision = []
        elif action == "Fork":
            self.available_slots = None
        return True
