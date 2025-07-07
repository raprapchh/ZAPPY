import time
import random
from .models import GameConfig, Player, AIState
from .network import NetworkManager
from .survival import SurvivalManager
from .movement import MovementManager
from .coordination import CoordinationManager
from .elevation import ElevationManager
from .inventory import InventoryManager
from .resources import ResourceManager

class ZappyAI:
    def __init__(self, config: GameConfig):
        self.config = config
        self.player = None
        self.last_food_check = 0
    
    def connect(self) -> bool:
        self.player = NetworkManager.create_connection(self.config, self.config.team_name)
        return self.player is not None
    
    def run_intelligent_strategy(self):
        if not self.player:
            return
        if SurvivalManager.needs_food_critically(self.player) or SurvivalManager.check_food_level(self.player) < 5:
            SurvivalManager.gather_food(self.player)
        for _ in range(2):
            MovementManager.move_forward(self.player)
        iteration = 0
        max_iterations = 500
        last_food_check = 0
        try:
            while iteration < max_iterations:
                iteration += 1
                response = NetworkManager.send_command(self.player, "Look")
                if response in ["DEAD", "DISCONNECTED", "ERROR"]:
                    break
                if iteration - last_food_check > 12:
                    current_food = SurvivalManager.check_food_level(self.player)
                    if SurvivalManager.needs_food_critically(self.player):
                        SurvivalManager.gather_food(self.player)
                    elif SurvivalManager.should_stockpile_food(self.player) and self.player.level >= 2:
                        SurvivalManager.gather_food_aggressively(self.player, 15)
                    last_food_check = iteration
                CoordinationManager.listen_and_respond(self.player)
                current_food = SurvivalManager.check_food_level(self.player)
                if SurvivalManager.needs_food_critically(self.player):
                    SurvivalManager.gather_food(self.player)
                elif self.player.level == 1:
                    self._level_1_strategy()
                elif self.player.level == 2:
                    self._level_2_strategy()
                else:
                    self._higher_level_strategy()
                if iteration % 50 == 0 and current_food >= 8:
                    if self.player.is_leader:
                        food_status = "WELL_FED" if current_food >= 12 else "LOW_FOOD"
                        CoordinationManager.smart_broadcast(self.player, f"LEADER_LEVEL_{self.player.level}_{food_status}_SEEKING_TEAM")
                    elif current_food >= 10:
                        CoordinationManager.smart_broadcast(self.player, f"FOLLOWER_LEVEL_{self.player.level}_READY_FOOD_{current_food}")
                if current_food < 3:
                    time.sleep(0.05)
                elif current_food < 8:
                    time.sleep(0.1)
                else:
                    time.sleep(0.2)
        except KeyboardInterrupt:
            print(f"\nPlayer {self.player.player_id} interrupted by user")
            if self.player and self.player.socket:
                try:
                    self.player.socket.close()
                except:
                    pass
            raise
    
    def _level_1_strategy(self):
        if SurvivalManager.needs_food_critically(self.player):
            SurvivalManager.gather_food(self.player)
            return
        if self.player.state != AIState.ELEVATING:
            linemate_count = InventoryManager.get_resource_count(self.player, "linemate")
            if linemate_count < 1:
                self.player.state = AIState.COLLECTING
                ResourceManager.collect_resource(self.player, "linemate")
            else:
                self.player.state = AIState.ELEVATING
        if self.player.state == AIState.ELEVATING:
            if ElevationManager.gather_resources_for_level(self.player, 2):
                ElevationManager.attempt_elevation(self.player, 2)
    
    def _level_2_strategy(self):
        if SurvivalManager.needs_food(self.player):
            SurvivalManager.gather_food(self.player)
            return
        inventory = InventoryManager.get_inventory(self.player)
        missing_resources = []
        if inventory.get("linemate", 0) < 1:
            missing_resources.append("linemate")
        if inventory.get("deraumere", 0) < 1:
            missing_resources.append("deraumere")
        if inventory.get("sibur", 0) < 1:
            missing_resources.append("sibur")
        if SurvivalManager.needs_food_soon(self.player) and not SurvivalManager.needs_food(self.player):
            tiles = MovementManager.look_around(self.player)
            if len(tiles) > 0 and 'food' in tiles[0]:
                NetworkManager.send_command(self.player, 'Take food')
        if missing_resources and not SurvivalManager.needs_food_soon(self.player):
            target_resource = missing_resources[0]
            ResourceManager.collect_resource(self.player, target_resource)
        exploration_pattern = random.randint(1, 3)
        if exploration_pattern == 1:
            for _ in range(4):
                MovementManager.move_forward(self.player)
            MovementManager.turn_right(self.player)
        elif exploration_pattern == 2:
            MovementManager.move_forward(self.player)
            MovementManager.move_forward(self.player)
            MovementManager.turn_left(self.player)
            MovementManager.move_forward(self.player)
        else:
            if random.random() < 0.7:
                MovementManager.move_forward(self.player)
            else:
                if random.random() < 0.5:
                    MovementManager.turn_left(self.player)
                else:
                    MovementManager.turn_right(self.player)
        if self.player.is_leader and random.random() < 0.15:
            if not missing_resources:
                CoordinationManager.smart_broadcast(self.player, f"READY_LEVEL3_RESOURCES_COMPLETE_{self.player.player_id}")
            else:
                CoordinationManager.smart_broadcast(self.player, f"SEEKING_LEVEL3_NEED_{len(missing_resources)}_RESOURCES_{self.player.player_id}")
        elif random.random() < 0.08:
            if not missing_resources:
                CoordinationManager.smart_broadcast(self.player, f"LEVEL2_READY_FOR_ELEVATION_{self.player.player_id}")
            else:
                CoordinationManager.smart_broadcast(self.player, f"LEVEL2_COLLECTING_RESOURCES_{self.player.player_id}")
    
    def _higher_level_strategy(self):
        if SurvivalManager.needs_food_critically(self.player):
            SurvivalManager.gather_food(self.player)
            return
        if random.random() < 0.3:
            for _ in range(random.randint(2, 4)):
                MovementManager.move_forward(self.player)
        if self.player.is_leader and random.random() < 0.1:
            CoordinationManager.smart_broadcast(self.player, f"leader_level_{self.player.level}_organizing")
