import time
import random
from .models import Player, AIState
from .network import NetworkManager

class CoordinationManager:
    @staticmethod
    def should_broadcast(player: Player) -> bool:
        current_time = time.time()
        min_interval = 3.0 if player.is_leader else 5.0
        time_since_last = current_time - player.last_broadcast_time
        random_factor = random.uniform(0.8, 1.2)
        return time_since_last > (min_interval * random_factor)

    @staticmethod
    def smart_broadcast(player: Player, message: str):
        if CoordinationManager.should_broadcast(player):
            recent_messages = NetworkManager.listen_for_broadcasts(player)
            if not any(message.lower() in msg.lower() for msg in recent_messages):
                NetworkManager.send_command(player, f"Broadcast {message}")
                player.last_broadcast_time = time.time()
                return True
        return False

    @staticmethod
    def listen_and_respond(player: Player):
        try:
            messages = NetworkManager.listen_for_broadcasts(player)
            for message in messages:
                if 'gather' in message.lower():
                    player.state = AIState.COLLECTING
                elif 'elevate' in message.lower():
                    player.state = AIState.ELEVATING
                elif 'coordinate' in message.lower():
                    player.state = AIState.COORDINATING
        except KeyboardInterrupt:
            raise
