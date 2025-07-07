# **Documentation de l'IA Zappy**

## **Vue d'ensemble**

L'IA Zappy est un client intelligent conçu pour jouer au jeu Zappy, un jeu multijoueur en réseau où des équipes s'affrontent pour faire évoluer leurs joueurs jusqu'au niveau maximum. L'objectif est d'avoir au moins 6 joueurs de niveau 8 dans son équipe pour remporter la partie.

### **Objectifs principaux**

* **Survie** : Maintenir un niveau de nourriture suffisant  
* **Collecte** : Rassembler les ressources nécessaires pour l'élévation  
* **Élévation** : Progresser dans les niveaux (1 → 8\)  
* **Coordination** : Collaborer avec les coéquipiers via des messages

### **Caractéristiques de l'IA**

* Stratégies adaptatives selon le niveau du joueur  
* Gestion intelligente de la nourriture avec priorisation  
* Système de coordination par broadcast  
* Navigation optimisée sur la carte  
* Gestion robuste des erreurs réseau

### **Architecture du projet Zappy**

                   ┌─────────────────┐  
                    │   ZAPPY SERVER  │  
                    │      (C)        │  
                    │                 │  
                    │ ┌─────────────┐ │  
                    │ │   Game      │ │  
                    │ │   Engine    │ │  
                    │ ├─────────────┤ │  
                    │ │   World     │ │  
                    │ │  Manager    │ │  
                    │ ├─────────────┤ │  
                    │ │   Network   │ │  
                    │ │   Handler   │ │  
                    │ └─────────────┘ │  
                    └─────────────────┘  
                             │  
            ┌────────────────┴────────────────┐  
            │                                 │  
    ┌───────────┴───┐               ┌───┴───────────┐  
    │               │               │               │  
    │  AI CLIENT    │               │  GUI CLIENT   │  
    │  (Python)     │               │    (C++)      │  
    │               │               │               │  
    │ ┌───────────┐ │               │ ┌───────────┐ │  
    │ │   State   │ │               │ │  Renderer │ │  
    │ │  Machine  │ │               │ │(SFML/Raylib)│ │  
    │ ├───────────┤ │               │ ├───────────┤ │  
    │ │ Strategy  │ │               │ │    UI     │ │  
    │ │  Engine   │ │               │ │  Manager  │ │  
    │ ├───────────┤ │               │ ├───────────┤ │  
    │ │   Team    │ │               │ │  Network  │ │  
    │ │Coordinator│ │               │ │  Handler  │ │  
    │ └───────────┘ │               │ └───────────┘ │  
    └───────────────┘               └───────────────┘

## **Architecture**

L'IA est structurée en modules spécialisés suivant le principe de séparation des responsabilités :

ZappyAI (Module principal)  
├── NetworkManager (Communication)  
├── SurvivalManager (Gestion de la survie)  
├── MovementManager (Déplacements)  
├── ResourceManager (Collecte de ressources)  
├── InventoryManager (Gestion de l'inventaire)  
├── ElevationManager (Rituels d'élévation)  
├── CoordinationManager (Communication inter-joueurs)  
└── Models (Structures de données)

## **Installation et utilisation**

### **Prérequis**

* Python 3.7+  
* Serveur Zappy en fonctionnement

### **Lancement**

python main.py \-p \<port\> \-n \<nom\_équipe\> \-h \<machine\>

**Paramètres :**

* `-p` : Port du serveur (obligatoire)  
* `-n` : Nom de l'équipe (obligatoire)  
* `-h` : Adresse du serveur (défaut: localhost)

**Exemple :**

python main.py \-p 4242 \-n Team1 \-h localhost

### **Arrêt gracieux**

L'IA peut être interrompue proprement avec `Ctrl+C`. Elle fermera automatiquement les connexions réseau.

## **Modules détaillés**

### **ZappyAI (Module principal)**

**Responsabilités :**

* Orchestration de tous les modules  
* Boucle principale de jeu  
* Gestion des stratégies par niveau

#### **Fonctions principales :**

**`__init__(self, config: GameConfig)`**

* **Objectif** : Initialise l'IA avec la configuration fournie  
* **Paramètres** : `config` contenant host, port, team\_name  
* **Actions** : Configure les paramètres de base et initialise `last_food_check`

**`connect(self) -> bool`**

* **Objectif** : Établit la connexion avec le serveur Zappy  
* **Retour** : `True` si connexion réussie, `False` sinon  
* **Processus** :  
  1. Utilise `NetworkManager.create_connection()`  
  2. Assigne le joueur créé à `self.player`  
  3. Retourne le statut de connexion

**`run_intelligent_strategy(self)`**

* **Objectif** : Boucle principale d'exécution de l'IA  
* **Fonctionnement** :  
  1. **Vérification initiale** : Contrôle critique de nourriture  
  2. **Mouvement initial** : 2 pas en avant pour commencer l'exploration  
  3. **Boucle principale** (500 itérations max) :  
     * Vérifie régulièrement la nourriture (toutes les 12 itérations)  
     * Écoute les communications d'équipe  
     * Exécute la stratégie selon le niveau  
     * Gère les broadcasts périodiques  
     * Adapte la vitesse selon le niveau de nourriture  
  4. **Gestion des interruptions** : Nettoyage propre en cas d'arrêt

**`_level_1_strategy(self)`**

* **Objectif** : Stratégie spécialisée pour passer du niveau 1 au 2  
* **Logique** :  
  1. **Priorité absolue** : Survie (vérification nourriture critique)  
  2. **État COLLECTING** : Recherche 1 linemate si manquant  
  3. **État ELEVATING** : Tentative d'élévation une fois linemate obtenu  
* **Caractéristiques** : Stratégie simple et directe, focus survie \+ linemate

**`_level_2_strategy(self)`**

* **Objectif** : Stratégie complexe pour le niveau 2 (passage vers niveau 3\)  
* **Besoins** : 1 linemate, 1 deraumere, 1 sibur \+ 1 partenaire niveau 2  
* **Logique** :  
  1. **Gestion nourriture** : Collecte si besoin ou opportuniste  
  2. **Collecte ressources** : Priorise les ressources manquantes  
  3. **Exploration** : 3 patterns de mouvement différents :  
     * Pattern 1 : 4 pas \+ rotation droite  
     * Pattern 2 : 2 pas \+ rotation gauche \+ 1 pas  
     * Pattern 3 : Mouvement aléatoire (70% avancer, 30% tourner)  
  4. **Communication** : Broadcasts selon le rôle (leader/follower)

**`_higher_level_strategy(self)`**

* **Objectif** : Stratégie pour niveaux 3+ (leadership et organisation)  
* **Fonctionnement** :  
  1. **Priorité survie** : Vérification critique de nourriture  
  2. **Exploration libre** : Mouvement aléatoire (2-4 pas)  
  3. **Leadership** : Messages d'organisation si leader (10% de chance)

### **NetworkManager**

**Responsabilités :**

* Gestion des connexions TCP  
* Envoi/réception de commandes  
* Gestion des timeouts et erreurs réseau

#### **Fonctions principales :**

**`create_connection(config: GameConfig, team_name: str) -> Player`**

* **Objectif** : Crée une nouvelle connexion au serveur  
* **Processus** :  
  1. **Création socket** : Socket TCP avec timeout 1 seconde  
  2. **Connexion** : Se connecte à `config.host:config.port`  
  3. **Handshake** :  
     * Reçoit le message de bienvenue  
     * Envoie le nom d'équipe  
     * Reçoit les dimensions de la carte et le nombre de slots  
  4. **Initialisation** : Configure le joueur avec ID aléatoire et statut leader  
* **Retour** : Objet `Player` configuré ou `None` si échec

**`send_command(player: Player, command: str) -> str`**

* **Objectif** : Envoie une commande au serveur et récupère la réponse  
* **Paramètres** : `player` (connexion), `command` (commande à envoyer)  
* **Fonctionnalités** :  
  1. **Vérification connexion** : Vérifie que le joueur est connecté  
  2. **Envoi** : Envoie la commande avec `\n`  
  3. **Réception** : Attend et décode la réponse  
  4. **Parsing spécial** : Détecte les changements de niveau dans la réponse  
  5. **Gestion erreurs** : Retourne des codes d'erreur spécifiques  
* **Retours possibles** : Réponse du serveur, "DEAD", "DISCONNECTED", "ERROR"

**`listen_for_broadcasts(player: Player) -> List[str]`**

* **Objectif** : Écoute les messages diffusés par les autres joueurs  
* **Mécanisme** :  
  1. **Timeout court** : 0.1 seconde pour ne pas bloquer  
  2. **Réception continue** : Lit tous les messages disponibles  
  3. **Filtrage** : Ne garde que les messages contenant des mots-clés strategiques  
  4. **Nettoyage** : Remet le timeout à None pour les autres opérations  
* **Retour** : Liste des messages pertinents reçus

### **SurvivalManager**

**Responsabilités :**

* Surveillance du niveau de nourriture  
* Collecte prioritaire de nourriture  
* Prévention de la famine

**Seuils critiques :**

* **Critique** : \< 2 unités (action immédiate)  
* **Faible** : \< 5 unités (recherche active)  
* **Bientôt faible** : \< 10 unités (collecte opportuniste)  
* **Stockage** : \< 15 unités (pour niveaux 2+)

#### **Fonctions principales :**

**`check_food_level(player: Player) -> int`**

* **Objectif** : Retourne la quantité actuelle de nourriture  
* **Mécanisme** : Utilise `InventoryManager.get_resource_count()` pour la nourriture  
* **Usage** : Fonction utilitaire utilisée par toutes les autres vérifications

**`needs_food(player: Player) -> bool`**

* **Objectif** : Détermine si le joueur a besoin de nourriture  
* **Seuil** : \< 5 unités de nourriture  
* **Usage** : Déclenchement de la collecte de nourriture standard

**`needs_food_soon(player: Player) -> bool`**

* **Objectif** : Détecte un besoin de nourriture proche  
* **Seuil** : \< 10 unités de nourriture  
* **Usage** : Collecte opportuniste pendant d'autres activités

**`needs_food_critically(player: Player) -> bool`**

* **Objectif** : Détecte une situation de survie critique  
* **Seuil** : \< 2 unités de nourriture  
* **Usage** : Priorité absolue, arrêt de toute autre activité

**`gather_food(player: Player) -> bool`**

* **Objectif** : Collecte de nourriture standard (jusqu'à 15 tentatives)  
* **Stratégie** :  
  1. **Vérification immédiate** : Regarde s'il y a de la nourriture sur la tuile actuelle  
  2. **Scan vision** : Cherche dans toutes les tuiles visibles  
  3. **Navigation** : Se déplace vers la nourriture trouvée  
  4. **Exploration** : Avance et tourne pour explorer si rien trouvé  
* **Retour** : `True` si nourriture collectée, `False` sinon

**`gather_food_aggressively(player: Player, target_amount: int = 15) -> bool`**

* **Objectif** : Collecte intensive jusqu'à atteindre la quantité cible  
* **Différences avec gather\_food** :  
  * Plus de tentatives (30 vs 15\)  
  * Continue jusqu'à atteindre `target_amount`  
  * Rotations moins fréquentes (toutes les 6 vs 4 tentatives)  
* **Usage** : Stockage pour joueurs niveau 2+ avec assez de nourriture de base

**`should_stockpile_food(player: Player) -> bool`**

* **Objectif** : Détermine si le joueur devrait faire des réserves  
* **Conditions** : Nourriture \< 15 ET niveau ≥ 2  
* **Usage** : Optimisation pour joueurs expérimentés

### **MovementManager**

**Responsabilités :**

* Déplacements sur la carte  
* Navigation vers des tuiles spécifiques  
* Exploration de l'environnement

**Navigation par tuiles :**

Vision niveau 1:     Vision niveau 2:  
    \[1\] \[2\] \[3\]         \[9\]\[10\]\[11\]\[12\]\[13\]\[14\]\[15\]  
      \[0\]                   \[4\] \[5\] \[6\] \[7\] \[8\]  
                              \[1\] \[2\] \[3\]  
                                \[0\]

#### **Fonctions principales :**

**`move_forward(player: Player) -> bool`**

* **Objectif** : Fait avancer le joueur d'une tuile  
* **Mécanisme** : Envoie "Forward" via NetworkManager  
* **Retour** : `True` si mouvement réussi ("ok"), `False` sinon

**`turn_left(player: Player) -> bool`**

* **Objectif** : Fait tourner le joueur de 90° vers la gauche  
* **Mécanisme** : Envoie "Left" via NetworkManager  
* **Retour** : `True` si rotation réussie, `False` sinon

**`turn_right(player: Player) -> bool`**

* **Objectif** : Fait tourner le joueur de 90° vers la droite  
* **Mécanisme** : Envoie "Right" via NetworkManager  
* **Retour** : `True` si rotation réussie, `False` sinon

**`look_around(player: Player) -> List[str]`**

* **Objectif** : Obtient la vision du joueur (contenu des tuiles visibles)  
* **Processus** :  
  1. **Commande** : Envoie "Look" au serveur  
  2. **Parsing** : Supprime les crochets et sépare par virgules  
  3. **Nettoyage** : Supprime les espaces en trop  
* **Format retour** : Liste de strings représentant chaque tuile  
* **Exemple** : `["player food", "linemate", "", "food deraumere"]`

**`navigate_to_tile(player: Player, tile_index: int)`**

* **Objectif** : Navigue vers une tuile spécifique dans la vision  
* **Logique complexe selon l'index** :  
  * **Index 0** : Tuile actuelle, rien à faire  
  * **Index 1** : Tourner gauche \+ avancer  
  * **Index 2** : Avancer uniquement  
  * **Index 3** : Tourner droite \+ avancer  
  * **Index 4-8** : Navigation avancée pour joueurs niveau 2+  
    * Combinaisons de mouvements et rotations  
    * Utilise la géométrie de la vision étendue  
* **Optimisation** : Chemins optimaux selon la position dans la grille de vision

### **ResourceManager**

**Responsabilités :**

* Recherche de ressources spécifiques  
* Optimisation des trajets de collecte  
* Priorisation des ressources selon les besoins

**Stratégie de recherche :**

1. Scan de la vision actuelle  
2. Déplacement en spirale si non trouvé  
3. Collecte opportuniste de nourriture pendant la recherche

#### **Fonctions principales :**

**`search_for_resource(player: Player, resource: str) -> int`**

* **Objectif** : Cherche une ressource spécifique et retourne sa position  
* **Algorithme de recherche** :  
  1. **Scan initial** : Regarde dans la vision actuelle  
  2. **Collecte opportuniste** : Ramasse la nourriture si trouvée  
  3. **Exploration systématique** :  
     * 2 pas en avant  
     * Rotation droite  
     * Répétition jusqu'à limite  
* **Limites** : Maximum `min(largeur × hauteur, 50)` tentatives  
* **Retour** : Index de la tuile contenant la ressource, \-1 si non trouvée

**`find_and_go_to_resource(player: Player, resource: str) -> bool`**

* **Objectif** : Trouve une ressource et s'y déplace  
* **Processus** :  
  1. **Recherche** : Utilise `search_for_resource()`  
  2. **Navigation** : Se déplace vers la ressource si trouvée  
* **Retour** : `True` si ressource trouvée et accessible, `False` sinon

**`collect_resource(player: Player, resource: str) -> bool`**

* **Objectif** : Fonction complète de collecte d'une ressource  
* **Processus complet** :  
  1. **Localisation** : Trouve et va vers la ressource  
  2. **Collecte** : Envoie commande "Take {resource}"  
  3. **Vérification** : Confirme le succès via réponse serveur  
* **Retour** : `True` si collecte réussie, `False` si échec ou ressource introuvable

### **InventoryManager**

**Responsabilités :**

* Analyse de l'inventaire du joueur  
* Parsing des réponses du serveur  
* Suivi des quantités de ressources

**Format de réponse typique :**

\[food 10, linemate 2, deraumere 1, sibur 0, ...\]

#### **Fonctions principales :**

**`get_inventory(player: Player) -> dict`**

* **Objectif** : Récupère et parse l'inventaire complet du joueur  
* **Processus complexe** :  
  1. **Commande** : Envoie "Inventory" au serveur  
  2. **Parsing robuste** :  
     * Supprime les crochets  
     * Sépare par virgules  
     * Pour chaque item, sépare nom et quantité  
     * Gère les espaces et les formats variables  
  3. **Gestion d'erreurs** : Retourne `{"food": 10}` par défaut si erreur  
* **Format retour** : Dictionnaire `{nom_ressource: quantité}`  
* **Robustesse** : Gère les variations de format du serveur

**`get_resource_count(player: Player, resource: str) -> int`**

* **Objectif** : Obtient la quantité d'une ressource spécifique  
* **Mécanisme** :  
  1. Récupère l'inventaire complet  
  2. Retourne la quantité de la ressource demandée  
* **Sécurité** : Retourne 0 si la ressource n'existe pas  
* **Usage** : Fonction utilitaire très utilisée par les autres modules

### **ElevationManager**

**Responsabilités :**

* Gestion des rituels d'élévation  
* Vérification des prérequis  
* Placement des ressources

**Prérequis d'élévation :**

| Niveau | Joueurs | linemate | deraumere | sibur | mendiane | phiras | thystame |
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
| 1→2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2→3 | 2 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3→4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4→5 | 4 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5→6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6→7 | 6 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7→8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

#### **Fonctions principales :**

**`ELEVATION_REQUIREMENTS` (Constante de classe)**

* **Objectif** : Dictionnaire des prérequis pour chaque niveau  
* **Structure** : `{niveau_cible: {ressource: quantité_requise}}`  
* **Usage** : Référence pour toutes les vérifications d'élévation

**`gather_resources_for_level(player: Player, target_level: int) -> bool`**

* **Objectif** : Collecte toutes les ressources nécessaires pour un niveau  
* **Algorithme intelligent** :  
  1. **Vérification niveau** : Vérifie que le niveau cible existe  
  2. **Pour chaque ressource nécessaire** :  
     * Vérifie la quantité actuelle  
     * Collecte si insuffisante (max 10 tentatives)  
     * Exploration aléatoire si collecte échoue  
     * Vérification critique de nourriture à chaque itération  
* **Stratégie d'échec** : Mouvement aléatoire pour explorer de nouvelles zones  
* **Priorité survie** : Arrêt immédiat si nourriture critique  
* **Retour** : `True` (toujours, car c'est un processus continu)

**`place_resources_for_level(player: Player, target_level: int)`**

* **Objectif** : Place les ressources requises sur la tuile pour le rituel  
* **Processus** :  
  1. **Vérification niveau** : Vérifie la validité du niveau cible  
  2. **Placement séquentiel** : Pour chaque ressource et quantité requise  
     * Envoie commande "Set {ressource}" pour chaque unité  
* **Important** : Ne vérifie pas si le joueur possède les ressources

**`attempt_elevation(player: Player, target_level: int = None)`**

* **Objectif** : Tente le rituel d'élévation complet  
* **Processus complet** :  
  1. **Détermination niveau** : Utilise niveau+1 si non spécifié  
  2. **Placement ressources** : Place toutes les ressources nécessaires  
  3. **Incantation** : Lance la commande "Incantation"  
  4. **Actualisation** : Rafraîchit la vision avec "Look"  
* **Retour** : `True` si "Current level:" dans la réponse (succès)  
* **Note** : Le serveur gère la vérification des joueurs présents

### **CoordinationManager**

**Responsabilités :**

* Communication inter-joueurs  
* Diffusion de messages stratégiques  
* Coordination des rituels d'élévation

**Types de messages :**

* Statut du leader : `LEADER_LEVEL_X_STATUS_SEEKING_TEAM`  
* Disponibilité follower : `FOLLOWER_LEVEL_X_READY_FOOD_Y`  
* Prêt pour élévation : `READY_LEVELX_RESOURCES_COMPLETE`

#### **Fonctions principales :**

**`should_broadcast(player: Player) -> bool`**

* **Objectif** : Détermine si le joueur peut envoyer un message  
* **Logique de limitation** :  
  1. **Calcul intervalle** : 3s pour leaders, 5s pour followers  
  2. **Facteur aléatoire** : ±20% pour éviter la synchronisation  
  3. **Vérification temps** : Compare avec le dernier message envoyé  
* **Anti-spam** : Empêche la surcharge de messages  
* **Retour** : `True` si assez de temps écoulé

**`smart_broadcast(player: Player, message: str) -> bool`**

* **Objectif** : Envoie un message de manière intelligente  
* **Logique anti-duplication** :  
  1. **Vérification temporelle** : Utilise `should_broadcast()`  
  2. **Écoute récente** : Vérifie les messages récents similaires  
  3. **Envoi conditionnel** : N'envoie que si message unique  
  4. **Mise à jour timestamp** : Met à jour le temps du dernier broadcast  
* **Prévention spam** : Évite les messages redondants  
* **Retour** : `True` si message envoyé, `False` si filtré

**`listen_and_respond(player: Player)`**

* **Objectif** : Écoute les messages et adapte le comportement  
* **Système de réaction** :  
  1. **Écoute active** : Récupère les messages via NetworkManager  
  2. **Parsing keywords** :  
     * "gather" → État COLLECTING  
     * "elevate" → État ELEVATING  
     * "coordinate" → État COORDINATING  
  3. **Changement d'état** : Modifie le comportement du joueur  
* **Réactivité** : Permet l'adaptation en temps réel aux instructions d'équipe  
* **Gestion erreurs** : Propagation des KeyboardInterrupt pour arrêt propre

## **Stratégies par niveau**

### **Niveau 1 \- Survie et collecte de linemate**

**Objectif :** Collecter 1 linemate pour passer niveau 2

**Priorités :**

1. **Survie** : Maintenir nourriture \> 2  
2. **Collecte** : Rechercher linemate  
3. **Élévation** : Tenter le rituel dès que possible

**Comportement :**

* Collecte agressive de nourriture si critique  
* Recherche active de linemate  
* Élévation immédiate une fois les ressources obtenues

### **Niveau 2 \- Stratégie équilibrée**

**Objectif :** Collecter 1 linemate, 1 deraumere, 1 sibur \+ trouver un partenaire

**Priorités :**

1. **Stockage** : Maintenir 15+ unités de nourriture  
2. **Ressources** : Collecter les 3 types nécessaires  
3. **Communication** : Diffuser son statut  
4. **Exploration** : Patterns de mouvement variés

**Comportement :**

* Collecte opportuniste de nourriture  
* Recherche séquentielle des ressources manquantes  
* Broadcast périodique du statut  
* Exploration en patterns (ligne, L, aléatoire)

### **Niveau 3+ \- Leadership et coordination**

**Objectif :** Organiser l'équipe et coordonner les élévations

**Priorités :**

1. **Leadership** : Diffuser des messages d'organisation  
2. **Coordination** : Faciliter les regroupements  
3. **Support** : Aider les joueurs de niveau inférieur

**Comportement :**

* Messages de leadership fréquents  
* Exploration extensive  
* Support à la coordination d'équipe

## **Communication et coordination**

### **Système de broadcast**

Les joueurs communiquent via des messages diffusés à toute l'équipe.

**Format des messages :**

ROLE\_LEVEL\_X\_STATUS\_INFO

**Exemples :**

* `LEADER_LEVEL_2_WELL_FED_SEEKING_TEAM`  
* `FOLLOWER_LEVEL_2_READY_FOOD_12`  
* `LEVEL2_COLLECTING_RESOURCES_1234`

### **Rôles des joueurs**

* **Leader** (30% de chance) : Coordination active, messages fréquents  
* **Follower** (70% de chance) : Réponse aux directives, support

### **Fréquence des communications**

* **Leaders** : Message toutes les 3-4 secondes (± variation aléatoire)  
* **Followers** : Message toutes les 5-6 secondes  
* **Conditions** : Seulement si nourriture \> 8 et pas de message récent similaire

## **Gestion des ressources**

### **Priorisation dynamique**

L'IA adapte ses priorités selon le contexte :

1. **Survie immédiate** (nourriture \< 2\)  
2. **Stockage sécurisé** (nourriture \< 15 pour niveau 2+)  
3. **Ressources d'élévation** (selon niveau actuel)  
4. **Exploration/coordination**

### **Optimisations**

* Collecte opportuniste de nourriture pendant la recherche de ressources  
* Patterns d'exploration efficaces  
* Évitement des zones déjà explorées

## **Configuration et paramétrage**

### **Paramètres de survie**

CRITICAL\_FOOD \= 2      \# Seuil critique  
LOW\_FOOD \= 5           \# Seuil faible  
SOON\_LOW\_FOOD \= 10     \# Collecte opportuniste  
STOCKPILE\_TARGET \= 15  \# Objectif de stockage

### **Paramètres de communication**

LEADER\_BROADCAST\_INTERVAL \= 3.0    \# Secondes entre messages (leaders)  
FOLLOWER\_BROADCAST\_INTERVAL \= 5.0  \# Secondes entre messages (followers)  
LEADER\_PROBABILITY \= 0.3           \# Chance d'être leader

### **Paramètres d'exploration**

MAX\_SEARCH\_ATTEMPTS \= 50           \# Tentatives max de recherche  
MOVEMENT\_PATTERNS \= 3              \# Nombre de patterns de mouvement

## **Débogage et maintenance**

### **Logs et monitoring**

L'IA affiche des informations de débogage pour :

* Connexions réseau  
* Changements de niveau  
* États critiques de nourriture  
* Erreurs de communication

### **Gestion des erreurs**

* **Déconnexion réseau** : Arrêt propre avec nettoyage  
* **Timeout** : Retry automatique avec backoff  
* **Commandes invalides** : Logging et continuation  
* **Mort du joueur** : Détection et arrêt

### **Interruption utilisateur**

* **Ctrl+C** : Arrêt gracieux avec fermeture des sockets  
* **Nettoyage automatique** : Libération des ressources réseau

### **Indicateurs de performance**

* Temps de survie du joueur  
* Niveau atteint  
* Nombre de ressources collectées  
* Messages de coordination envoyés

## **Conseils d'utilisation**

### **Optimisation des performances**

1. **Serveur local** : Utilisez localhost pour réduire la latence  
2. **Équipes équilibrées** : 4-6 joueurs par équipe optimal  
3. **Monitoring** : Surveillez les logs pour détecter les problèmes

### **Paramétrage avancé**

1. **Agressivité** : Réduisez les seuils de nourriture pour plus d'agressivité  
2. **Communication** : Ajustez les intervalles selon la taille de l'équipe  
3. **Exploration** : Modifiez les patterns selon la taille de la carte

### **Troubleshooting**

* **Connexion refusée** : Vérifiez que le serveur est démarré  
* **Timeout fréquents** : Réduisez la charge réseau  
* **Joueurs bloqués** : Redémarrez avec des paramètres différents

Cette IA offre une base solide pour jouer à Zappy de manière autonome et intelligente, avec des stratégies adaptées à chaque niveau et une coordination d'équipe efficace.

