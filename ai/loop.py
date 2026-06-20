##
## EPITECH PROJECT, 2026
## G-YEP-400 : ai/loop.py
## File description:
## Network core: send commands, read responses, run the main AI loop
##

from ai.trantorian import Trantorian
from ai.protocol import is_async_message, is_incantation_event, parse_broadcast, parse_heartbeat
from ai.strategy import build_plan
from ai.fork import spawn_child, spawn_children, fill_count
from ai.constants import PIPELINE_CAP, CONNECT_NBR_PERIOD, INVENTORY_REFRESH_PERIOD

def send_command(socket_file, command):
    """Send a command and detect a closed server connection."""

    try:
        socket_file.write(command + "\n")
        socket_file.flush()
    except (BrokenPipeError, ConnectionResetError, OSError) as error:
        print(f"Connection lost while sending '{command}': {error}")
        return False
    return True

def handle_async_line(line, trantorian):
    """Record heartbeats from team-mates; keep other async lines visible."""

    if line.startswith("message "):
        broadcast = parse_broadcast(line)
        if broadcast is not None:
            direction, text = broadcast
            heartbeat = parse_heartbeat(text)
            if heartbeat is not None:
                team, token, level, king_flag = heartbeat
                if team == trantorian.team_name:
                    trantorian.note_heartbeat(direction, token, level, king_flag)
                return
    print(f"[async] {line}")

def read_response(socket_file, trantorian, expecting_incantation):
    """Read a meaningful response and detect a closed connection."""

    while True:
        try:
            raw_line = socket_file.readline()
        except (ConnectionResetError, OSError) as error:
            print(f"Connection lost while reading: {error}")
            return None
        if raw_line == "":
            print("The server closed the connection.")
            return None
        line = raw_line.strip()
        if line == "":
            continue
        if is_async_message(line):
            handle_async_line(line, trantorian)
            continue
        if not expecting_incantation and is_incantation_event(line):
            if line.startswith("Current level:"):
                level = line.split("Current level:")[-1].strip()
                if level.isdigit():
                    trantorian.update_level(int(level))
            print(f"[async] {line}")
            continue
        return line

def handle_response(trantorian, verb, object_name, response):
    """Apply one response to the agent state. Returns False if dead."""

    flag = "K" if trantorian.is_king else "w"
    print(f"[L{trantorian.level} {trantorian.state} {flag} buf{trantorian.pending_commands}] {verb:<11} -> {response}")
    if response == "dead":
        print("The trantorian is dead")
        return False
    trantorian.update_from_response(verb, response, object_name)
    if verb == "Fork" and response == "ok":
        spawn_child(trantorian)
    return True

def execute_plan(socket_file, trantorian, plan):
    """Execute a plan and stop cleanly if the connection is lost."""

    sent = []
    plan_index = 0
    executed = 0
    while plan_index < len(plan) or sent:
        while (plan_index < len(plan) and len(sent) < PIPELINE_CAP):
            command = plan[plan_index]
            plan_index += 1
            parts = command.split()
            verb = parts[0]
            object_name = parts[1] if len(parts) > 1 else None
            if not send_command(socket_file, command):
                return False, executed
            trantorian.pending_commands += 1
            sent.append((verb, object_name))
        verb, object_name = sent.pop(0)
        response = read_response(socket_file, trantorian, verb == "Incantation")
        if response is None:
            trantorian.pending_commands = max(0, trantorian.pending_commands - 1)
            return False, executed
        if verb == "Incantation" and response == "Elevation underway":
            second_response = read_response(socket_file, trantorian, True)
            if second_response is None:
                trantorian.pending_commands = max(0, trantorian.pending_commands - 1)
                return False, executed
            response = (f"{response} / {second_response}")
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
        if since_inventory >= INVENTORY_REFRESH_PERIOD:
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
        if plan == ["Connect_nbr"]:
            spawn_children(trantorian, fill_count(trantorian))
        since_inventory += executed
        since_connect += executed
