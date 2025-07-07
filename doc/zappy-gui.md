# Architecture du Client GUI Zappy

## Vue d'ensemble

Le client GUI est une application C++ chargée de fournir une représentation visuelle en temps réel du monde du jeu Zappy. Il se connecte au serveur Zappy via le protocole `GRAPHIC` et interprète les commandes du serveur pour afficher l'état du jeu.

L'application supporte deux modes de rendu :
1.  **2D (SFML)**: Une vue de dessus classique utilisant la bibliothèque SFML pour le rendu.
2.  **3D (Raylib)**: Une vue en perspective 3D utilisant la bibliothèque Raylib.

## Architecture

L'architecture s'inspire du modèle **MVC (Modèle-Vue-Contrôleur)**, adapté pour une application temps réel.

```
┌─────────────────────────────────────────────────────────┐
│                      APPLICATION                        │
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────┐   │
│  │    MODÈLE   │  │      VUE     │  │  CONTRÔLEUR   │   │
│  │             │  │              │  │               │   │
│  │  World      │  │   IRenderer  │  │ Application   │   │
│  │  (Player,   │  │   (Renderer, │  │ (Événements)  │   │
│  │  Tile, Egg) │  │  Renderer3D) │  │ UIManager     │   │
│  └──────┬──────┘  └──────┬───────┘  └───────┬───────┘   │
│         │                │                  │           │
│         └────────────────┴──────────────────┘           │
│                          │                              │
└──────────────────────────┼──────────────────────────────┘
                           │
                     NetworkClient (Thread séparé)
```

-   **Modèle**: Représenté principalement par la classe `World`, qui contient l'état complet du jeu : la carte, les joueurs, les équipes, les œufs et les ressources.
-   **Vue**: Gérée par l'interface `IRenderer` et ses implémentations `Renderer` (2D) et `Renderer3D` (3D). La vue inclut également la `Camera` et l'`UIManager`.
-   **Contrôleur**: La logique de contrôle est partagée. `Application` gère la boucle principale et les événements utilisateur (clavier, souris). `NetworkClient` reçoit les mises à jour du serveur dans un thread séparé et les place dans une file d'attente. `MessageParser` traduit les messages bruts du serveur en objets `Message` exploitables.

## Structure des modules

```
gui/
├── include/
│   |
│   ├── Application.hpp      # Classe principale, gestion de la boucle de jeu
│   ├── Config.hpp           # Gestion de la configuration (arguments, fichiers)
|   |
│   ├── model/
│   │   ├── World.hpp            # Conteneur principal de l'état du jeu
│   │   ├── Tile.hpp             # Représente une case de la carte
│   │   ├── Player.hpp           # Représente un joueur
│   │   ├── Egg.hpp              # Représente un œuf
│   │   ├── Team.hpp             # Représente une équipe
│   │   └── Entity.hpp           # Classe de base pour les entités
│   ├── view/
│   │   ├── IRenderer.hpp        # Interface pour les moteurs de rendu
│   │   ├── Renderer.hpp         # Moteur de rendu 2D (SFML)
│   │   ├── Renderer3D.hpp       # Moteur de rendu 3D (Raylib)
│   │   ├── Camera.hpp           # Caméra 2D avec modes (Free, Follow, Overview)
│   │   ├── Animation.hpp        # Système de gestion des animations de sprites
│   │   ├── ParticleSystem.hpp   # Système de particules pour les effets visuels (2D)
│   │   └── TileAnimationSystem.hpp # Animations sur les cases (2D)
│   ├── ui/
│   │   ├── UIManager.hpp        # Gère l'ensemble des éléments de l'interface
│   │   ├── HUD.hpp              # Affichage tête haute (temps, etc.)
│   │   ├── DebugPanel.hpp       # Panneau de débogage (F3)
│   │   ├── TeamPanel.hpp        # Panneau d'information des équipes
│   │   ├── PlayerInfoPanel.hpp  # Panneau d'information pour un joueur sélectionné
│   │   ├── TileInfoPanel.hpp    # Panneau d'information pour une case sélectionnée
│   │   └── EventLog.hpp         # Journal des événements du jeu
│   ├── network/
│   │   ├── NetworkClient.hpp    # Client TCP pour la communication avec le serveur
│   │   ├── MessageParser.hpp    # Transforme les chaînes du serveur en objets Message
│   │   ├── Message.hpp          # Représente un message du serveur
│   │   └── ThreadSafeQueue.hpp  # File d'attente sécurisée pour la communication inter-thread
│   ├── events/
│   │   ├── Event.hpp            # Définition des événements internes (ex: PlayerAdded)
│   │   └── EventDispatcher.hpp  # Système de publication/souscription d'événements
│   └── utils/
│       ├── Logger.hpp           # Utilitaire de logging
│       ├── Math.hpp             # Fonctions mathématiques utiles
│       └── ResourceManager.hpp  # Gestion centralisée des assets (textures, polices)
│
├── src/
│   └── (Contient les fichiers .cpp correspondants)
│
└── assets/
    ├── fonts/
    ├── music/
    ├── sprites/               # Sprites pour le rendu 2D
    └── sprites3D/             # Modèles 3D (format .glb)
```

## Classes principales

### Application

La classe `Application` orchestre l'ensemble du client. Elle initialise tous les systèmes, gère la boucle de jeu principale et coordonne les mises à jour, le rendu et la gestion des événements.

```cpp
class Application {
private:
    // Systèmes principaux
    std::unique_ptr<EventDispatcher> eventDispatcher_;
    std::unique_ptr<NetworkClient> network_;
    std::unique_ptr<World> world_;
    std::unique_ptr<IRenderer> renderer_;
    std::unique_ptr<UIManager> ui_;
    std::unique_ptr<Camera> camera_;

    // Fenêtre (pour le mode 2D)
    sf::RenderWindow window_;

    // État de l'application
    const Config& config_;
    bool running_;
    bool initialized_;

public:
    bool initialize();
    void run(); // Contient la boucle de jeu
    void shutdown();
};
```

### World (Modèle)

`World` est la source de vérité pour l'état du jeu. Il contient toutes les données reçues du serveur.

```cpp
class World {
private:
    // Carte du jeu
    std::vector<std::vector<Tile>> tiles_;
    int mapWidth_{0};
    int mapHeight_{0};

    // Entités du jeu
    std::unordered_map<PlayerID, std::unique_ptr<Player>> players_;
    std::unordered_map<TeamID, std::unique_ptr<Team>> teams_;
    std::unordered_map<EggID, std::unique_ptr<Egg>> eggs_;

    // Communication interne
    EventDispatcher& dispatcher_;

public:
    void setMapSize(int width, int height);
    void addPlayer(std::unique_ptr<Player> player);
    void removePlayer(PlayerID id);
    Player* getPlayer(PlayerID id);
    // ... autres getters et setters
    void update(float deltaTime);
};
```

### IRenderer et ses implémentations (Vue)

L'interface `IRenderer` définit le contrat pour les moteurs de rendu.
-   `Renderer` (2D) utilise SFML pour dessiner des sprites. Il intègre des systèmes d'animation et de particules.
-   `Renderer3D` (3D) utilise Raylib pour dessiner des modèles 3D. Il gère sa propre caméra 3D et l'éclairage.

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual bool initialize() = 0;
    virtual void render(const World& world, const Camera& camera, float deltaTime) = 0;
    // ... autres méthodes virtuelles (resize, handleEvents, etc.)
};
```

## Système de rendu

### Rendu 2D (SFML)

-   **Couches**: Le rendu est effectué en plusieurs passes : fond (ciel), cases (herbe), ressources, animations de cases, entités (joueurs, œufs), effets de particules, et enfin l'interface utilisateur.
-   **Optimisations**:
    -   **Culling**: Seuls les éléments visibles dans le champ de la caméra sont dessinés.
    -   **Animations**: Le `AnimationSystem` gère les animations de sprites basées sur l'état des entités (marche, inactivité).
    -   **Effets**: Le `ParticleSystem` et le `TileAnimationSystem` ajoutent des effets visuels pour les actions comme les incantations, les broadcasts ou la mort.

### Rendu 3D (Raylib)

-   **Scène 3D**: Le `Renderer3D` construit une scène 3D à partir des données du `World`.
-   **Modèles**: Il charge des modèles 3D (`.glb`) pour le terrain, les joueurs, les ressources et les œufs.
-   **Caméra 3D**: Il utilise une `Camera3D` de Raylib, dont la position est calculée pour offrir une vue en perspective de la carte. La caméra peut être contrôlée manuellement.
-   **Éclairage et Ombres**: Un éclairage simple et des ombres projetées au sol sont implémentés pour donner de la profondeur à la scène.
-   **Sélection**: L'utilisateur peut sélectionner des cases et des joueurs en cliquant dessus, grâce au lancer de rayon (raycasting).

## Système de caméra

La classe `Camera` est utilisée pour la navigation 2D.

-   **Modes**:
    1.  `FREE`: Déplacement libre avec le clavier (`WASD`/flèches) et la souris.
    2.  `FOLLOW`: Suit un joueur spécifique.
    3.  `TEAM`: Centre la vue sur les joueurs d'une équipe.
    4.  `OVERVIEW`: Affiche une vue d'ensemble de toute la carte.
-   **Contrôles**:
    -   `WASD`/Flèches: Déplacement.
    -   Molette de la souris: Zoom.
    -   Clic milieu / Clic gauche (en mode `FREE`): Glisser la caméra.
    -   `Tab`: Changer de mode de caméra.
    -   `Espace`: Réinitialiser la vue.

En mode 3D, `Renderer3D` gère sa propre caméra `Camera3D` avec des contrôles similaires (orbite, pan, zoom).

## Système de réseau

-   **Thread dédié**: `NetworkClient` s'exécute dans un thread séparé pour ne pas bloquer la boucle de rendu principale.
-   **Communication asynchrone**:
    -   Les messages reçus du serveur sont parsés par `MessageParser`.
    -   Les objets `Message` résultants sont placés dans une `ThreadSafeQueue<Message>`.
    -   La boucle principale (`Application::update`) récupère les messages de cette file pour mettre à jour le `World`.

```cpp
// Pseudo-code du thread réseau
void NetworkClient::networkThreadFunction() {
    while (!shouldStop_) {
        // Recevoir les données du socket
        processIncomingData(); // Ajoute les données brutes à un buffer

        // Parser le buffer pour extraire des messages complets
        auto messages = parser.parseBuffer(receiveBuffer_);
        for (auto& message : messages) {
            incomingMessages_.push(std::move(message)); // Pousse le message dans la file thread-safe
        }

        // Envoyer les commandes en attente
        sendPendingCommands();

        std::this_thread::sleep_for(1ms);
    }
}
```

## Interface Utilisateur (UI)

L'`UIManager` est responsable de l'initialisation et de la gestion de tous les composants de l'interface.

-   **Panneaux d'information**:
    -   `PlayerInfoPanel`: Affiche les détails d'un joueur sélectionné (niveau, inventaire, équipe).
    -   `TileInfoPanel`: Affiche les détails d'une case sélectionnée (coordonnées, ressources).
    -   `TeamPanel`: Liste les équipes et leurs statistiques.
    -   `DebugPanel` (activé avec `F3`): Affiche des informations de performance (FPS, temps de rendu, etc.).
    -   `EventLog`: Affiche un journal des événements importants du jeu.
-   **Interaction**: L'UI peut être interactive. Cliquer sur un joueur ou une case affiche le panneau d'information correspondant.

## Gestion des assets

-   **`ResourceManager`**: Classe singleton qui charge et stocke toutes les ressources (textures, polices) pour éviter les chargements redondants.
-   **Structure des assets**:
    ```
    assets/
    ├── fonts/
    │   └── Display.ttf
    ├── music/
    │   ├── get-out.mp3
    │   └── playtime.mp3
    ├── sprites/
    │   ├── animation.png
    │   ├── crystal-*.png
    │   ├── egg.png
    │   ├── food.png
    │   └── grass.png
    └── sprites3D/
        ├── character-male-d.glb
        ├── Crystal.glb
        ├── Egg.glb
        ├── Floor Grass Sliced B.glb
        └── Hamburger.glb
    ```

## Configuration

La configuration est gérée par la classe `Config`. Elle peut être définie via :
1.  Les arguments de la ligne de commande (prioritaires).
2.  Un fichier de configuration (non implémenté actuellement mais la structure est prévue).

Options configurables :
-   `-p, --port`: Port du serveur.
-   `-h, --host`: Hôte du serveur.
-   `-g, --graphics`: Mode de rendu (`2d` ou `3d`).
-   `-r, --resolution`: Résolution de la fenêtre (ex: `1920x1080`).
-   `-f, --fullscreen`: Activer le mode plein écran.
-   `--debug`: Afficher les informations de débogage.
