from .models import Player
from .movement import MovementManager
from .network import NetworkManager
from .inventory import InventoryManager

class SurvivalManager:
    @staticmethod
    def check_food_level(player: Player) -> int:
        return InventoryManager.get_resource_count(player, "food")
    
    @staticmethod
    def needs_food(player: Player) -> bool:
        inventory = InventoryManager.get_inventory(player)
        return inventory.get('food', 0) < 5
    
    @staticmethod
    def needs_food_soon(player: Player) -> bool:
        inventory = InventoryManager.get_inventory(player)
        return inventory.get('food', 0) < 10
    
    @staticmethod
    def needs_food_critically(player: Player) -> bool:
        food_count = SurvivalManager.check_food_level(player)
        return food_count < 2
    
    @staticmethod
    def gather_food_aggressively(player: Player, target_amount: int = 15) -> bool:
        current_food = SurvivalManager.check_food_level(player)
        attempts = 0
        max_attempts = 30
        while attempts < max_attempts and current_food < target_amount:
            tiles = MovementManager.look_around(player)
            if len(tiles) > 0 and 'food' in tiles[0]:
                response = NetworkManager.send_command(player, 'Take food')
                if response == "ok":
                    current_food += 1
                    if current_food >= target_amount:
                        return True
            for index, tile in enumerate(tiles):
                if 'food' in tile:
                    MovementManager.navigate_to_tile(player, index)
                    response = NetworkManager.send_command(player, 'Take food')
                    if response == "ok":
                        current_food += 1
                        if current_food >= target_amount:
                            return True
            MovementManager.move_forward(player)
            if attempts % 6 == 0:
                MovementManager.turn_right(player)
            attempts += 1
            current_food = SurvivalManager.check_food_level(player)
        return SurvivalManager.check_food_level(player) >= target_amount
    
    @staticmethod
    def gather_food(player: Player) -> bool:
        attempts = 0
        max_attempts = 15
        while attempts < max_attempts:
            tiles = MovementManager.look_around(player)
            if len(tiles) > 0 and 'food' in tiles[0]:
                response = NetworkManager.send_command(player, 'Take food')
                if response == "ok":
                    return True
            for index, tile in enumerate(tiles):
                if 'food' in tile:
                    MovementManager.navigate_to_tile(player, index)
                    response = NetworkManager.send_command(player, 'Take food')
                    if response == "ok":
                        return True
            MovementManager.move_forward(player)
            if attempts % 4 == 0:
                MovementManager.turn_right(player)
            attempts += 1
        return False
    
    @staticmethod
    def should_stockpile_food(player: Player) -> bool:
        current_food = SurvivalManager.check_food_level(player)
        return current_food < 15 and player.level >= 2
