#!/usr/bin/env python3
import socket as sock_module
from enum import Enum
from dataclasses import dataclass
from typing import List, Optional

class Direction(Enum):
    NORTH = 1
    EAST = 2
    SOUTH = 3
    WEST = 4

class Resource(Enum):
    FOOD = "food"
    LINEMATE = "linemate"
    DERAUMERE = "deraumere"
    SIBUR = "sibur"
    MENDIANE = "mendiane"
    PHIRAS = "phiras"
    THYSTAME = "thystame"

class AIState(Enum):
    EXPLORING = "exploring"
    COLLECTING = "collecting"
    COORDINATING = "coordinating"
    ELEVATING = "elevating"
    LISTENING = "listening"

@dataclass
class GameConfig:
    host: str = "127.0.0.1"
    port: int = 4242
    team_name: str = "Team1"

@dataclass
class Player:
    socket: Optional[sock_module.socket] = None
    level: int = 1
    player_id: int = 0
    map_width: int = 0
    map_height: int = 0
    team_name: str = ""
    state: AIState = AIState.EXPLORING
    last_broadcast_time: float = 0
    is_leader: bool = False
    
    def is_connected(self) -> bool:
        return self.socket is not None
