##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/trantorian.py
## File description:
## Trantorian class
##

import random

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
        self.plan = []
        self.last_action = None
        self.last_object = None
        self.state = "EXPLORE"
        self.last_broadcast = None
        self.gather_token = random.randint(0, 999999)
        self.actions_since_fork = 0
    
