import random
from .models import Player
from .network import NetworkManager
from .inventory import InventoryManager
from .resources import ResourceManager
from .survival import SurvivalManager

class ElevationManager:
    ELEVATION_REQUIREMENTS = {
        2: {"players": 1, "linemate": 1, "deraumere": 0, "sibur": 0, "mendiane": 0, "phiras": 0, "thystame": 0},
        3: {"players": 2, "linemate": 1, "deraumere": 1, "sibur": 1, "mendiane": 0, "phiras": 0, "thystame": 0},
        4: {"players": 2, "linemate": 2, "deraumere": 0, "sibur": 1, "mendiane": 0, "phiras": 2, "thystame": 0},
        5: {"players": 4, "linemate": 1, "deraumere": 1, "sibur": 2, "mendiane": 0, "phiras": 1, "thystame": 0},
        6: {"players": 4, "linemate": 1, "deraumere": 2, "sibur": 1, "mendiane": 3, "phiras": 0, "thystame": 0},
        7: {"players": 6, "linemate": 1, "deraumere": 2, "sibur": 3, "mendiane": 0, "phiras": 1, "thystame": 0},
        8: {"players": 6, "linemate": 2, "deraumere": 2, "sibur": 2, "mendiane": 2, "phiras": 2, "thystame": 1}
    }
    
    @staticmethod
    def gather_resources_for_level(player: Player, target_level: int) -> bool:
        if target_level not in ElevationManager.ELEVATION_REQUIREMENTS:
            return False
        requirements = ElevationManager.ELEVATION_REQUIREMENTS[target_level]
        for resource, needed_count in requirements.items():
            if resource == "players" or needed_count == 0:
                continue
            current_count = InventoryManager.get_resource_count(player, resource)
            attempts = 0
            max_attempts = 10
            while current_count < needed_count and attempts < max_attempts:
                if ResourceManager.collect_resource(player, resource):
                    current_count = InventoryManager.get_resource_count(player, resource)
                else:
                    attempts += 1
                    from .movement import MovementManager
                    for _ in range(random.randint(1, 3)):
                        MovementManager.move_forward(player)
                    if random.random() < 0.5:
                        MovementManager.turn_left(player)
                    else:
                        MovementManager.turn_right(player)
                if SurvivalManager.needs_food_critically(player):
                    SurvivalManager.gather_food(player)
                    break
        return True
    
    @staticmethod
    def place_resources_for_level(player: Player, target_level: int):
        if target_level not in ElevationManager.ELEVATION_REQUIREMENTS:
            return
        requirements = ElevationManager.ELEVATION_REQUIREMENTS[target_level]
        for resource, needed_count in requirements.items():
            if resource == "players" or needed_count == 0:
                continue
            for _ in range(needed_count):
                NetworkManager.send_command(player, f'Set {resource}')
    
    @staticmethod
    def attempt_elevation(player: Player, target_level: int = None):
        if target_level is None:
            target_level = player.level + 1
        ElevationManager.place_resources_for_level(player, target_level)
        response = NetworkManager.send_command(player, "Incantation")
        NetworkManager.send_command(player, "Look")
        return "Current level:" in response
