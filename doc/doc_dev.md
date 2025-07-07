# 🌟 Zappy - Documentation Développeur

## Table des matières
1. [Présentation du projet](#présentation-du-projet)
2. [Architecture globale](#architecture-globale)
3. [Structure du projet](#structure-du-projet)
4. [Workflow de développement](#workflow-de-développement)
5. [Compilation et build](#compilation-et-build)
6. [Serveur](#serveur)
7. [Client IA](#client-ia)
8. [Interface Graphique (GUI)](#interface-graphique-gui)
9. [Protocole de communication](#protocole-de-communication)
10. [Tests et débogage](#tests-et-débogage)
11. [Optimisation et performance](#optimisation-et-performance)
12. [Bonnes pratiques](#bonnes-pratiques)
13. [Ressources et références](#ressources-et-références)
14. [Auteurs](#auteurs)

## Présentation du projet

Zappy est un projet d'intelligence artificielle distribuée simulant un écosystème où plusieurs équipes d'IA s'affrontent pour survivre et évoluer. Le projet est composé de trois composants principaux :

### Composants du système
- **Serveur** (C) : Gère la simulation, la logique de jeu et les communications
- **Client IA** (Python) : Implémente les stratégies d'intelligence artificielle
- **Interface Graphique** (C++/SFML/Raylib) : Visualise la simulation en temps réel

### Fonctionnalités principales
- Simulation d'un monde en grille avec ressources distribuées
- Système de niveaux d'évolution (1-8) avec rituels d'élévation
- Communication inter-IA par broadcast
- Gestion de la faim et de la survie
- Interface 2D/3D temps réel avec animations
- Architecture réseau scalable

## Architecture globale

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   GUI Client    │    │   AI Clients    │    │   GUI Client    │
│   (C++/SFML)    │    │   (Python)      │    │   (C++/Raylib)  │
└─────────┬───────┘    └─────────┬───────┘    └─────────┬───────┘
          │                      │                      │
          │         ┌────────────┼────────────┐         │
          │         │            │            │         │
          └─────────┤       ZAPPY SERVER      ├─────────┘
                    │       (C Network)       │
                    │                         │
                    │   ┌─────────────────┐   │
                    │   │   Game Engine   │   │
                    │   │   - Map System  │   │
                    │   │   - Players     │   │
                    │   │   - Resources   │   │
                    │   │   - Events      │   │
                    │   └─────────────────┘   │
                    └─────────────────────────┘
```

### Flux de communication
1. **AI → Server** : Commandes de jeu (Forward, Take, Broadcast, etc.)
2. **Server → AI** : Réponses et état du jeu
3. **GUI → Server** : Commandes de monitoring (msz, bct, tna, etc.)
4. **Server → GUI** : État complet du monde et événements

### Patterns architecturaux utilisés
- **Command Pattern** : Pour la gestion des commandes AI
- **Observer Pattern** : Pour les événements GUI
- **Factory Pattern** : Pour la création d'objets de jeu
- **Strategy Pattern** : Pour les différentes stratégies d'IA

## Structure du projet

```
zappy/
├── Makefile                   # Build configuration global
├── server/                    # Serveur de jeu (C)
│   ├── include/
│   │   └── server.h           # Headers principaux
│   ├── core/                  # Logique serveur principale
│   │   ├── server.c           # Boucle principale
│   │   ├── config.c           # Parsing arguments
│   │   └── server_*.c         # Modules serveur
│   ├── network/               # Gestion réseau
│   │   ├── socket.c           # Sockets TCP
│   │   ├── client.c           # Gestion clients
│   │   └── buffer.c           # Buffers circulaires
│   ├── world/                 # Monde de jeu
│   │   ├── map.c              # Carte et tuiles
│   │   └── map_*.c            # Utilitaires carte
│   ├── game/                  # Logique de jeu
│   │   ├── player.c           # Joueurs
│   │   ├── team.c             # Équipes
│   │   └── egg.c              # Œufs
│   ├── commands/              # Commandes AI/GUI
│   │   ├── ai_commands.c      # Commandes IA
│   │   └── gui_commands.c     # Commandes GUI
│   └── engine/                # Moteur temporel
│       ├── time.c             # Timestamps
│       └── timeout.c          # Gestion timeouts
├── ai/                        # Client IA (Python)
│   ├── main.py                # Point d'entrée
│   ├── models.py              # Modèles de données
│   ├── network.py             # Communication réseau
│   ├── zappy_ai.py            # IA principale
│   ├── movement.py            # Gestion mouvement
│   ├── survival.py            # Stratégies survie
│   ├── elevation.py           # Logique élévation
│   ├── coordination.py        # Coordination équipe
│   ├── inventory.py           # Gestion inventaire
│   └── resources.py           # Collecte ressources
└── gui/                       # Interface graphique (C++)
    ├── main.cpp               # Point d'entrée
    ├── include/               # Headers C++
    │   ├── Application.hpp    # Application principale
    │   ├── Config.hpp         # Configuration
    │   ├── Common.hpp         # Types communs
    │   ├── model/             # Modèles données
    │   ├── view/              # Rendu et affichage
    │   ├── ui/                # Interface utilisateur
    │   ├── network/           # Client réseau
    │   ├── events/            # Système événements
    │   └── utils/             # Utilitaires
    └── src/                   # Implémentations C++
        ├── core/
        ├── model/
        ├── view/
        ├── ui/
        ├── network/
        └── utils/
```

## Workflow de développement

### 1. Setup environnement

```bash
# Cloner le projet
git clone <repository_url>
cd zappy

# Vérifier les dépendances
# Serveur : gcc, make
# IA : python3
# GUI : g++, SFML, Raylib (optionnel)

# Build complet
make
```

### 2. Branches et Git Flow

```bash
# Créer une branche feature
git checkout -b feature/nom-fonctionnalite

# Développement avec commits atomiques
git add -p  # Ajout sélectif
git commit -m "feat(server): ajouter gestion timeout events"

# Push et pull request
git push origin feature/nom-fonctionnalite
```

### 3. Tests locaux

```bash
# Test serveur seul
./zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 5 -f 100

# Test avec IA
./zappy_ai -p 4242 -n team1 -h localhost

# Test avec GUI
./zappy_gui -p 4242 -h localhost
```

## Compilation et build

### Makefile principal

Le Makefile gère trois composants :

```makefile
# Targets principaux
all: zappy_ai zappy_server zappy_gui

# Serveur C
zappy_server: $(OBJ_SERVER)
    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Client IA Python
zappy_ai:
    # Génère un script wrapper pour Python

# GUI C++
zappy_gui: $(OBJ_GUI)
    $(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS) $(RAYLIB_LIBS)
```

### Options de compilation

```bash
# Debug mode
make DEBUG=1

# Mode release optimisé
make RELEASE=1

# Compilation avec Raylib (3D)
make RAYLIB_AVAILABLE=1

# Nettoyage
make clean    # Objets seulement
make fclean   # Tout nettoyer
make re       # Recompilation complète
```

### Dépendances

**Serveur :**
- gcc/clang
- libc standard
- POSIX sockets

**IA :**
- Python 3.6+
- Modules standard uniquement

**GUI :**
- g++/clang++ (C++17)
- SFML 2.5+ (graphics, window, system, network, audio)
- Raylib 3.0+ (optionnel, pour le mode 3D)

## Serveur

### Architecture serveur

Le serveur utilise une architecture événementielle basée sur `poll()` :

```c
// Boucle principale
void server_run(server_t *server) {
    while (server->running) {
        // 1. Calculer timeout pour prochains événements
        timeout = calculate_timeout(server->events, now);
        
        // 2. Poll des sockets
        ret = poll(server->poll_fds, server->poll_count, timeout);
        
        // 3. Traiter nouvelles connexions
        if (server->poll_fds[0].revents & POLLIN)
            handle_new_connection(server);
            
        // 4. Traiter données clients
        for (size_t i = 1; i < server->poll_count; i++)
            handle_client_events(server, &i);
            
        // 5. Exécuter événements programmés
        server_process_events(server);
    }
}
```

### Système d'événements temporels

```c
// Ajouter événement futur
uint64_t execute_time = get_timestamp() + (7000 / server->config.freq);
event_queue_add(server->events, execute_time, client->fd, "forward_exec");

// Exécution différée
event_t *event = event_queue_get_ready(server->events, now);
if (event) {
    handle_ai_command(server, client, event->command);
    event_queue_remove(server->events, event);
}
```

### Gestion mémoire

- **Buffers circulaires** pour I/O réseau
- **Listes chaînées** pour objets dynamiques (joueurs, ressources)
- **Grilles statiques** pour la carte
- **Pools d'objets** pour événements

### API Serveur principales

```c
// Initialisation
bool server_init(server_t *server, config_t *config);
void server_cleanup(server_t *server);

// Gestion clients
void server_add_client(server_t *server, client_t *client);
void server_remove_client(server_t *server, client_t *client);

// Commandes IA
void handle_ai_command(server_t *server, client_t *client, const char *command);

// Commandes GUI
void handle_gui_command(server_t *server, client_t *client, const char *command);

// Monde de jeu
map_t *map_create(int width, int height);
bool map_add_resource(map_t *map, int x, int y, resource_type_t type);
player_t *player_create(int id, int x, int y, int team_id);
```

## Client IA

### Architecture IA

L'IA utilise une architecture modulaire avec état :

```python
class ZappyAI:
    def __init__(self, config: GameConfig):
        self.config = config
        self.player = None
        
    def run_intelligent_strategy(self):
        # Stratégie adaptative par niveau
        if self.player.level == 1:
            self._level_1_strategy()
        elif self.player.level == 2:
            self._level_2_strategy()
        else:
            self._higher_level_strategy()
```

### Système de stratégies

**Niveau 1 : Survie et collecte basique**
```python
def _level_1_strategy(self):
    # Priorité absolue : nourriture
    if SurvivalManager.needs_food_critically(self.player):
        SurvivalManager.gather_food(self.player)
        return
        
    # Collecter linemate pour level 2
    if not InventoryManager.has_linemate(self.player):
        ResourceManager.collect_resource(self.player, "linemate")
    else:
        ElevationManager.attempt_elevation(self.player, 2)
```

**Niveaux supérieurs : Coordination**
```python
def _level_2_strategy(self):
    # Gestion proactive de la nourriture
    if SurvivalManager.needs_food_soon(self.player):
        SurvivalManager.gather_food(self.player)
        
    # Collecte multi-ressources
    missing = ElevationManager.get_missing_resources(self.player, 3)
    if missing:
        ResourceManager.collect_resource(self.player, missing[0])
        
    # Communication d'équipe
    if self.player.is_leader:
        CoordinationManager.broadcast_status(self.player)
```

### Modules IA

**SurvivalManager** : Gestion de la faim
- `needs_food_critically()` : < 2 food
- `gather_food_aggressively()` : Collecte intensive
- `should_stockpile_food()` : Stratégie de stockage

**MovementManager** : Déplacements intelligents
- `navigate_to_tile()` : Navigation optimale
- `explore_pattern()` : Patterns d'exploration
- `avoid_obstacles()` : Évitement

**CoordinationManager** : Communication équipe
- `smart_broadcast()` : Messages intelligents
- `listen_and_respond()` : Écoute active
- `coordinate_elevation()` : Rituels groupés

### Configuration réseau

```python
class NetworkManager:
    @staticmethod
    def create_connection(config: GameConfig, team_name: str) -> Player:
        # Connexion socket avec timeout
        # Handshake protocol
        # Parsing réponse serveur
        
    @staticmethod
    def send_command(player: Player, command: str) -> str:
        # Envoi commande avec gestion erreurs
        # Parsing réponse
        # Mise à jour état joueur
```

## Interface Graphique (GUI)

### Architecture GUI

Le GUI utilise une architecture MVC avec système d'événements :

```cpp
// Application principale
class Application {
    std::unique_ptr<World> world_;           // Modèle
    std::unique_ptr<IRenderer> renderer_;    // Vue
    std::unique_ptr<UIManager> ui_;          // Contrôleur
    std::unique_ptr<NetworkClient> network_; // Réseau
    std::unique_ptr<EventDispatcher> events_; // Événements
};
```

### Système de rendu

**Mode 2D (SFML)**
```cpp
class Renderer : public IRenderer {
    void render(const World& world, const Camera& camera, float deltaTime) {
        renderTiles(world, camera);           // Terrain
        renderResources(world, camera);       // Ressources
        renderEntities(world, camera, deltaTime); // Joueurs/Œufs
        renderEffects(camera, deltaTime);     // Particules/Animations
        renderUI();                          // Interface
    }
};
```

**Mode 3D (Raylib)**
```cpp
class Renderer3D : public IRenderer {
    void render(const World& world, const Camera& camera, float deltaTime) {
        BeginMode3D(camera3D_);
        renderTerrain(world);     // Terrain 3D
        renderPlayers(world, deltaTime); // Modèles 3D
        renderResources(world);   // Cristaux/Nourriture
        EndMode3D();
        
        renderUI(world);         // HUD 2D
    }
};
```

### Système d'événements

```cpp
class EventDispatcher {
    template<typename T>
    void subscribe(std::function<void(const T&)> listener);
    
    void post(const Event& event);
};

// Utilisation
dispatcher_.subscribe<PlayerAddedEvent>([this](const PlayerAddedEvent& e) {
    this->onPlayerAdded(e);
});
```

### Interface utilisateur

**Panneaux d'information**
- `PlayerInfoPanel` : Détails joueur (niveau, inventaire)
- `TileInfoPanel` : Contenu tuile
- `TeamPanel` : Statistiques équipes
- `DebugPanel` : Métriques performance

**HUD temps réel**
- FPS counter
- Temps de jeu
- Compteurs ressources globaux
- Log d'événements

### Configuration GUI

```cpp
class Config {
    RenderMode renderMode = RenderMode::Mode2D; // 2D ou 3D
    int windowWidth = 1280;
    int windowHeight = 720;
    bool vsync = true;
    int fpsLimit = 60;
    bool showFPS = true;
    bool showGrid = true;
};
```

## Protocole de communication

### Commandes IA → Serveur

```
// Mouvement
Forward          → ok/ko
Right           → ok/ko  
Left            → ok/ko

// Information
Look            → [tile_content,tile_content,...]
Inventory       → [food n,linemate n,deraumere n,...]
Connect_nbr     → n

// Actions
Take object     → ok/ko
Set object      → ok/ko
Incantation     → "Elevation underway" puis "Current level: n"
Broadcast text  → ok/ko
Fork            → ok/ko
Eject           → ok/ko
```

### Commandes GUI → Serveur

```
// Carte
msz             → msz width height
bct x y         → bct x y food linemate deraumere sibur mendiane phiras thystame
mct             → bct pour toutes les tuiles

// Équipes
tna             → tna name (pour chaque équipe)

// Joueurs
ppo #id         → ppo #id x y orientation
plv #id         → plv #id level
pin #id         → pin #id x y food linemate deraumere sibur mendiane phiras thystame

// Serveur
sgt             → sgt freq
sst freq        → sgt new_freq
```

### Messages serveur → GUI

```
// Événements joueurs
pnw #id x y o level team → Nouveau joueur
ppo #id x y o           → Position joueur  
plv #id level           → Niveau joueur
pin #id x y resources   → Inventaire joueur
pdi #id                 → Mort joueur
pbc #id message         → Broadcast joueur

// Événements élévation
pic x y level #ids      → Début incantation
pie x y result          → Fin incantation

// Événements œufs
enw #id #player x y     → Nouvel œuf
ebo #id                 → Éclosion œuf
edi #id                 → Mort œuf

// Événements objets
pdr #id resource        → Objet posé
pgt #id resource        → Objet pris

// Fin de jeu
seg team_name           → Victoire équipe
```

## Tests et débogage

### Tests serveur

```bash
# Test basic avec traces
./zappy_server -p 4242 -x 5 -y 5 -n team1 -c 2 -f 10 2>&1 | tee server.log

# Test charge multiple IA
for i in {1..5}; do
    ./zappy_ai -p 4242 -n team1 -h localhost &
done

# Test GUI simultané
./zappy_gui -p 4242 -h localhost --debug
```

### Debugging IA

```python
# Mode debug dans l'IA
import logging
logging.basicConfig(level=logging.DEBUG)

# Traces réseau
LOG_DEBUG(f"Sending command: {command}")
LOG_DEBUG(f"Received response: {response}")

# Métriques performance
def profile_function(func):
    start_time = time.time()
    result = func()
    LOG_DEBUG(f"{func.__name__} took {time.time() - start_time:.3f}s")
    return result
```

### Debugging GUI

```cpp
// Panel debug avec F3
void DebugPanel::render(sf::RenderTarget& target) {
    if (!visible_) return;
    
    renderPerformanceSection(target);  // FPS, frame time
    renderEntitySection(target);       // Nombre entités
    renderNetworkSection(target);      // Latence, messages/s
    renderMemorySection(target);       // Utilisation mémoire
}

// Logs avec niveaux
LOG_DEBUG("Camera position updated");
LOG_WARNING("High frame time detected");
LOG_ERROR("Failed to load texture");
```

### Outils de debugging

**Serveur :**
- `gdb` pour debugging C
- `valgrind` pour fuites mémoire
- `strace` pour appels système
- `netstat` pour connexions réseau

**IA :**
- `pdb` pour debugging Python
- `profile`/`cProfile` pour performance
- `memory_profiler` pour mémoire

**GUI :**
- `gdb` pour debugging C++
- Intel Graphics Performance Analyzers
- RenderDoc pour debugging graphique

## Optimisation et performance

### Serveur - Optimisations réseau

```c
// Buffers circulaires pour éviter copies
typedef struct circular_buffer_s {
    char *data;
    size_t size;
    size_t read_pos;
    size_t write_pos;
    bool full;
} circular_buffer_t;

// Polling non-bloquant
int poll_timeout = calculate_timeout(server->events, now);
poll(server->poll_fds, server->poll_count, poll_timeout);

// Queue d'événements pour batch processing
event_queue_add(server->events, execute_time, client_fd, command);
```

### IA - Optimisations algorithmiques

```python
# Cache des calculs coûteux
@lru_cache(maxsize=128)
def calculate_optimal_path(start, target, obstacles):
    # A* ou Dijkstra avec cache
    
# Éviter recalculs constants
def update_game_state(self, deltaTime):
    if deltaTime < self.last_update + 0.1:  # Throttling
        return
    self.last_update = deltaTime
    
# Patterns de mouvement pré-calculés
EXPLORATION_PATTERNS = {
    'spiral': [(0,1), (1,0), (0,-1), (-1,0)],
    'random_walk': precompute_random_moves(100),
}
```

### GUI - Optimisations graphiques

```cpp
// Culling de frustum
sf::FloatRect viewBounds = camera.getViewBounds();
for (const auto& entity : entities) {
    if (!viewBounds.intersects(entity.getBounds()))
        continue;  // Skip invisible entities
    renderEntity(entity);
}

// Batching sprites
void SpriteManager::beginBatch();
for (const auto& sprite : sprites) {
    spriteManager->addSprite(sprite);
}
spriteManager->endBatch(target);

// LOD (Level of Detail)
float distance = math::distance(camera.getPosition(), entity.getPosition());
int lodLevel = distance < 100 ? 0 : distance < 500 ? 1 : 2;
renderEntity(entity, lodLevel);
```

### Métriques de performance

**Serveur :**
- Messages/seconde traités
- Latence moyenne commandes
- Utilisation CPU/mémoire
- Connexions simultanées

**IA :**
- Temps de décision moyen
- Commandes/seconde envoyées
- Taux de survie
- Efficacité collecte ressources

**GUI :**
- FPS maintenu
- Temps de rendu par frame
- Entités affichées simultanément
- Utilisation GPU

## Bonnes pratiques

### Code C (Serveur)

```c
// Conventions nommage
typedef struct player_s player_t;     // Types avec _t
bool player_move_forward(player_t *player, map_t *map);  // snake_case

// Gestion erreurs systématique
bool map_add_resource(map_t *map, int x, int y, resource_type_t type) {
    if (!map || type < 0 || type >= RESOURCE_TYPES)
        return false;
    // ...
    return true;
}

// Libération mémoire
void player_destroy(player_t *player) {
    if (!player) return;
    // Nettoyer ressources
    free(player);
}

// Constantes nommées
#define MAX_CLIENTS 64
#define BUFFER_SIZE 8192
#define RESOURCE_TYPES 7
```

### Code Python (IA)

```python
# Type hints systématiques
def move_to_resource(player: Player, resource_type: str) -> bool:
    """Move player towards the nearest resource of specified type."""
    
# Gestion exceptions
try:
    response = network.send_command(command)
except NetworkError as e:
    LOG_ERROR(f"Network error: {e}")
    return False
    
# Docstrings claires
class SurvivalManager:
    """Manages player survival strategies including food gathering."""
    
    @staticmethod
    def needs_food_critically(player: Player) -> bool:
        """Returns True if player has less than 2 food units."""
        return player.inventory.get('food', 0) < 2
```

### Code C++ (GUI)

```cpp
// RAII et smart pointers
class Application {
private:
    std::unique_ptr<NetworkClient> network_;
    std::unique_ptr<World> world_;
    std::unique_ptr<UIManager> ui_;
};

// Const correctness
const sf::Texture& ResourceManager::getTexture(const std::string& name) const;

// Exception safety
try {
    auto player = std::make_unique<Player>(id, x, y);
    world_->addPlayer(std::move(player));
} catch (const std::exception& e) {
    LOG_ERROR("Failed to create player: " + std::string(e.what()));
}

// Move semantics
Player(Player&& other) noexcept 
    : id_(other.id_), position_(std::move(other.position_)) {}
```

### Architecture générale

**Séparation des responsabilités**
- Logique métier ≠ Présentation
- Réseau ≠ Logique de jeu  
- Modèle ≠ Vue ≠ Contrôleur

**Interfaces stables**
```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render(const World& world, const Camera& camera, float deltaTime) = 0;
    virtual void resize(unsigned int width, unsigned int height) = 0;
};
```

**Configuration externalisée**
```bash
# Paramètres runtime
./zappy_server -p 4242 -x 20 -y 20 -n team1 team2 -c 10 -f 100

# Fichiers de config
{
    "ai_strategy": "aggressive",
    "coordination_enabled": true,
    "debug_level": "info"
}
```

## Ressources et références

### Documentation technique
- **RFC protocole Zappy** : Spécification complète des messages
- **SFML Documentation** : https://www.sfml-dev.org/documentation/
- **Raylib Documentation** : https://www.raylib.com/
- **Python Socket Programming** : Guide réseau Python

---

### Auteurs

- Raphaël Chanliongco
- Hubert Touraine  
- Jean-Baptiste Boshra  
- Hugo Dufour
- Alexandre de Angelis

---

*Documentation réalisée dans le cadre du projet EPITECH Zappy – Juin 2025.*