##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/constants.py
## File description:
## Where global variable are stored
##

# Incantation requirements: level_to_reach -> players + stones
REQUIREMENTS = {
    2: {"players": 1, "linemate": 1, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0},
    3: {"players": 2, "linemate": 1, "deraumere": 1, "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0},
    4: {"players": 2, "linemate": 2, "deraumere": 0, "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0},
    5: {"players": 4, "linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0},
    6: {"players": 4, "linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0},
    7: {"players": 6, "linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0},
    8: {"players": 6, "linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}
}
STONES = ["linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame"]
MAX_LEVEL = 8
# Per-stone maximum ever needed by a single rite: foragers hoard up to this so a camped cohort can pool the stones for any level without leaving to gather more.
HOARD_TARGET = {stone: max(level_req.get(stone, 0) for level_req in REQUIREMENTS.values()) for stone in STONES}

# AI states (used for logging / branching)
SURVIVE = "SURVIVE"
SOLO = "SOLO"
KING = "KING"
CONVERGE = "CONVERGE"
RALLY = "RALLY"
FORAGE = "FORAGE"

# Food management
FOOD_CRITICAL = 18
FOOD_LOW_LIMIT = 35
FOOD_TRAVEL_TARGET = 75
FOOD_RALLY_HIGH = 40
FOOD_RALLY_ORBIT = 24
KING_FOOD_KEEP = 55

# Tile scoring
HIGH_FOOD_PRIORITY = 100
NORMAL_FOOD_PRIORITY = 20
STONE_PRIORITY = 80
DISTANCE_PENALTY = 10
CURRENT_TILE_BONUS = 5

# Coordination (broadcast heartbeats)
WORKER_HEARTBEAT_PERIOD = 3
KING_HEARTBEAT_PERIOD = 2
HEARD_TIMEOUT = 25
RALLY_STALL_LIMIT = 35
KING_STICKY = 2000000000

# Population control 
POP_TARGET = 10
POP_MEMORY = 70
POP_WARMUP_SECONDS = 25
FORK_COOLDOWN = 25
FILL_BATCH = 1
KING_STABLE_ROUNDS = 4

# Networking / loop cadence
PIPELINE_CAP = 1
CONNECT_NBR_PERIOD = 18
INVENTORY_REFRESH_PERIOD = 10
