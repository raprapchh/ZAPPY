
# Zappy - Documentation Utilisateur

**EPITECH Zappy Project – Juin 2025**

## Table des matières

1. [Introduction](#1-introduction)  
    1.1 [Vue d’ensemble](#11-vue-densemble)  
    1.2 [Public visé](#12-public-visé)  
    1.3 [Prérequis](#13-prérequis)  
2. [Installation](#2-installation)  
    2.1 [Compilation du projet](#21-compilation-du-projet)  
    2.2 [Vérification de l’installation](#22-vérification-de-linstallation)  
3. [Architecture du système](#3-architecture-du-système)  
    3.1 [Serveur de jeu](#31-serveur-de-jeu)  
    3.2 [Interface graphique (GUI)](#32-interface-graphique-gui)  
    3.3 [Intelligence artificielle (AI)](#33-intelligence-artificielle-ai)  
4. [Utilisation](#4-utilisation)  
    4.1 [Démarrage du serveur](#41-démarrage-du-serveur)  
    4.2 [Lancement de l’interface graphique](#42-lancement-de-linterface-graphique)  
    4.3 [Lancement de l’intelligence artificielle](#43-lancement-de-lintelligence-artificielle)  
    4.4 [Configuration des équipes](#44-configuration-des-équipes)  
5. [Règles du jeu](#5-règles-du-jeu)  
    5.1 [Objectif](#51-objectif)  
    5.2 [Monde de jeu](#52-monde-de-jeu)  
    5.3 [Ressources](#53-ressources)  
    5.4 [Élévation des joueurs](#54-élévation-des-joueurs)  
6. [Interface graphique](#6-interface-graphique)  
    6.1 [Mode 2D](#61-mode-2d)  
    6.2 [Mode 3D](#62-mode-3d)  
    6.3 [Contrôles et navigation](#63-contrôles-et-navigation)  
7. [Protocole de communication](#7-protocole-de-communication)  
    7.1 [Commandes AI](#71-commandes-ai)  
    7.2 [Commandes GUI](#72-commandes-gui)  
8. [Dépannage et FAQ](#8-dépannage-et-faq)  
    8.1 [Problèmes courants](#81-problèmes-courants)  
    8.2 [Questions fréquentes](#82-questions-fréquentes)  
9. [Sécurité et bonnes pratiques](#9-sécurité-et-bonnes-pratiques)  
10. [Support et ressources](#10-support-et-ressources)  
11. [Glossaire](#11-glossaire)  
12. [Auteurs](#12-auteurs)

---

## 1. Introduction

### 1.1 Vue d’ensemble

Zappy est un projet de jeu multijoueur en réseau développé dans le cadre du cursus EPITECH. Il consiste à concevoir un environnement simulé dans lequel des intelligences artificielles (clients AI) interagissent au sein d’un monde partagé, géré par un serveur central. Ce monde est visualisable grâce à deux interfaces graphiques (GUI), l’une en 2D et l’autre en 3D, permettant une représentation en temps réel de l’état du jeu et des actions des entités.

### 1.2 Public visé

Cette documentation s’adresse :
- aux étudiants développeurs,
- aux examinateurs ou encadrants,
- à toute personne souhaitant comprendre ou faire fonctionner le projet.

### 1.3 Prérequis

Un système UNIX (Linux, macOS).  
Un compilateur C et C++ (gcc, g++).  
L'interpréteur Python 3.  
Outils de compilation : make.  
Bibliothèques nécessaires : SFML (pour le GUI 2D), raylib (pour le GUI 3D).  
Une bonne compréhension des sockets réseau, des threads, et de la programmation orientée objet (pour le client graphique).  
Familiarité avec les Makefiles multi-langages (C, C++, Python).  

---

## 2. Installation

### 2.1 Compilation du projet

```bash
git clone https://github.com/<votre_repo>/zappy.git
cd zappy
make
```

Cela va compiler :
- le serveur (`zappy_server`),
- l’interface graphique en 2D avec **SFML** (`zappy_gui`),
- l’interface graphique en 3D avec **raylib** (`zappy_gui` en mode 3D),
- les clients autonomes AI (`zappy_ai`).

### 2.2 Vérification de l’installation

Vérifiez que les exécutables suivants sont générés :
- `zappy_server`
- `zappy_gui`
- `zappy_ai`

---

## 3. Architecture du système

### 3.1 Serveur de jeu

Le serveur gère :
- les connexions clients (AI et GUI),
- l’état du monde,
- la logique de jeu et la synchronisation.

### 3.2 Interface graphique (GUI)

GUI affiche :
- une carte interactive du jeu,
- l’état des joueurs et ressources,
- les événements en temps réel (spawn, mort, elevation...),
- en 2D avec SFML ou en 3D avec raylib.

### 3.3 Intelligence artificielle (AI)

Les clients AI sont autonomes :
- se connectent au serveur,
- prennent des décisions via un protocole défini,
- interagissent avec le monde et les autres joueurs.

---

## 4. Utilisation

### 4.1 Démarrage du serveur

```bash
./zappy_server -p <port> -x <width> -y <height> -n <team_1> <team_2> [...] -c <clients_nb> -f <freq>
```

Exemple :
```bash
./zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 3 -f 100
```

### 4.2 Lancement de l’interface graphique

```bash
./zappy_gui -p <port> -h <ip_du_serveur> [-g 3d]
```

- `-g 3d` : lance le mode 3D avec raylib. Si absent, le mode 2D avec SFML est utilisé.

Exemple :
```bash
./zappy_gui -p 4242 -h localhost
```

### 4.3 Lancement de l’intelligence artificielle

```bash
./zappy_ai -p <port> -n <nom_equipe> -h <ip_du_serveur>
```

Exemple :
```bash
./zappy_ai -p 4242 -n team1 -h localhost
```

### 4.4 Configuration des équipes

Chaque équipe est définie via l’argument `-n`. Le nombre maximal de clients par équipe est défini avec `-c`.

---

## 5. Règles du jeu

### 5.1 Objectif

Être la première équipe à élever 6 de ses membres au niveau 8.

### 5.2 Monde de jeu

- Carte 2D torique (le bord droit rejoint le bord gauche).
- Cases contenant ressources, joueurs, œufs.

### 5.3 Ressources

| Ressource | Densité | Utilisation                      |
| --------- | ------- | -------------------------------- |
| Food      | 0.5     | Survie (1 = 126 unités de temps) |
| Linemate  | 0.3     | Élévation                        |
| Deraumere | 0.15    | Élévation                        |
| Sibur     | 0.1     | Élévation                        |
| Mendiane  | 0.1     | Élévation                        |
| Phiras    | 0.08    | Élévation                        |
| Thystame  | 0.05    | Élévation (rare)                 |

### 5.4 Élévation des joueurs

| Niveau | Joueurs | Linemate | Deraumere | Sibur | Mendiane | Phiras | Thystame |
| ------ | ------- | -------- | --------- | ----- | -------- | ------ | -------- |
| 1→2    | 1       | 1        | 0         | 0     | 0        | 0      | 0        |
| 2→3    | 2       | 1        | 1         | 1     | 0        | 0      | 0        |
| 3→4    | 2       | 2        | 0         | 1     | 0        | 2      | 0        |
| 4→5    | 4       | 1        | 1         | 2     | 0        | 1      | 0        |
| 5→6    | 4       | 1        | 2         | 1     | 3        | 0      | 0        |
| 6→7    | 6       | 1        | 2         | 3     | 0        | 1      | 0        |
| 7→8    | 6       | 2        | 2         | 2     | 2        | 2      | 1        |

---

## 6. Interface graphique

### 6.1 Mode 2D

Vue classique en grille avec **SFML**.

### 6.2 Mode 3D

Vue isométrique ou en perspective avec **raylib**.

### 6.3 Contrôles et navigation

- Flèches ou clics : déplacement sur la carte
- Scroll : zoom
- Clic sur joueur : afficher son état

---

## 7. Protocole de communication

### 7.1 Commandes AI

Les commandes envoyées par les clients IA sont transmises au serveur sous forme de chaînes de caractères terminées par un caractère de saut de ligne (\n). Chaque commande possède un temps d'exécution spécifique exprimé en unités de temps/frequence (f) et retourne une réponse précise.

| **Action**                         | **Commande**          | **Temps (f)** | **Réponse**                      |
| ---------------------------------- | --------------------- | ------------- | -------------------------------- |
| Avancer d’une case                 | `Forward`             | 7/f           | `ok`                             |
| Tourner à droite (90°)             | `Right`               | 7/f           | `ok`                             |
| Tourner à gauche (90°)             | `Left`                | 7/f           | `ok`                             |
| Voir les tuiles autour             | `Look`                | 7/f           | `[tile1, tile2, ...]`            |
| Vérifier l’inventaire              | `Inventory`           | 1/f           | `[linemate n, deraumere n, ...]` |
| Envoyer un message                 | `Broadcast <message>` | 7/f           | `ok`                             |
| Connexions restantes pour l’équipe | `Connect_nbr`         | 0             | `<nombre>`                       |
| Pondre un œuf                      | `Fork`                | 42/f          | `ok`                             |
| Éjecter les joueurs de la case     | `Eject`               | 7/f           | `ok` ou `ko`                     |
| Prendre un objet                   | `Take <object>`       | 7/f           | `ok` ou `ko`                     |
| Poser un objet                     | `Set <object>`        | 7/f           | `ok` ou `ko`                     |
| Démarrer une incantation           | `Incantation`         | 300/f         | `Elevation underway` / `ko`      |
| Mort du joueur                     | —                     | —             | `dead`                           |

Exemples de commandes :

Forward  
Right, Left  
Look  
Inventory  
Broadcast Hello  
Connect_nbr  
Fork  
Eject  
Take linemate  
Set food  
Incantation  

### 7.2 Commandes GUI

L'interface graphique (2D/3D) reçoit en temps réel des événements envoyés par le serveur. Ces événements permettent d’afficher l’état du monde, les actions des joueurs, et la progression du jeu.

| **Commande**                      | **Requête associée (client → serveur)** | **Description (réponse serveur → client)**                       |
| --------------------------------- | --------------------------------------- | ---------------------------------------------------------------- |
| `msz X Y`                         | `msz`                                   | Dimensions de la carte (X colonnes, Y lignes)                    |
| `bct X Y q0 q1 q2 q3 q4 q5 q6`    | `bct X Y`                               | Contenu de la case (X, Y)                                        |
| `mct` → *plusieurs* `bct`         | `mct`                                   | Contenu de toute la carte (toutes les cases)                     |
| `tna N`                           | `tna`                                   | Nom d'une équipe (répété pour chaque équipe)                     |
| `pnw #n X Y O L N`                | —                                       | Connexion d’un nouveau joueur (#id, pos, orientation, lvl, team) |
| `ppo #n X Y O`                    | `ppo #n`                                | Position d’un joueur                                             |
| `plv #n L`                        | `plv #n`                                | Niveau actuel d’un joueur                                        |
| `pin #n X Y q0 q1 q2 q3 q4 q5 q6` | `pin #n`                                | Inventaire d’un joueur                                           |
| `pex #n`                          | —                                       | Expulsion déclenchée par un joueur                               |
| `pbc #n M`                        | —                                       | Message diffusé par un joueur (`Broadcast`)                      |
| `pic X Y L #n #n...`              | —                                       | Début d’une incantation (coordonnées, niveau, joueurs impliqués) |
| `pie X Y R`                       | —                                       | Fin de l’incantation (succès ou échec)                           |
| `pfk #n`                          | —                                       | Ponte d’un œuf par un joueur                                     |
| `pdr #n i`                        | —                                       | Pose d’une ressource `i` par un joueur                           |
| `pgt #n i`                        | —                                       | Ramassage d’une ressource `i` par un joueur                      |
| `pdi #n`                          | —                                       | Mort d’un joueur                                                 |
| `enw #e #n X Y`                   | —                                       | Ponte d’un œuf (#e) par un joueur (#n) à la position (X, Y)      |
| `ebo #e`                          | —                                       | Connexion d’un client IA né d’un œuf                             |
| `edi #e`                          | —                                       | Mort d’un œuf                                                    |
| `sgt T`                           | `sgt`                                   | Récupération de la fréquence du serveur                          |
| `sst T`                           | `sst T`                                 | Modification de la fréquence du serveur                          |
| `seg N`                           | —                                       | Fin de la partie, l’équipe `N` a gagné                           |
| `smg M`                           | —                                       | Message envoyé par le serveur (texte brut)                       |
| `suc`                             | —                                       | Commande inconnue                                                |
| `sbp`                             | —                                       | Paramètre de commande invalide                                   |

GUI reçoit notamment les événements suivants (principaux) :

pnw : connexion d’un nouveau joueur  
ppo : mise à jour de la position d’un joueur  
bct : mise à jour du contenu d’une case  
seg : signal de fin de partie (victoire d'une équipe)  

---

## 8. Dépannage et FAQ

### 8.1 Problèmes courants

| Problème                  | Solution                          |
|---------------------------|-----------------------------------|
| Le client ne se connecte pas | Vérifier IP/port, pare-feu     |
| Le GUI ne s’affiche pas   | Vérifier dépendances SFML/raylib  |

### 8.2 Questions fréquentes

- **Q : Peut-on changer la taille de la carte ?**  
  **R :** Oui, via `-x` et `-y`.

- **Q : L’élévation échoue ?**  
  **R :** Vérifiez le nombre de joueurs et les ressources nécessaires.

---

## 9. Sécurité et bonnes pratiques

- Ne jamais exécuter le serveur avec les droits root.
- Vérifier que les clients ne spamment pas le serveur.
- Loguer les commandes pour faciliter le debug.

---

## 10. Support et ressources

- Dépôt GitHub du projet
- Documentation officielle EPITECH
- Tutoriels réseaux (sockets en C)

---

## 11. Glossaire

- **Client AI** : Programme autonome contrôlant un joueur
- **GUI** : Interface graphique
- **Incantation** : Action d’élévation de niveau
- **Tile** : Case du monde de jeu

---

## 12. Auteurs

- Raphaël Chanliongco
- Hubert Touraine  
- Jean-Baptiste Boshra  
- Hugo Dufour
- Alexandre de Angelis

---

*Documentation réalisée dans le cadre du projet EPITECH Zappy – Juin 2025.*