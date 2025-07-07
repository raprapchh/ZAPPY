import socket as sock_module
import random
from typing import List
from .models import Player, GameConfig

class NetworkManager:
    @staticmethod
    def create_connection(config: GameConfig, team_name: str) -> Player:
        player = Player()
        try:
            player.socket = sock_module.socket(sock_module.AF_INET, sock_module.SOCK_STREAM)
            player.socket.settimeout(1.0)
            player.socket.connect((config.host, config.port))
            welcome = player.socket.recv(1024).decode('utf-8')
            player.socket.sendall((team_name + '\n').encode('utf-8'))
            response = player.socket.recv(1024).decode('utf-8').strip().split()
            player.map_width = int(response[1])
            player.map_height = int(response[2])
            player.team_name = team_name
            player.player_id = random.randint(1000, 9999)
            player.is_leader = random.random() < 0.3
        except Exception:
            return None
        return player

    @staticmethod
    def send_command(player: Player, command: str) -> str:
        if not player.is_connected():
            return "ERROR: Not connected"
        try:
            player.socket.sendall((command + '\n').encode('utf-8'))
            response = player.socket.recv(1024).decode('utf-8').strip()
            if 'Current level:' in response:
                try:
                    new_level = int(response.split()[2])
                    player.level = new_level
                except (IndexError, ValueError):
                    pass
            if response == 'dead':
                return "DEAD"
            return response
        except (ConnectionResetError, BrokenPipeError):
            return "DISCONNECTED"
        except Exception:
            return "ERROR"

    @staticmethod
    def listen_for_broadcasts(player: Player) -> List[str]:
        messages = []
        try:
            player.socket.settimeout(0.1)
            while True:
                try:
                    data = player.socket.recv(1024).decode('utf-8')
                    if data:
                        lines = data.strip().split('\n')
                        for line in lines:
                            if 'message' in line.lower() or any(word in line for word in ['gather', 'elevate', 'coordinate']):
                                messages.append(line.strip())
                    else:
                        break
                except sock_module.timeout:
                    break
                except KeyboardInterrupt:
                    raise
        except KeyboardInterrupt:
            raise
        except Exception:
            pass
        finally:
            try:
                player.socket.settimeout(None)
            except:
                pass
        return messages
