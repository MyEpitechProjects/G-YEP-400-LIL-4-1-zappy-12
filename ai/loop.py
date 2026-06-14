##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/loop.py
## File description:
## Network core: send commands, read responses, run the main AI loop
##

from ai.trantorian import Trantorian
from ai.protocol import is_async_message, is_incantation_event, parse_broadcast
from ai.strategy import build_plan
from ai.fork import spawn_child

def send_command(socket_file, command):
    socket_file.write(command + "\n")
    socket_file.flush()

def read_response(socket_file, trantorian, expecting_incantation):
    """Read the next meaningful response, skipping unsolicited async lines."""
    while True:
        line = socket_file.readline().strip()
        if is_async_message(line):
            if line.startswith("message "):
                broadcast = parse_broadcast(line)
                if broadcast is not None:
                    trantorian.heard_calls = (trantorian.heard_calls + [broadcast])[-20:]
            print(f"[async] {line}")
            continue
        if not expecting_incantation and is_incantation_event(line):
            if line.startswith("Current level:"):
                level = line.split("Current level:")[-1].strip()
                if level.isdigit():
                    trantorian.level = int(level)
                trantorian.vision = []
            print(f"[async] {line}")
            continue
        return line

def run_ai(trantorian: Trantorian, socket_file):
    """Run the main AI loop and communicate with the server."""
    plan = []
    since_inventory = 0
    while True:
        if not plan:
            if since_inventory >= 20:
                plan = ["Inventory"]
                since_inventory = 0
            else:
                plan = build_plan(trantorian)
        command = plan.pop(0)
        parts = command.split()
        verb = parts[0]
        if len(parts) > 1:
            object_name = parts[1]
        else:
            object_name = None
        send_command(socket_file, command)
        trantorian.pending_commands += 1
        response = read_response(socket_file, trantorian, verb == "Incantation")
        if verb == "Incantation" and response == "Elevation underway":
            second_response = read_response(socket_file, trantorian, True)
            response = f"{response} / {second_response}"
        trantorian.pending_commands -= 1
        print(f"[Buffer: {trantorian.pending_commands}/10] {verb:<11} -> {response}")
        if response == "dead":
            print("The trantorian is dead")
            break
        trantorian.update_from_response(verb, response, object_name)
        if verb == "Fork":
            trantorian.actions_since_fork = 0
            if response == "ok":
                spawn_child(trantorian)
        since_inventory += 1
        trantorian.actions_since_fork += 1
