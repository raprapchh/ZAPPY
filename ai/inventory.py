from .models import Player
from .network import NetworkManager

class InventoryManager:
    @staticmethod
    def get_inventory(player: Player) -> dict:
        response = NetworkManager.send_command(player, "Inventory")
        inventory = {}
        try:
            content = response.strip('[]')
            if not content:
                return {}
            items = content.split(',')
            for item in items:
                item = item.strip()
                if not item:
                    continue
                parts = item.split()
                if len(parts) >= 2:
                    for i in range(len(parts) - 1, -1, -1):
                        try:
                            count = int(parts[i])
                            name = ' '.join(parts[:i]) if i > 0 else parts[0]
                            name = name.strip()
                            inventory[name] = count
                            break
                        except ValueError:
                            continue
                else:
                    inventory[item] = 0
        except Exception:
            return {"food": 10}
        return inventory
    
    @staticmethod
    def get_resource_count(player: Player, resource: str) -> int:
        inventory = InventoryManager.get_inventory(player)
        return inventory.get(resource, 0)
