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
        socket_file.write(command + "\n") # protocol: every command ends with '\n'
        socket_file.flush() # push out now
    # Error manager
    except (BrokenPipeError, ConnectionResetError, OSError) as error:
        print(f"Connection lost while sending '{command}': {error}")
        return False
    return True

def handle_async_line(line, trantorian):
    """Record valid heartbeats and handle asynchronous ejections."""

    # "eject: K" can arrive unsolicited
    if line.startswith("eject:"):
        trantorian.reset_eject_check()
        trantorian.vision = []
        print(f"[async] {line}")
        return
    # "message K, text": a broadcast.
    if line.startswith("message "):
        broadcast = parse_broadcast(line)
        if broadcast is not None:
            direction, text = broadcast
            heartbeat = parse_heartbeat(text)
            if heartbeat is not None:
                team, token, level, king_flag, state, sequence = heartbeat
                if team == trantorian.team_name:
                    trantorian.note_heartbeat(direction, token, level, king_flag, state, sequence)
                return
    print(f"[async] {line}")

def read_response(socket_file, trantorian, expecting_incantation):
    """Read a meaningful response and detect a closed connection."""

    while True: # loop because async lines must be skipped until a real reply arrives
        try:
            raw_line = socket_file.readline() # blocks until '\n' or EOF
        except (ConnectionResetError, OSError) as error:
            print(f"Connection lost while reading: {error}")
            return None
        if raw_line == "":  # if readline returns "" only on EOF -> socket closed
            print("The server closed the connection.")
            return None
        line = raw_line.strip()
        if line == "": # blank/keepalive line: ignore and read the next one
            continue
        if is_async_message(line): # broadcasts / ejects: handle then keep reading
            handle_async_line(line, trantorian)
            continue
        # A participant (not the initiator) gets "Current level: K" out of band: apply it, but only when he is not the one currently waiting on an Incantation.
        if not expecting_incantation and is_incantation_event(line):
            if line.startswith("Current level:"):
                level = line.split("Current level:")[-1].strip()
                if level.isdigit():
                    trantorian.update_level(int(level))
            print(f"[async] {line}")
            continue
        return line  # the actual answer to the waiting command

def handle_response(trantorian, verb, object_name, response):
    """Apply one response to the agent state. Returns False if dead."""

    flag = "K" if trantorian.is_king else "w"
    print(f"[L{trantorian.level} {trantorian.state} {flag} buf{trantorian.pending_commands}] {verb:<11} -> {response}")
    if response == "dead":
        print("The trantorian is dead")
        return False
    trantorian.update_from_response(verb, response, object_name)
    if verb == "Fork" and response == "ok":
        spawn_child(trantorian)  # egg laid -> immediately start a process to claim it
    return True

def execute_plan(socket_file, trantorian, plan):
    """Execute a plan and stop cleanly if the connection is lost."""

    # The protocol lets us queue up to 10 commands before reading.
    sent = []
    plan_index = 0
    executed = 0
    while plan_index < len(plan) or sent:
        # Fill the server-side buffer up to PIPELINE_CAP before blocking on reads.
        while (plan_index < len(plan) and len(sent) < PIPELINE_CAP):
            command = plan[plan_index]
            plan_index += 1
            parts = command.split()
            verb = parts[0]
            object_name = parts[1] if len(parts) > 1 else None # "Take food" -> "food"
            if not send_command(socket_file, command):
                return False, executed
            trantorian.pending_commands += 1
            sent.append((verb, object_name))
        # Pop the oldest command
        verb, object_name = sent.pop(0)
        response = read_response(socket_file, trantorian, verb == "Incantation")
        if response is None:  # connection died mid-flight
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
            return False, executed # agent died
    return True, executed

def run_ai(trantorian: Trantorian, socket_file):
    """Run the main AI loop and communicate with the server."""

    since_inventory = 0
    since_connect = 0
    while True:
        if since_inventory >= INVENTORY_REFRESH_PERIOD:  # Periodically resync the real inventory
            plan = ["Inventory"]
            since_inventory = 0
        elif since_connect >= CONNECT_NBR_PERIOD:  # Periodically ask how many team slots are free
            plan = ["Connect_nbr"]
            since_connect = 0
        else:
            plan = build_plan(trantorian)
        alive, executed = execute_plan(socket_file, trantorian, plan)
        if not alive:
            break
        if plan == ["Connect_nbr"]: # After learning the free-slot count, the king fills them with new processes.
            spawn_children(trantorian, fill_count(trantorian))
        since_inventory += executed
        since_connect += executed
