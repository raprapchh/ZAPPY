from typing import List
from .models import Player
from .network import NetworkManager

class MovementManager:
    @staticmethod
    def move_forward(player: Player) -> bool:
        response = NetworkManager.send_command(player, "Forward")
        return response == "ok"
    
    @staticmethod
    def turn_left(player: Player) -> bool:
        response = NetworkManager.send_command(player, "Left")
        return response == "ok"
    
    @staticmethod
    def turn_right(player: Player) -> bool:
        response = NetworkManager.send_command(player, "Right")
        return response == "ok"
    
    @staticmethod
    def look_around(player: Player) -> List[str]:
        response = NetworkManager.send_command(player, "Look")
        try:
            tiles = response.strip('[]').split(',')
            return [tile.strip() for tile in tiles]
        except:
            return []
    
    @staticmethod
    def navigate_to_tile(player: Player, tile_index: int):
        if tile_index == 0:
            return
        MovementManager.move_forward(player)
        if tile_index == 2:
            return
        elif tile_index == 1:
            MovementManager.turn_left(player)
            MovementManager.move_forward(player)
        elif tile_index == 3:
            MovementManager.turn_right(player)
            MovementManager.move_forward(player)
        elif player.level >= 2:
            MovementManager.move_forward(player)
            if tile_index == 4:
                MovementManager.turn_left(player)
                MovementManager.move_forward(player)
                MovementManager.move_forward(player)
                MovementManager.turn_right(player)
            elif tile_index == 5:
                MovementManager.turn_left(player)
                MovementManager.move_forward(player)
                MovementManager.turn_right(player)
            elif tile_index == 7:
                MovementManager.turn_right(player)
                MovementManager.move_forward(player)
                MovementManager.turn_left(player)
            elif tile_index == 8:
                MovementManager.turn_right(player)
                MovementManager.move_forward(player)
                MovementManager.move_forward(player)
                MovementManager.turn_left(player)
