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
from ai.constants import PIPELINE_CAP, CONNECT_NBR_PERIOD

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
    
def handle_response(trantorian, verb, object_name, response):
    """Apply one response to the agent state. Returns False if dead."""
    print(f"[Buffer: {trantorian.pending_commands}/10] {verb:<11} -> {response}")
    if response == "dead":
        print("The trantorian is dead")
        return False
    trantorian.update_from_response(verb, response, object_name)
    if verb == "Fork":
        trantorian.actions_since_fork = 0
        if response == "ok":
            spawn_child(trantorian)
    trantorian.actions_since_fork += 1
    return True

def execute_plan(socket_file, trantorian, plan):
    """Send the whole plan keeping the server buffer filled, read replies in order.
    Returns (alive, executed_count)."""
    sent = []
    i = 0
    n = len(plan)
    executed = 0
    while i < n or sent:
        while i < n and len(sent) < PIPELINE_CAP:
            command = plan[i]
            i += 1
            parts = command.split()
            verb = parts[0]
            object_name = parts[1] if len(parts) > 1 else None
            send_command(socket_file, command)
            trantorian.pending_commands += 1
            sent.append((verb, object_name))
        verb, object_name = sent.pop(0)
        response = read_response(socket_file, trantorian, verb == "Incantation")
        if verb == "Incantation" and response == "Elevation underway":
            second = read_response(socket_file, trantorian, True)
            response = f"{response} / {second}"
        trantorian.pending_commands -= 1
        executed += 1
        if not handle_response(trantorian, verb, object_name, response):
            return False, executed
    return True, executed

def run_ai(trantorian: Trantorian, socket_file):
    """Run the main AI loop and communicate with the server."""
    since_inventory = 0
    since_connect = 0
    while True:
        if since_inventory >= 20:
            plan = ["Inventory"]
            since_inventory = 0
        elif since_connect >= CONNECT_NBR_PERIOD:
            plan = ["Connect_nbr"]
            since_connect = 0
        else:
            plan = build_plan(trantorian)
        alive, executed = execute_plan(socket_file, trantorian, plan)
        if not alive:
            break
        since_inventory += executed
        since_connect += executed
