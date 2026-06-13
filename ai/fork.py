##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/fork.py
## File description:
## Reproduction: decide to fork, build child command, spawn child process
##

import subprocess

from ai.constants import COMFORTABLE_FOOD, FORK_COOLDOWN

def should_reproduce(trantorian):
    """Fork only if food is comfortable and the cooldown has elapsed."""
    food = trantorian.inventory.get("food", 0)
    return food >= COMFORTABLE_FOOD and trantorian.actions_since_fork >= FORK_COOLDOWN

def child_command(trantorian):
    return [trantorian.binary, "-p", str(trantorian.port), "-n", trantorian.team_name, "-h", trantorian.machine]

def spawn_child(trantorian):
    """Launch a new client to occupy the egg that has just been laid."""
    subprocess.Popen(child_command(trantorian), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)