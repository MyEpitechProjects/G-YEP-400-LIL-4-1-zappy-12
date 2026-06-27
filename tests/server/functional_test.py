import socket
import subprocess
import time
import unittest
import os
import sys

class TestZappyServerFunctional(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_path = os.path.abspath("../../zappy_server")
        if not os.path.exists(server_path):
            raise unittest.SkipTest(f"Server binary not found at {server_path}")
        cls.port = 4243
        cls.server_process = subprocess.Popen(
            [server_path, "-p", str(cls.port), "-x", "10", "-y", "10", "-n", "team1", "team2", "-c", "2", "-f", "100"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        time.sleep(1)

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()
        cls.server_process.wait()

    def test_client_connection(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect(("127.0.0.1", self.port))
        
        data = s.recv(1024).decode("utf-8")
        self.assertEqual(data, "WELCOME\n")
        
        s.sendall(b"team1\n")
        data = s.recv(1024).decode("utf-8")
        lines = data.split('\n')
        self.assertTrue(len(lines) >= 2)
        
        s.close()

    def test_gui_connection(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect(("127.0.0.1", self.port))
        
        data = s.recv(1024).decode("utf-8")
        self.assertEqual(data, "WELCOME\n")
        
        s.sendall(b"GRAPHIC\n")
        time.sleep(0.1)
        data = s.recv(4096).decode("utf-8")
        self.assertIn("msz 10 10", data)
        self.assertIn("sgt 100", data)
        
        s.close()

    def test_invalid_team(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect(("127.0.0.1", self.port))
        
        data = s.recv(1024).decode("utf-8")
        self.assertEqual(data, "WELCOME\n")
        
        s.sendall(b"invalid_team\n")
        try:
            data = s.recv(1024).decode("utf-8")
            self.assertEqual(data, "ko\n")
        except socket.timeout:
            pass
        s.close()

if __name__ == "__main__":
    unittest.main()
