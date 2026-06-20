##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/fork.py
## File description:
## Reproduction: the king grows/maintains the team near the population target
##

import os
import sys
import time
import subprocess

from ai.constants import POP_TARGET, POP_WARMUP_SECONDS, FORK_COOLDOWN, FILL_BATCH, FOOD_LOW_LIMIT

PACKAGE_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def child_output(trantorian):
    """Pick where a spawned child logs: a test directory or /dev/null."""

    log_dir = os.environ.get("ZAPPY_AI_LOGDIR")
    if not log_dir:
        return subprocess.DEVNULL
    path = os.path.join(log_dir, f"child_{trantorian.team_name}_{os.getpid()}_{int(time.time()*1000)%100000}.log")
    return open(path, "w")

def spawn_child(trantorian):
    """Launch one new client process to occupy a free team slot."""

    command = [sys.executable, "-m", "ai.main", "-p", str(trantorian.port), "-n", trantorian.team_name, "-h", trantorian.machine]
    output = child_output(trantorian)
    subprocess.Popen(command, cwd=PACKAGE_ROOT, stdout=output, stderr=subprocess.STDOUT)

def spawn_children(trantorian, count):
    """Launch several new clients at once."""

    for _ in range(count):
        spawn_child(trantorian)

def is_population_manager(trantorian):
    """Only the established king manages reproduction, once its head-count is reliable."""

    return trantorian.king_is_stable() and (time.time() - trantorian.start_time) > POP_WARMUP_SECONDS

def should_fork(trantorian):
    """King lays a fresh egg when the team is below its population target."""

    if not is_population_manager(trantorian):
        return False
    if trantorian.population_estimate() >= POP_TARGET:
        return False
    if trantorian.tick - trantorian.last_fork_tick < FORK_COOLDOWN:
        return False
    if trantorian.inventory.get("food", 0) < FOOD_LOW_LIMIT:
        return False
    return True

def fill_count(trantorian):
    """How many clients the king should spawn to occupy free egg slots now."""

    if not is_population_manager(trantorian):
        return 0
    if trantorian.population_estimate() >= POP_TARGET:
        return 0
    slots = trantorian.available_slots
    if slots is None or slots <= 0:
        return 0
    return min(slots, FILL_BATCH)
