# Projet Zappy - Vue d'ensemble

## Introduction

Zappy est un jeu en réseau où plusieurs équipes d'IA s'affrontent sur une carte pour collecter des ressources et atteindre le niveau maximum. Le projet se compose de trois parties principales qui communiquent via TCP/IP.

> 📚 **Documentation complète** : Toute la documentation détaillée se trouve dans le dossier [`doc/`](doc/). Ce README présente une vue d'ensemble et des liens vers les documents spécifiques.

## Architecture globale

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              SERVEUR (C)                                │
│  ┌─────────────┐  ┌──────────────┐  ┌────────────┐  ┌──────────────┐    │
│  │   Network   │  │ Game Engine  │  │   World    │  │ Time Manager │    │
│  │   Module    │  │              │  │   Module   │  │              │    │
│  │             │  │  - Teams     │  │  - Map     │  │  - Events    │    │
│  │  - Poll()   │  │  - Players   │  │  - Tiles   │  │  - Actions   │    │
│  │  - Sockets  │  │  - Elevation │  │  - Items   │  │  - Queue     │    │
│  │  - Buffers  │  │  - Rules     │  │  - Spawn   │  │              │    │
│  └──────┬──────┘  └──────┬───────┘  └─────┬──────┘  └──────┬───────┘    │
│         │                │                │                │            │
│         └────────────────┴────────────────┴────────────────┘            │
│                                    │                                    │
└────────────────────────────────────┼────────────────────────────────────┘
                                     │
                  ┌──────────────────┴──────────────────┐
                  │         Protocole TCP/IP            │
                  │    ┌─────────┐     ┌─────────┐      │
                  │    │   AI    │     │   GUI   │      │
                  │    │Protocol │     │Protocol │      │
                  │    └────┬────┘     └────┬────┘      │
                  └─────────┼───────────────┼───────────┘
                            │               │
                ┌───────────┴───┐       ┌───┴───────────┐
                │               │       │               │
                │  AI CLIENT    │       │  GUI CLIENT   │
                │  (Python)     │       │    (C++)      │
                │               │       │               │
                │ ┌───────────┐ │       │ ┌───────────┐ │
                │ │   State   │ │       │ │  Renderer │ │
                │ │  Machine  │ │       │ │  (SFML,   | |
                | |           | |       | |  Raylib)  │ │
                │ ├───────────┤ │       │ ├───────────┤ │
                │ │ Strategy  │ │       │ │    UI     │ │
                │ │  Engine   │ │       │ │  Manager  │ │
                │ ├───────────┤ │       │ ├───────────┤ │
                │ │   Team    │ │       │ │  Network  │ │
                │ │Coordinator│ │       │ │  Handler  │ │
                │ └───────────┘ │       │ └───────────┘ │
                └───────────────┘       └───────────────┘
```

## Communication entre composants

### Flux de données

```
AI Client ──────[Commands]─────> Server
         <─────[Responses]──────

GUI Client ─────[GRAPHIC]──────> Server
          <────[Game State]─────

Server ─────[Broadcasts]──────> All Clients
```

### Protocoles

- **AI Protocol**: Commandes textuelles (Forward, Look, Take, etc.)
- **GUI Protocol**: Messages structurés pour l'état du jeu
- **Format**: ASCII avec délimiteur `\n`

## Structure des modules

### 1. **Serveur** (voir [doc/zappy-server.md](doc/zappy-server.md))
- Gestion du monde et de la logique de jeu
- Communication réseau avec poll()
- Gestion du temps et des événements
- Single-threaded, event-driven

### 2. **Client GUI** (voir [doc/zappy-gui.md](doc/zappy-gui.md))
- Visualisation 2D/3D du monde
- Interface utilisateur
- Réception et parsing des mises à jour

### 3. **Client AI** (voir [doc/Final_doc_ai.md](doc/Final_doc_ai.md))
- Intelligence artificielle autonome
- Stratégies d'équipe
- Communication avec le serveur

### 4. **Protocole réseau** (voir [doc/zappy-network.md](doc/zappy-network.md))
- Spécifications détaillées des messages
- Gestion des buffers
- Formats et validation

### 5. **Guide de développement** (voir [doc/zappy-development.md](doc/zappy-development.md))
- Standards de code
- Bonnes pratiques
- Organisation Git
- Tests et CI/CD

### 6. **Documentation** (voir [doc/](doc/))
- Tous les documents techniques détaillés
- Guides d'implémentation
- Spécifications complètes

## Synchronisation et cohérence

### Gestion du temps
```
┌─────────────┐     ┌──────────────┐     ┌─────────────┐
│   Action    │     │ Time Manager │     │   Event     │
│   Request   │────>│              │────>│   Queue     │
└─────────────┘     │  Calculate   │     └─────────────┘
                    │   Deadline   │              │
                    └──────────────┘              ▼
                                          ┌─────────────┐
                                          │  Execute    │
                                          │  at Time T  │
                                          └─────────────┘
```

### État du jeu
- Le serveur est la seule source de vérité
- Les clients maintiennent une copie locale pour l'affichage
- Synchronisation via messages périodiques

## Dépendances

### Dépendances externes
- **Serveur**: libc uniquement
- **GUI**: SFML 2.5+, Raylib
- **AI**: Au choix (recommandé: Python 3.8+)

## Points d'intégration critiques

1. **Initialisation de connexion**
   - Handshake client/serveur
   - Attribution d'équipe
   - Validation des paramètres

2. **Synchronisation d'état**
   - Updates périodiques pour la GUI
   - Réponses aux commandes AI
   - Gestion des timeouts

3. **Gestion des erreurs**
   - Déconnexions propres
   - Recovery après erreur
   - Logs centralisés

## Compilation et lancement

```bash
# Compilation complète
make

# Lancement du serveur
./zappy_server -p 4242 -x 20 -y 20 -n team1 team2 -c 6 -f 100

# Lancement GUI
./zappy_gui -p 4242 -h localhost

# Lancement AI
./zappy_ai -p 4242 -n team1 -h localhost
```

## Tests et validation

Voir [doc/zappy-development.md](doc/zappy-development.md#tests) pour les détails sur :
- Tests unitaires
- Tests d'intégration
- Benchmarks
- CI/CD

## Répartition du travail

| Développeur | Responsabilité principale | Modules |
|-------------|-------------------------|----------|
| Alexandre | Architecture serveur | Network, Core Server |
| Dev 2 | Logique de jeu | Game Engine, World |
| Alexandre | Protocoles | libnetwork, Protocol handlers |
| Hugo | Interface graphique | GUI Client complet |
| Dev 5 | Intelligence artificielle | AI Client, Tests |

## Prochaines étapes

1. Lire les documents spécifiques à votre partie
2. Setup l'environnement de développement
3. Implémenter les modules de base
4. Tests d'intégration progressifs
5. Optimisations et polish

## 📚 Documentation complète

Tous les documents détaillés sont disponibles dans le dossier [`doc/`](doc/) :

- [`doc/zappy-server.md`](doc/zappy-server.md) - Architecture détaillée du serveur
- [`doc/zappy-gui.md`](doc/zappy-gui.md) - Architecture du client graphique
- [`doc/zappy-ai.md`](doc/Final_doc_ai.md) - Architecture du client AI
- [`doc/zappy-network.md`](doc/zappy-network.md) - Protocole réseau détaillé
- [`doc/zappy-development.md`](doc/zappy-development.md) - Guide de développement complet

Pour plus de détails sur chaque composant, consultez les documents spécifiques.
