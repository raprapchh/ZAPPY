# Architecture du Client AI Zappy

## Vue d'ensemble

Le client AI est un programme autonome qui contrôle un joueur dans le monde Zappy. Il doit prendre des décisions stratégiques pour survivre, collecter des ressources et coordonner avec son équipe pour atteindre le niveau 8.

## Architecture comportementale

```
┌─────────────────────────────────────────────────────┐
│                   AI CLIENT                         │
│                                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────┐  │
│  │ State Machine│  │   Analyzer   │  │ Planner   │  │
│  │              │  │              │  │           │  │
│  │  - Survival  │  │ - World Map  │  │ - Goals   │  │
│  │  - Gather    │  │ - Inventory  │  │ - Actions │  │
│  │  - Elevate   │  │ - Team State │  │ - Path    │  │
│  └──────┬───────┘  └───────┬──────┘  └─────┬─────┘  │
│         │                  │               │        │
│         └──────────────────┴───────────────┘        │
│                            │                        │
│                     ┌──────▼───────┐                │
│                     │   Executor   │                │
│                     │              │                │
│                     │ - Commands   │                │
│                     │ - Network    │                │
│                     └──────────────┘                │
└─────────────────────────────────────────────────────┘
```

## Structure recommandée (Python)

```
ai/
├── main.py                 # Point d'entrée
├── core/
│   ├── client.py          # Client réseau
│   ├── protocol.py        # Gestion du protocole
│   └── config.py          # Configuration
│
├── brain/
│   ├── state_machine.py   # Machine à états
│   ├── analyzer.py        # Analyse de l'environnement
│   ├── planner.py         # Planification d'actions
│   └── memory.py          # Mémoire de l'IA
│
├── strategies/
│   ├── survival.py        # Stratégie de survie
│   ├── gathering.py       # Collecte de ressources
│   ├── elevation.py       # Stratégie d'élévation
│   └── exploration.py     # Exploration de la carte
│
├── behaviors/
│   ├── movement.py        # Comportements de déplacement
│   ├── communication.py   # Communication équipe
│   ├── combat.py          # Éjection défensive
│   └── reproduction.py    # Gestion des forks
│
├── utils/
│   ├── pathfinding.py     # Algorithmes de pathfinding
│   ├── geometry.py        # Calculs géométriques
│   └── logger.py          # Système de logs
│
└── tests/
    ├── test_behaviors.py
    └── test_strategies.py
```

## Machine à états

### États principaux

```python
class AIState(Enum):
    # États de base
    INITIALIZING = "initializing"
    IDLE = "idle"
    
    # États de survie
    CRITICAL_FOOD = "critical_food"      # < 10 food
    SEEKING_FOOD = "seeking_food"        # < 50 food
    
    # États de collecte
    GATHERING_RESOURCES = "gathering"
    DEPOSITING = "depositing"
    
    # États d'élévation
    PREPARING_ELEVATION = "prep_elevation"
    SEEKING_PARTNERS = "seeking_partners"
    PERFORMING_ELEVATION = "elevation"
    
    # États d'exploration
    EXPLORING = "exploring"
    MAPPING = "mapping"
    
    # États d'équipe
    RESPONDING_TO_CALL = "responding"
    BROADCASTING = "broadcasting"
    
    # États spéciaux
    REPRODUCING = "forking"
    DEFENDING = "defending"
    DEAD = "dead"
```

### Transitions d'états

```
                    ┌─────────────┐
                    │ INITIALIZING│
                    └──────┬──────┘
                           │
                    ┌──────▼──────┐
               ┌────┤    IDLE     ├────┐
               │    └──────┬──────┘    │
               │           │           │
        food<10│    food<50│           │resources_needed
               │           │           │
        ┌──────▼──────┐ ┌──▼───┐ ┌────▼────┐
        │CRITICAL_FOOD│ │SEEKING││GATHERING│
        └─────────────┘ │ FOOD │ └─────────┘
                        └──────┘
```

## Stratégies détaillées

### 1. Stratégie de survie

```python
class SurvivalStrategy:
    CRITICAL_THRESHOLD = 10   # Unités de temps
    SAFE_THRESHOLD = 100      # Unités de temps
    COMFORTABLE_THRESHOLD = 300
    
    def evaluate_food_need(self, inventory):
        food_time = inventory['food'] * 126
        
        if food_time < self.CRITICAL_THRESHOLD:
            return Priority.CRITICAL
        elif food_time < self.SAFE_THRESHOLD:
            return Priority.HIGH
        elif food_time < self.COMFORTABLE_THRESHOLD:
            return Priority.MEDIUM
        return Priority.LOW
    
    def find_nearest_food(self, vision_data):
        # Analyser la vision pour trouver la nourriture
        # Utiliser BFS pour le chemin le plus court
        pass
```

### 2. Stratégie de collecte

```python
class GatheringStrategy:
    def __init__(self):
        self.resource_goals = {
            1: {'linemate': 1},
            2: {'linemate': 1, 'deraumere': 1, 'sibur': 1},
            3: {'linemate': 2, 'sibur': 1, 'phiras': 2},
            # ... etc
        }
    
    def calculate_needed_resources(self, level, inventory):
        needed = {}
        goals = self.resource_goals[level]
        
        for resource, required in goals.items():
            current = inventory.get(resource, 0)
            if current < required:
                needed[resource] = required - current
        
        return needed
    
    def prioritize_resources(self, needed):
        # Prioriser les ressources rares
        rarity = {
            'thystame': 7,
            'phiras': 6,
            'mendiane': 5,
            'sibur': 4,
            'deraumere': 3,
            'linemate': 2,
            'food': 1
        }
        
        return sorted(needed.keys(), 
                     key=lambda x: rarity.get(x, 0), 
                     reverse=True)
```

### 3. Stratégie d'élévation

```python
class ElevationStrategy:
    def __init__(self):
        self.elevation_requirements = {
            1: {'players': 1, 'resources': {...}},
            2: {'players': 2, 'resources': {...}},
            # ... etc
        }
    
    def can_initiate_elevation(self, level, tile_content):
        req = self.elevation_requirements[level]
        
        # Vérifier les ressources
        for resource, count in req['resources'].items():
            if tile_content.get(resource, 0) < count:
                return False
        
        # Vérifier les joueurs (approximatif)
        player_count = tile_content.get('player', '').count('player')
        return player_count >= req['players']
    
    def coordinate_elevation(self, level):
        # Broadcasting pour rassembler les joueurs
        message = f"ELEVATION_L{level}_AT_{self.position}"
        return f"Broadcast {message}"
```

## Système de mémoire

### World Model

```python
class WorldModel:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.tiles = {}  # Dictionnaire sparse
        self.last_update = {}
        self.resources_seen = defaultdict(list)
        
    def update_from_look(self, look_data, position, orientation):
        # Parser et mettre à jour les tiles visibles
        # Calculer les positions absolues
        # Marquer le timestamp
        pass
    
    def find_nearest_resource(self, resource, from_pos):
        # Utiliser les données mémorisées
        # Prendre en compte l'âge de l'information
        candidates = self.resources_seen[resource]
        
        # Filtrer par âge
        current_time = time.time()
        valid = [pos for pos in candidates 
                if current_time - self.last_update[pos] < 60]
        
        # Trouver le plus proche
        return min(valid, key=lambda p: self.distance(from_pos, p))
```

### Team Coordination

```python
class TeamCoordinator:
    def __init__(self, team_name):
        self.team_name = team_name
        self.team_members = {}
        self.ongoing_elevations = {}
        self.message_history = deque(maxlen=100)
        
    def parse_broadcast(self, direction, message):
        # Format: "TEAM:ACTION:DATA"
        parts = message.split(':')
        
        if len(parts) >= 3 and parts[0] == self.team_name:
            action = parts[1]
            data = ':'.join(parts[2:])
            
            if action == "ELEVATION":
                self.handle_elevation_call(direction, data)
            elif action == "HELP":
                self.handle_help_request(direction, data)
            # ... etc
    
    def broadcast_team_message(self, action, data):
        message = f"{self.team_name}:{action}:{data}"
        return f"Broadcast {message}"
```

## Algorithmes importants

### 1. Pathfinding toroïdal

```python
def toroidal_distance(pos1, pos2, width, height):
    dx = abs(pos1[0] - pos2[0])
    dy = abs(pos1[1] - pos2[1])
    
    # Prendre en compte le wrap-around
    dx = min(dx, width - dx)
    dy = min(dy, height - dy)
    
    return dx + dy  # Distance Manhattan

def find_path(start, goal, world_model):
    # A* adapté pour monde toroïdal
    # Utiliser toroidal_distance comme heuristique
    pass
```

### 2. Interprétation de la vision

```python
def parse_look_response(response):
    # Format: [tile0, tile1, tile2, ...]
    tiles = response.strip()[1:-1].split(',')
    
    # Mapping des positions relatives par niveau
    level_positions = {
        1: [(0,0), (0,-1), (1,-1), (-1,-1)],
        2: [(0,-2), (1,-2), (2,-2), (-1,-2), (-2,-2), ...],
        # ... etc
    }
    
    parsed = {}
    for i, tile_content in enumerate(tiles):
        if i < len(level_positions[current_level]):
            rel_pos = level_positions[current_level][i]
            parsed[rel_pos] = parse_tile_content(tile_content)
    
    return parsed
```

### 3. Décision multi-critères

```python
class DecisionMaker:
    def __init__(self):
        self.weights = {
            'survival': 0.4,
            'progression': 0.3,
            'team': 0.2,
            'exploration': 0.1
        }
    
    def evaluate_action(self, action, context):
        scores = {
            'survival': self.survival_score(action, context),
            'progression': self.progression_score(action, context),
            'team': self.team_score(action, context),
            'exploration': self.exploration_score(action, context)
        }
        
        total = sum(scores[k] * self.weights[k] for k in scores)
        return total
    
    def choose_best_action(self, possible_actions, context):
        return max(possible_actions, 
                  key=lambda a: self.evaluate_action(a, context))
```

## Communication d'équipe

### Protocole de messages

```
Format: TEAM:TYPE:DATA

Types:
- ELEVATION:L{level}:X{x}:Y{y}     # Appel pour élévation
- NEED:RESOURCE:{resource}          # Besoin de ressource
- HAVE:RESOURCE:{resource}:X:Y      # Ressource disponible
- DANGER:X:Y                        # Avertissement
- READY:L{level}                    # Prêt pour élévation
```

### Stratégies de communication

1. **Broadcast économe**
   - Limiter les broadcasts (coût: 7 time units)
   - Grouper les informations
   - Utiliser des codes courts

2. **Réponse intelligente**
   - Calculer si ça vaut le coup de répondre
   - Prendre en compte la distance
   - Éviter les duplications

## Optimisations et heuristiques

### 1. Exploration efficace

```python
class ExplorationStrategy:
    def __init__(self):
        self.visited_tiles = set()
        self.exploration_pattern = "spiral"  # ou "random", "systematic"
        
    def next_exploration_target(self, current_pos):
        if self.exploration_pattern == "spiral":
            return self.spiral_next(current_pos)
        # ... autres patterns
```

### 2. Cache de décisions

```python
class DecisionCache:
    def __init__(self, ttl=5):
        self.cache = {}
        self.timestamps = {}
        self.ttl = ttl
    
    def get_or_compute(self, key, compute_func):
        now = time.time()
        if key in self.cache:
            if now - self.timestamps[key] < self.ttl:
                return self.cache[key]
        
        result = compute_func()
        self.cache[key] = result
        self.timestamps[key] = now
        return result
```

### 3. Anticipation

```python
class Anticipator:
    def predict_resource_depletion(self, tile, resource):
        # Basé sur l'activité observée
        # Prédire quand une ressource sera épuisée
        pass
    
    def predict_player_level(self, player_activity):
        # Estimer le niveau des autres joueurs
        # Basé sur leurs actions observées
        pass
```

## Gestion des erreurs et robustesse

### 1. Reconnexion automatique

```python
class RobustClient:
    def __init__(self):
        self.max_retries = 5
        self.retry_delay = 1
        
    def connect_with_retry(self):
        for attempt in range(self.max_retries):
            try:
                self.connect()
                return True
            except ConnectionError:
                time.sleep(self.retry_delay * (2 ** attempt))
        return False
```

### 2. Recovery après erreur

```python
class StateRecovery:
    def recover_from_unknown_state(self):
        # Séquence de recovery
        commands = [
            "Inventory",  # Vérifier l'inventaire
            "Look",       # Observer l'environnement
            "Connect_nbr" # Vérifier les slots équipe
        ]
        # Reconstruire l'état à partir des réponses
```

## Configuration et tuning

### Fichier de configuration

```yaml
# config.yaml
network:
  host: localhost
  port: 4242
  timeout: 5
  buffer_size: 4096

team:
  name: "team1"
  coordination: true
  broadcast_prefix: "T1"

behavior:
  food_threshold: 50
  exploration_radius: 10
  elevation_timeout: 30

performance:
  decision_cache_ttl: 5
  pathfinding_max_iterations: 1000
  memory_cleanup_interval: 60

debug:
  log_level: INFO
  log_commands: true
  save_worldmap: true
```

### Paramètres ajustables

```python
class AIParameters:
    # Survie
    FOOD_CRITICAL = 10
    FOOD_SAFE = 100
    
    # Exploration
    EXPLORATION_RADIUS = 15
    REVISIT_DELAY = 60
    
    # Élévation
    ELEVATION_GATHER_TIMEOUT = 30
    ELEVATION_RETRY_DELAY = 10
    
    # Performance
    MAX_COMMANDS_QUEUED = 5
    THINK_TIME = 0.1  # Secondes entre décisions
```

## Tests et debugging

### 1. Mode debug

```python
class DebugAI:
    def __init__(self, ai):
        self.ai = ai
        self.command_log = []
        self.decision_log = []
        
    def log_decision(self, state, options, chosen):
        self.decision_log.append({
            'timestamp': time.time(),
            'state': state,
            'options': options,
            'chosen': chosen,
            'reason': self.ai.last_decision_reason
        })
```

### 2. Visualisation

```python
class WorldMapVisualizer:
    def save_worldmap(self, world_model, filename):
        # Sauvegarder une représentation ASCII
        # ou générer une image de la carte connue
        pass
    
    def print_status(self, ai_state):
        print(f"""
        === AI Status ===
        State: {ai_state.current_state}
        Level: {ai_state.level}
        Food: {ai_state.inventory['food']} ({ai_state.food_time}s)
        Position: {ai_state.position}
        Team members nearby: {ai_state.team_nearby}
        """)
```

### 3. Métriques

```python
class AIMetrics:
    def __init__(self):
        self.start_time = time.time()
        self.commands_sent = 0
        self.elevation_attempts = 0
        self.deaths = 0
        self.resources_collected = defaultdict(int)
        
    def report(self):
        runtime = time.time() - self.start_time
        return {
            'runtime': runtime,
            'commands_per_second': self.commands_sent / runtime,
            'elevation_success_rate': self.calculate_success_rate(),
            'resources_per_minute': self.calculate_collection_rate()
        }
```

## Stratégies avancées

### 1. Formation d'alliances

- Identifier les joueurs fiables
- Créer des sous-groupes pour élévations
- Partager les ressources stratégiquement

### 2. Économie de ressources

- Tracker qui possède quoi dans l'équipe
- Système de "banque" centralisée
- Échanges coordonnés

### 3. Défense territoriale

- Identifier les zones riches
- Éjecter les intrus
- Patrouilles coordonnées

## Checklist de développement

- [ ] Client réseau de base
- [ ] Parser de protocole complet
- [ ] Machine à états
- [ ] Stratégie de survie
- [ ] Système de vision et parsing
- [ ] Mémoire du monde
- [ ] Pathfinding toroïdal
- [ ] Stratégie de collecte
- [ ] Communication d'équipe
- [ ] Stratégie d'élévation
- [ ] Gestion des erreurs
- [ ] Optimisations (cache, etc.)
- [ ] Mode debug et logs
- [ ] Configuration externe
- [ ] Tests unitaires
- [ ] Documentation