# Architecture du Serveur Zappy

## Vue d'ensemble

Le serveur est le cœur du projet Zappy. Il gère l'état du monde, applique les règles du jeu, et coordonne la communication entre tous les clients. Architecture **single-threaded** et **event-driven** utilisant `poll()`.

## Architecture modulaire

```
server/
├── core/               # Cœur du serveur
│   ├── server.c       # Boucle principale
│   ├── poll_manager.c # Gestion du multiplexing
│   └── config.c       # Parsing des arguments
│
├── network/           # Module réseau
│   ├── socket.c      # Gestion des sockets
│   ├── client.c      # Gestion des clients
│   ├── buffer.c      # Buffers circulaires
│   └── protocol.c    # Parsing/Building
│
├── world/            # Module monde
│   ├── map.c        # Carte toroïdale
│   ├── tile.c       # Gestion des cases
│   ├── resources.c  # Spawn et distribution
│   └── vision.c     # Calcul de vision
│
├── game/            # Logique de jeu
│   ├── player.c     # Gestion des joueurs
│   ├── team.c       # Gestion des équipes
│   ├── elevation.c  # Système d'élévation
│   └── egg.c        # Système d'œufs
│
├── engine/          # Moteur temporel
│   ├── time.c       # Gestion du temps
│   ├── events.c     # Queue d'événements
│   └── actions.c    # Exécution d'actions
│
└── commands/        # Handlers de commandes
    ├── ai_commands.c
    └── gui_commands.c
```

## Structures de données principales

### Structure Server

```c
typedef struct server_s {
    /* Configuration */
    config_t config;              // Paramètres de lancement
    
    /* Réseau */
    int listen_fd;                // Socket d'écoute
    struct pollfd *poll_fds;      // Array pour poll()
    size_t poll_count;            // Nombre de fds
    client_t *clients;            // Liste des clients
    
    /* Monde */
    map_t *map;                   // Carte du jeu
    team_t *teams;                // Équipes
    size_t team_count;            
    
    /* Temps */
    time_manager_t *time_mgr;     // Gestionnaire temporel
    event_queue_t *events;        // Queue d'événements
    
    /* Pools mémoire */
    pool_t *player_pool;          // Pool de joueurs
    pool_t *egg_pool;             // Pool d'œufs
    pool_t *resource_pool;        // Pool de ressources
    
    /* État */
    bool running;                 // Flag d'exécution
    game_state_t state;           // État du jeu
} server_t;
```

### Structure Client

```c
typedef struct client_s {
    /* Réseau */
    int fd;                       // File descriptor
    struct sockaddr_in addr;      // Adresse IP
    
    /* Buffers */
    circular_buffer_t *in_buf;    // Buffer d'entrée
    circular_buffer_t *out_buf;   // Buffer de sortie
    
    /* État */
    client_type_t type;           // AI ou GUI
    client_state_t state;         // CONNECTING, CONNECTED, etc.
    
    /* Parsing */
    char partial_cmd[CMD_MAX];    // Commande partielle
    size_t partial_len;           
    
    /* Jeu */
    player_t *player;             // Joueur associé (si AI)
    team_t *team;                 // Équipe (si AI)
    
    /* Commandes */
    command_queue_t *cmd_queue;   // Queue de commandes (max 10)
    
    /* Stats */
    time_t connect_time;          
    size_t bytes_sent;
    size_t bytes_received;
} client_t;
```

## Gestion du temps et des événements

### Concept temporel

```
Temps réel ─────> Time Units ─────> Actions
   (ms)            (1/f sec)         (durée)

Example avec f=100:
- 1 time unit = 10ms
- Action "Forward" = 7 time units = 70ms
```

### Event Queue

```c
typedef struct event_s {
    uint64_t execute_at;          // Timestamp d'exécution
    event_type_t type;            // Type d'événement
    void *data;                   // Données associées
    event_handler_t handler;      // Fonction à appeler
    struct event_s *next;         // Liste chaînée triée
} event_t;
```

### Flow d'exécution

```
1. Client envoie commande
2. Validation et parsing
3. Calcul du temps d'exécution
4. Ajout à l'event queue
5. Poll avec timeout jusqu'au prochain event
6. Exécution de l'event
7. Envoi de la réponse
```

## Module Network

### Buffer circulaire

```
┌─────────────────────────────────────┐
│                                     │
│  [────DONNÉES────]        [ESPACE]  │
│   ^              ^        ^         │
│   read_pos       write_pos          │
└─────────────────────────────────────┘
```

**Avantages:**
- Pas de memmove nécessaire
- Taille fixe (pas de realloc)
- Gestion simple du wrap-around

### Gestion des commandes

```c
typedef struct command_s {
    char data[CMD_MAX];           // Commande complète
    size_t len;                   
    uint64_t received_at;         // Pour debug/stats
    struct command_s *next;       
} command_t;
```

**Limite de 10 commandes:**
- Si queue pleine, drop la nouvelle commande
- Envoyer "ko" au client
- Logger l'événement

## Module World

### Map toroïdale

```
Coordonnées (x, y):
- real_x = x % map_width
- real_y = y % map_height
- Si x < 0: real_x = map_width + (x % map_width)
```

### Tile structure

```c
typedef struct tile_s {
    /* Position */
    int x, y;
    
    /* Contenu */
    resource_list_t *resources;   // Liste de ressources
    player_list_t *players;       // Liste de joueurs
    egg_list_t *eggs;            // Liste d'œufs
    
    /* Optimisation */
    uint32_t resource_count[RESOURCE_MAX];
    uint32_t total_resources;
    bool dirty;                   // Pour GUI updates
} tile_t;
```

### Spawn des ressources

**Algorithme de distribution:**
1. Calculer quantité totale: `width * height * density`
2. S'assurer qu'il y a au moins 1 de chaque ressource
3. Distribuer aléatoirement mais uniformément
4. Re-spawn toutes les 20 time units

## Module Game

### Système d'élévation

**Vérifications:**
1. Au début: Vérifier ressources + joueurs requis
2. Pendant: Freeze tous les participants
3. À la fin: Re-vérifier les conditions
4. Si succès: Consommer ressources, level up
5. Si échec: Unfreeze sans effet

### Gestion des œufs

```
Fork ──> Create Egg ──> Add to tile ──> Wait for connection
                           │
                           ▼
                    Client connects ──> Hatch ──> Spawn player
```

## Optimisations importantes

### 1. Spatial indexing
- Diviser la map en régions pour broadcasts
- Cache de vision par niveau
- Dirty flags pour GUI updates

### 2. Memory pools
```c
// Au lieu de malloc/free fréquents
player_t *player = pool_alloc(server->player_pool);
// ...
pool_free(server->player_pool, player);
```

### 3. Zero-copy networking
- Utiliser `writev()` pour envoyer plusieurs buffers
- Éviter les copies inutiles
- Direct pointer dans circular buffer

### 4. Time management
- Utiliser `clock_gettime(CLOCK_MONOTONIC)`
- Calculer timeout dynamiquement
- Batch les events proches

## Gestion des erreurs

### Hiérarchie d'erreurs

```
CRITICAL ──> Arrêt du serveur
ERROR ────> Log + recovery
WARNING ──> Log + continue
INFO ─────> Log si verbose
DEBUG ────> Log si debug mode
```

### Recovery patterns

```c
// Pattern goto cleanup
error_code_t handle_client_command(client_t *client) {
    error_code_t ret = ERR_NONE;
    command_t *cmd = NULL;
    
    cmd = parse_command(client);
    if (!cmd) {
        ret = ERR_PARSE;
        goto cleanup;
    }
    
    // ... traitement ...
    
cleanup:
    if (cmd) command_free(cmd);
    return ret;
}
```

## Points d'attention critiques

### 1. Thread safety
- **Rappel**: Single-threaded !
- Pas de mutex nécessaire
- Attention aux callbacks qui modifient l'état

### 2. Integer overflow
- Vérifier les calculs de temps
- Utiliser des types appropriés (uint64_t pour timestamps)
- Saturer plutôt qu'overflow

### 3. Resource leaks
- Valgrind obligatoire
- Cleanup handlers pour chaque module
- Atexit pour cleanup global

### 4. Network edge cases
- Client qui déconnecte pendant une action
- Partial writes (EAGAIN)
- Broken pipe (SIGPIPE)

## Tests spécifiques serveur

### Tests unitaires
- Test de chaque module isolément
- Mock des dépendances
- Couverture des cas d'erreur

### Tests d'intégration
- Scénarios complets de jeu
- Multiple clients
- Conditions de course

### Tests de stress
- 100+ clients simultanés
- Commandes en rafale
- Déconnexions/reconnexions

### Tests de robustesse
- Fuzzing des entrées
- Mémoire limitée
- CPU throttling

## Debugging et monitoring

### Logs structurés
```
[TIMESTAMP] [LEVEL] [MODULE] Message
2024-03-14 10:23:45.123 [INFO] [GAME] Player 42 reached level 3
```

### Métriques importantes
- Latence par commande
- Utilisation mémoire
- Nombre de clients actifs
- Events par seconde

### Debug mode
- Dump de l'état du monde
- Trace des commandes
- Statistiques détaillées

## Checklist de développement

- [ ] Parser d'arguments robuste
- [ ] Initialisation des sockets
- [ ] Poll loop avec timeout dynamique
- [ ] Circular buffers pour I/O
- [ ] Parsing incrémental des commandes
- [ ] Event queue avec priority
- [ ] Map toroïdale
- [ ] Spawn/respawn des ressources
- [ ] Système d'élévation complet
- [ ] Gestion des œufs
- [ ] Memory pools
- [ ] Cleanup propre
- [ ] Tests complets
- [ ] Documentation