# Guide de Développement Zappy

## Setup de l'environnement

### Prérequis

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    valgrind \
    gdb \
    libsfml-dev \
    python3-pip \
    git \
    doxygen \
    graphviz

# Python pour l'AI
pip3 install pyyaml numpy

# macOS
brew install cmake sfml python3 doxygen graphviz
```

### Structure initiale

```bash
# Cloner et initialiser
git clone <repo>
cd zappy
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Standards de code

### C (Serveur)

#### Style de code

```c
// Fichier: player.c

// Headers système d'abord
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Puis headers locaux
#include "player.h"
#include "world.h"

// Constantes en majuscules
#define MAX_INVENTORY_SIZE 7
#define PLAYER_VISION_BASE 1

// Types et structures
typedef struct player_s {
    int id;
    position_t pos;
    orientation_t orientation;
    int level;
    inventory_t inventory;
    team_t *team;
    bool is_frozen;  // Pour incantation
} player_t;

// Fonctions avec préfixe du module
player_t *player_create(int id, team_t *team) {
    player_t *player = calloc(1, sizeof(player_t));
    if (!player) {
        log_error("Failed to allocate player");
        return NULL;
    }
    
    player->id = id;
    player->team = team;
    player->level = 1;
    player->orientation = NORTH;
    
    // Initialiser avec 10 food
    player->inventory.food = 10;
    
    return player;
}

// Toujours vérifier les paramètres
error_code_t player_move_forward(player_t *player) {
    if (!player) {
        return ERR_INVALID_PARAM;
    }
    
    // Logique de déplacement
    position_t new_pos = calculate_forward_position(
        player->pos, 
        player->orientation
    );
    
    // Vérifier validité
    if (!world_is_valid_position(new_pos)) {
        return ERR_INVALID_POSITION;
    }
    
    player->pos = new_pos;
    return ERR_NONE;
}

// Cleanup systématique
void player_destroy(player_t *player) {
    if (!player) return;
    
    // Libérer ressources allouées
    // ...
    
    free(player);
}
```

#### Conventions de nommage

- **Fonctions**: `module_action_object()` (ex: `player_take_resource()`)
- **Variables**: `snake_case`
- **Constantes**: `UPPER_CASE`
- **Types**: `suffix_t` pour les typedef
- **Structures**: `suffix_s` pour les struct

### C++ (GUI)

#### Style de code

```cpp
// Fichier: WorldRenderer.cpp

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace zappy {
namespace gui {

// Classes en PascalCase
class WorldRenderer {
public:
    // Constructeur explicite
    explicit WorldRenderer(const RenderConfig& config);
    
    // Pas de destructeur si pas nécessaire (RAII)
    
    // Méthodes publiques
    void render(sf::RenderWindow& window, const World& world);
    void setCamera(std::unique_ptr<Camera> camera);
    
    // Getters const
    const Camera& getCamera() const { return *camera_; }
    
private:
    // Membres privés avec underscore
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<TileRenderer> tile_renderer_;
    RenderConfig config_;
    
    // Méthodes privées
    void renderTiles(sf::RenderWindow& window, const TileGrid& tiles);
    void renderEntities(sf::RenderWindow& window, const EntityList& entities);
    
    // Désactiver copie
    WorldRenderer(const WorldRenderer&) = delete;
    WorldRenderer& operator=(const WorldRenderer&) = delete;
};

} // namespace gui
} // namespace zappy
```

### Python (AI)

#### Style de code

```python
#!/usr/bin/env python3
"""
AI Client for Zappy game.

This module implements the autonomous AI behavior.
"""

from dataclasses import dataclass
from enum import Enum, auto
from typing import Dict, List, Optional, Tuple
import logging


# Configuration en haut du fichier
DEFAULT_CONFIG = {
    'host': 'localhost',
    'port': 4242,
    'timeout': 5.0
}


class AIState(Enum):
    """Possible states for the AI state machine."""
    IDLE = auto()
    GATHERING = auto()
    ELEVATING = auto()
    SURVIVING = auto()


@dataclass
class Position:
    """2D position on the map."""
    x: int
    y: int
    
    def distance_to(self, other: 'Position') -> int:
        """Calculate Manhattan distance."""
        return abs(self.x - other.x) + abs(self.y - other.y)


class ZappyAI:
    """Main AI client class."""
    
    def __init__(self, team_name: str, config: Optional[Dict] = None):
        """Initialize AI with team name and configuration."""
        self.team_name = team_name
        self.config = {**DEFAULT_CONFIG, **(config or {})}
        self.logger = logging.getLogger(f'AI.{team_name}')
        
        # State
        self._state = AIState.IDLE
        self._position = Position(0, 0)
        self._inventory: Dict[str, int] = {}
        
    def run(self) -> None:
        """Main AI loop."""
        try:
            self._connect()
            while self._is_alive():
                self._think()
                self._act()
        except KeyboardInterrupt:
            self.logger.info("Shutting down...")
        finally:
            self._cleanup()
    
    def _think(self) -> None:
        """Decision making logic."""
        # Analyser l'état actuel
        food_level = self._calculate_food_level()
        
        if food_level < CRITICAL_FOOD_THRESHOLD:
            self._state = AIState.SURVIVING
        elif self._can_elevate():
            self._state = AIState.ELEVATING
        else:
            self._state = AIState.GATHERING
```

## Organisation Git

### Structure des branches

```
main (stable)
│
├── develop (intégration)
│   │
│   ├── feature/server-core
│   ├── feature/game-logic
│   ├── feature/gui-client
│   ├── feature/ai-client
│   └── feature/network-protocol
│
├── hotfix/critical-bug
└── release/v1.0
```

### Workflow Git

#### 1. Créer une feature

```bash
# Depuis develop
git checkout develop
git pull origin develop
git checkout -b feature/my-feature

# Travailler...
git add .
git commit -m "feat: add player movement logic"
```

#### 2. Convention de commits

Format: `type(scope): description`

Types:
- `feat`: Nouvelle fonctionnalité
- `fix`: Correction de bug
- `docs`: Documentation
- `style`: Formatage (pas de changement de code)
- `refactor`: Refactoring
- `test`: Ajout de tests
- `chore`: Maintenance

Exemples:
```bash
git commit -m "feat(server): add circular buffer implementation"
git commit -m "fix(gui): correct player sprite orientation"
git commit -m "docs(ai): update strategy documentation"
git commit -m "test(network): add protocol parsing tests"
```

#### 3. Pull Request

Template PR:
```markdown
## Description
Brief description of changes

## Type of change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing done

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added where needed
- [ ] Documentation updated
- [ ] No warnings with -Wall -Wextra -Werror
```

### Hooks Git

#### Pre-commit hook

```bash
#!/bin/bash
# .git/hooks/pre-commit

# Check formatting
echo "Checking code format..."
find . -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror

# Run quick tests
echo "Running quick tests..."
make test-quick

# Check for debugging code
echo "Checking for debug code..."
git diff --cached | grep -E "(console\.log|printf.*DEBUG|TODO|FIXME)" && {
    echo "Debug code detected!"
    exit 1
}
```

## Tests

### Structure des tests

```
tests/
├── unit/
│   ├── server/
│   │   ├── test_buffer.c
│   │   ├── test_parser.c
│   │   └── test_game_logic.c
│   ├── gui/
│   │   └── test_renderer.cpp
│   └── ai/
│       └── test_strategies.py
│
├── integration/
│   ├── test_client_server.sh
│   ├── test_multiple_clients.sh
│   └── test_full_game.sh
│
├── performance/
│   ├── benchmark_server.c
│   └── stress_test.py
│
└── fixtures/
    ├── maps/
    └── configs/
```

### Tests unitaires (C)

```c
// test_buffer.c
#include "unity.h" // Framework de test léger
#include "buffer.h"

void setUp(void) {
    // Avant chaque test
}

void tearDown(void) {
    // Après chaque test
}

void test_circular_buffer_write_read(void) {
    circular_buffer_t *buf = buffer_create(1024);
    TEST_ASSERT_NOT_NULL(buf);
    
    const char *data = "Hello, World!\n";
    size_t written = buffer_write(buf, data, strlen(data));
    TEST_ASSERT_EQUAL(strlen(data), written);
    
    char output[1024];
    size_t read = buffer_read_line(buf, output, sizeof(output));
    TEST_ASSERT_EQUAL(strlen(data), read);
    TEST_ASSERT_EQUAL_STRING(data, output);
    
    buffer_destroy(buf);
}

void test_circular_buffer_overflow(void) {
    circular_buffer_t *buf = buffer_create(10);
    
    // Essayer d'écrire plus que la capacité
    const char *data = "This is too long for buffer";
    size_t written = buffer_write(buf, data, strlen(data));
    TEST_ASSERT_LESS_THAN(strlen(data), written);
    
    buffer_destroy(buf);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_circular_buffer_write_read);
    RUN_TEST(test_circular_buffer_overflow);
    return UNITY_END();
}
```

### Tests d'intégration

```bash
#!/bin/bash
# test_elevation.sh

# Start server
./zappy_server -p 4242 -x 10 -y 10 -n team1 -c 6 -f 100 &
SERVER_PID=$!
sleep 1

# Start multiple AI clients
for i in {1..4}; do
    ./zappy_ai -p 4242 -n team1 &
    AI_PIDS+=($!)
done

# Start GUI
./zappy_gui -p 4242 -h localhost &
GUI_PID=$!

# Monitor for elevation
timeout 60 bash -c '
while true; do
    if grep -q "Current level: 8" server.log; then
        echo "SUCCESS: Player reached level 8!"
        exit 0
    fi
    sleep 1
done'

RESULT=$?

# Cleanup
kill $SERVER_PID ${AI_PIDS[@]} $GUI_PID 2>/dev/null

exit $RESULT
```

### Coverage

```makefile
# Dans le Makefile
coverage:
	@echo "Running coverage analysis..."
	@$(MAKE) clean
	@$(MAKE) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage"
	@$(MAKE) test
	@lcov --capture --directory . --output-file coverage.info
	@lcov --remove coverage.info '/usr/*' --output-file coverage.info
	@lcov --list coverage.info
	@genhtml coverage.info --output-directory coverage_html
	@echo "Coverage report: coverage_html/index.html"
```

## CI/CD

### GitHub Actions

```yaml
# .github/workflows/ci.yml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake valgrind libsfml-dev
    
    - name: Build
      run: |
        mkdir build
        cd build
        cmake ..
        make -j$(nproc)
    
    - name: Run unit tests
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Memory check
      run: |
        cd build
        valgrind --leak-check=full --error-exitcode=1 ./tests/test_server
    
    - name: Lint
      run: |
        find . -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror
    
    - name: Coverage
      run: |
        cd build
        make coverage
    
    - name: Upload coverage
      uses: codecov/codecov-action@v3
      with:
        file: ./build/coverage.info
```

## Debugging

### GDB pour le serveur

```bash
# Compiler avec symboles debug
make DEBUG=1

# Lancer avec GDB
gdb ./zappy_server
(gdb) set args -p 4242 -x 10 -y 10 -n team1 -c 6 -f 100
(gdb) break handle_client_command
(gdb) run

# Commandes utiles
(gdb) bt              # Backtrace
(gdb) info locals     # Variables locales
(gdb) print *client   # Afficher structure
(gdb) watch player->level  # Watchpoint
```

### Valgrind pour les fuites

```bash
# Fuites mémoire
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes --verbose \
         ./zappy_server -p 4242 -x 10 -y 10 -n team1 -c 1 -f 100

# Profiling avec callgrind
valgrind --tool=callgrind ./zappy_server [args]
kcachegrind callgrind.out.*
```

### Debug réseau

```bash
# Surveiller le trafic
sudo tcpdump -i lo -A 'tcp port 4242'

# Ou avec Wireshark
sudo wireshark -k -i lo -f 'tcp port 4242'

# Simuler latence/perte
sudo tc qdisc add dev lo root netem delay 100ms loss 5%
```

## Performance

### Profiling CPU

```c
// Micro-benchmarks
#include <time.h>

double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

void benchmark_function() {
    double start = get_time_ms();
    
    // Code à mesurer
    for (int i = 0; i < 1000000; i++) {
        parse_command("Forward\n");
    }
    
    double end = get_time_ms();
    printf("Time: %.2f ms\n", end - start);
}
```

### Optimisations communes

1. **Éviter les allocations fréquentes**
   ```c
   // Mauvais
   for (int i = 0; i < n; i++) {
       char *buf = malloc(SIZE);
       // ...
       free(buf);
   }
   
   // Bon
   char *buf = malloc(SIZE);
   for (int i = 0; i < n; i++) {
       // Réutiliser buf
   }
   free(buf);
   ```

2. **Cache-friendly code**
   ```c
   // Parcours row-major pour matrices
   for (int y = 0; y < height; y++) {
       for (int x = 0; x < width; x++) {
           process_tile(map[y][x]);
       }
   }
   ```

3. **Éviter les copies inutiles**
   ```cpp
   // Mauvais
   std::vector<Entity> getEntities() { return entities_; }
   
   // Bon
   const std::vector<Entity>& getEntities() const { return entities_; }
   ```

## Documentation

### Doxygen

```c
/**
 * @file player.h
 * @brief Player management functions
 * @author Team Zappy
 */

/**
 * @brief Move player forward in their current direction
 * 
 * @param player Pointer to the player structure
 * @return Error code (ERR_NONE on success)
 * 
 * @pre player != NULL
 * @pre player->team != NULL
 * @post Player position updated if move valid
 * 
 * @note Takes 7 time units to execute
 */
error_code_t player_move_forward(player_t *player);
```

### README par module

Chaque module devrait avoir son README:

```markdown
# Network Module

## Overview
Handles all network communication for the Zappy server.

## Architecture
- `socket.c`: Low-level socket operations
- `client.c`: Client connection management
- `buffer.c`: Circular buffer implementation
- `protocol.c`: Protocol parsing and building

## Usage
```c
network_t *net = network_create(port);
network_start_listening(net);
```

## Testing
```bash
make test-network
```
```

## Sécurité

### Validation des entrées

```c
// Toujours valider les entrées utilisateur
bool validate_player_name(const char *name) {
    if (!name || strlen(name) == 0) return false;
    if (strlen(name) > MAX_PLAYER_NAME) return false;
    
    // Caractères alphanumériques uniquement
    for (const char *p = name; *p; p++) {
        if (!isalnum(*p) && *p != '_' && *p != '-') {
            return false;
        }
    }
    
    return true;
}
```

### Limites de ressources

```c
// Limiter l'utilisation des ressources
#define MAX_CLIENTS 1000
#define MAX_BUFFER_SIZE 4096
#define MAX_COMMAND_LENGTH 1024
#define MAX_PENDING_COMMANDS 10

// Vérifier avant allocation
if (client_count >= MAX_CLIENTS) {
    send_error(fd, "Server full");
    close(fd);
    return;
}
```

## Checklist finale

### Avant de push

- [ ] Code compile sans warnings (`-Wall -Wextra -Werror`)
- [ ] Tests unitaires passent
- [ ] Pas de fuites mémoire (Valgrind)
- [ ] Code formaté (clang-format)
- [ ] Documentation à jour
- [ ] Pas de code de debug

### Avant une release

- [ ] Tous les tests passent
- [ ] Coverage > 80%
- [ ] Performance benchmarks OK
- [ ] Documentation complète
- [ ] CHANGELOG mis à jour
- [ ] Version tags

### Review checklist

- [ ] Logique correcte
- [ ] Gestion d'erreurs complète
- [ ] Pas de code dupliqué
- [ ] Nommage clair
- [ ] Complexité raisonnable
- [ ] Tests adéquats