from .models import Player
from .movement import MovementManager
from .network import NetworkManager

class ResourceManager:
    @staticmethod
    def search_for_resource(player: Player, resource: str) -> int:
        search_attempts = 0
        max_attempts = min(player.map_width * player.map_height, 50)
        while search_attempts < max_attempts:
            tiles = MovementManager.look_around(player)
            if len(tiles) > 0 and 'food' in tiles[0]:
                NetworkManager.send_command(player, 'Take food')
            for index, tile in enumerate(tiles):
                if resource in tile:
                    return index
            for _ in range(2):
                MovementManager.move_forward(player)
                search_attempts += 1
                if search_attempts >= max_attempts:
                    break
            MovementManager.turn_right(player)
            search_attempts += 1
        return -1
    
    @staticmethod
    def find_and_go_to_resource(player: Player, resource: str):
        tile_index = ResourceManager.search_for_resource(player, resource)
        if tile_index != -1:
            MovementManager.navigate_to_tile(player, tile_index)
        return tile_index != -1
    
    @staticmethod
    def collect_resource(player: Player, resource: str) -> bool:
        if ResourceManager.find_and_go_to_resource(player, resource):
            response = NetworkManager.send_command(player, f'Take {resource}')
            return response == "ok"
        return False
