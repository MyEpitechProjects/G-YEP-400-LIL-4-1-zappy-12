import socket
import subprocess
import time
import threading
import sys
import os

PORT = 4244
NUM_CLIENTS = 10
SERVER_PROCESS = None

def run_client(client_id):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(5.0)
        s.connect(("127.0.0.1", PORT))
        
        data = s.recv(1024).decode("utf-8")
        if data != "WELCOME\n":
            print(f"Client {client_id} failed to connect.")
            return

        s.sendall(b"GRAPHIC\n")
        data = s.recv(4096).decode("utf-8")
        if "msz" not in data:
            print(f"Client {client_id} failed to get msz.")
            return

        for _ in range(10):
            s.sendall(b"mct\n")
            s.recv(4096)
            time.sleep(0.01)

        s.close()
    except Exception as e:
        print(f"Client {client_id} exception: {e}")

def main():
    global SERVER_PROCESS
    server_path = os.path.abspath("../../zappy_server")
    if not os.path.exists(server_path):
        print("Server binary not found")
        sys.exit(1)

    print("Starting server for load testing...")
    SERVER_PROCESS = subprocess.Popen(
        [server_path, "-p", str(PORT), "-x", "30", "-y", "30", "-n", "team1", "team2", "team3", "team4", "-c", "25", "-f", "1000"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    time.sleep(1)

    print(f"Spawning {NUM_CLIENTS} concurrent GUI clients...")
    threads = []
    start_time = time.time()
    
    for i in range(NUM_CLIENTS):
        t = threading.Thread(target=run_client, args=(i,))
        threads.append(t)
        t.start()
        
    for t in threads:
        t.join()
        
    end_time = time.time()
    print(f"Load test finished in {end_time - start_time:.2f} seconds.")
    
    SERVER_PROCESS.terminate()
    SERVER_PROCESS.wait()

if __name__ == "__main__":
    main()
