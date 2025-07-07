# Protocole Réseau Zappy

## Vue d'ensemble

Le protocole Zappy utilise TCP/IP avec des messages textuels ASCII. Tous les messages se terminent par `\n`. Il existe deux protocoles distincts : un pour les clients AI et un pour le client GUI.

## Architecture réseau

```
┌──────────────┐         TCP/IP          ┌──────────────┐
│   Server     │ ◄─────────────────────► │   AI Client  │
│              │                         └──────────────┘
│   Port X     │         TCP/IP          ┌──────────────┐
│              │ ◄─────────────────────► │  GUI Client  │
└──────────────┘                         └──────────────┘
```

## Handshake de connexion

### Client AI

```
Client connects to server
S: "WELCOME\n"
C: "team_name\n"           # Nom de l'équipe
S: "1\n"                   # Nombre de slots disponibles
S: "10 20\n"               # Dimensions du monde (X Y)
```

### Client GUI

```
Client connects to server
S: "WELCOME\n"
C: "GRAPHIC\n"             # Mot clé spécial
[Server commence à envoyer l'état initial]
```

## Buffer et gestion des messages

### Structure de buffer circulaire

```c
typedef struct circular_buffer_s {
    char *data;
    size_t size;
    size_t read_pos;
    size_t write_pos;
    size_t used;
} circular_buffer_t;
```

### Algorithme de lecture

```
1. Recevoir données dans buffer
2. Chercher '\n'
3. Si trouvé:
   - Extraire message complet
   - Avancer read_pos
   - Parser et traiter
4. Si pas trouvé:
   - Attendre plus de données
   - Vérifier overflow
```

## Protocole AI détaillé

### Commandes client → serveur

| Commande | Format | Description | Temps |
|----------|--------|-------------|--------|
| Forward | `Forward\n` | Avancer d'une case | 7/f |
| Right | `Right\n` | Tourner 90° droite | 7/f |
| Left | `Left\n` | Tourner 90° gauche | 7/f |
| Look | `Look\n` | Observer environnement | 7/f |
| Inventory | `Inventory\n` | Voir inventaire | 1/f |
| Broadcast | `Broadcast text\n` | Diffuser message | 7/f |
| Connect_nbr | `Connect_nbr\n` | Slots équipe disponibles | 0 |
| Fork | `Fork\n` | Pondre un œuf | 42/f |
| Eject | `Eject\n` | Éjecter les joueurs | 7/f |
| Take | `Take object\n` | Prendre objet | 7/f |
| Set | `Set object\n` | Poser objet | 7/f |
| Incantation | `Incantation\n` | Commencer élévation | 300/f |

### Réponses serveur → client

#### Look
```
Format: [tile0, tile1, tile2, ...]\n

Exemple level 1:
[player food, linemate, food deraumere player]\n

Numérotation des tiles par niveau:
Level 1: 4 tiles (0-3)
Level 2: 9 tiles (0-8)
Level 3: 16 tiles (0-15)
etc.
```

#### Inventory
```
Format: [food n, linemate n, deraumere n, sibur n, mendiane n, phiras n, thystame n]\n

Exemple:
[food 10, linemate 2, deraumere 0, sibur 1, mendiane 0, phiras 0, thystame 0]\n
```

#### Broadcast reçu
```
Format: message K, text\n

K = direction (0-8)
0 = même case
1 = devant
2-8 = sens horaire

Exemple:
message 3, hello world\n
```

#### Élévation
```
Début: "Elevation underway\n"
Succès: "Current level: K\n"
Échec: "ko\n"
```

#### Éjection
```
Si éjecté: "eject: K\n"
K = direction d'où vient l'éjection
```

#### Mort
```
"dead\n"
[Connexion fermée]
```

## Protocole GUI détaillé

### Messages serveur → GUI

| Commande | Format | Description |
|----------|--------|-------------|
| msz | `msz X Y\n` | Taille de la carte |
| bct | `bct X Y q0 q1 q2 q3 q4 q5 q6\n` | Contenu d'une case |
| tna | `tna N\n` | Nom d'une équipe |
| pnw | `pnw #n X Y O L N\n` | Nouveau joueur |
| ppo | `ppo #n X Y O\n` | Position joueur |
| plv | `plv #n L\n` | Level joueur |
| pin | `pin #n X Y q0 q1 q2 q3 q4 q5 q6\n` | Inventaire joueur |
| pex | `pex #n\n` | Joueur expulse |
| pbc | `pbc #n M\n` | Broadcast joueur |
| pic | `pic X Y L #n #n ...\n` | Début incantation |
| pie | `pie X Y R\n` | Fin incantation |
| pfk | `pfk #n\n` | Joueur pond |
| pdr | `pdr #n i\n` | Joueur drop ressource |
| pgt | `pgt #n i\n` | Joueur prend ressource |
| pdi | `pdi #n\n` | Mort joueur |
| enw | `enw #e #n X Y\n` | Nouvel œuf |
| ebo | `ebo #e\n` | Éclosion œuf |
| edi | `edi #e\n` | Mort œuf |
| sgt | `sgt T\n` | Time unit actuel |
| seg | `seg N\n` | Fin du jeu |
| smg | `smg M\n` | Message serveur |
| suc | `suc\n` | Commande inconnue |
| sbp | `sbp\n` | Mauvais paramètres |

### Commandes GUI → serveur

| Commande | Format | Description |
|----------|--------|-------------|
| msz | `msz\n` | Demande taille carte |
| bct | `bct X Y\n` | Demande contenu case |
| mct | `mct\n` | Demande toute la carte |
| tna | `tna\n` | Demande noms équipes |
| ppo | `ppo #n\n` | Demande position joueur |
| plv | `plv #n\n` | Demande level joueur |
| pin | `pin #n\n` | Demande inventaire joueur |
| sgt | `sgt\n` | Demande time unit |
| sst | `sst T\n` | Modifie time unit |

## Formats détaillés

### Orientation (O)
- 1 = Nord (haut)
- 2 = Est (droite)
- 3 = Sud (bas)
- 4 = Ouest (gauche)

### Ressources (i)
- 0 = food (nourriture)
- 1 = linemate
- 2 = deraumere
- 3 = sibur
- 4 = mendiane
- 5 = phiras
- 6 = thystame

### Résultat incantation (R)
- 0 = échec
- 1 = succès

## Séquences typiques

### Connexion et jeu AI

```sequence
AI->Server: TCP Connect
Server->AI: WELCOME\n
AI->Server: team1\n
Server->AI: 3\n
Server->AI: 20 20\n
AI->Server: Look\n
Server->AI: [player food, , linemate]\n
AI->Server: Forward\n
Server->AI: ok\n
AI->Server: Take food\n
Server->AI: ok\n
```

### Connexion GUI et initialisation

```sequence
GUI->Server: TCP Connect
Server->GUI: WELCOME\n
GUI->Server: GRAPHIC\n
Server->GUI: msz 20 20\n
Server->GUI: sgt 100\n
Server->GUI: tna team1\n
Server->GUI: tna team2\n
[... envoi de tout l'état initial ...]
```

### Élévation complète

```sequence
AI->Server: Incantation\n
Server->AI: Elevation underway\n
Server->GUI: pic X Y L #n1 #n2 ...\n
[... 300/f time units ...]
Server->AI: Current level: 3\n
Server->GUI: pie X Y 1\n
Server->GUI: plv #n1 3\n
Server->GUI: plv #n2 3\n
```

## Gestion des buffers

### Taille recommandée des buffers

```c
#define BUFFER_SIZE 4096        // Buffer circulaire
#define MAX_MESSAGE_SIZE 1024   // Taille max d'un message
#define MAX_PENDING_COMMANDS 10 // Max commandes en attente
```

### Stratégie de parsing

```c
typedef enum {
    PARSE_NEED_MORE,    // Message incomplet
    PARSE_COMPLETE,     // Message complet trouvé
    PARSE_ERROR         // Erreur de format
} parse_result_t;

parse_result_t parse_message(circular_buffer_t *buf, message_t *msg) {
    // 1. Chercher '\n'
    size_t newline_pos = find_newline(buf);
    if (newline_pos == NOT_FOUND) {
        return PARSE_NEED_MORE;
    }
    
    // 2. Vérifier taille
    if (newline_pos > MAX_MESSAGE_SIZE) {
        return PARSE_ERROR;
    }
    
    // 3. Extraire message
    extract_message(buf, msg, newline_pos);
    
    return PARSE_COMPLETE;
}
```

## Optimisations réseau

### 1. Nagle's Algorithm

```c
// Désactiver pour réduire latence
int flag = 1;
setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int));
```

### 2. Buffer sizes

```c
// Augmenter buffers système
int size = 65536;
setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
```

### 3. Keep-alive

```c
// Activer keep-alive
int enable = 1;
setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int));
```

## Gestion d'erreurs réseau

### Erreurs courantes

| Erreur | Cause | Action |
|--------|-------|--------|
| EAGAIN | Buffer plein/vide | Réessayer plus tard |
| ECONNRESET | Client déconnecté | Nettoyer et fermer |
| EPIPE | Broken pipe | Ignorer ou fermer |
| EINTR | Signal reçu | Réessayer |

### Pattern de gestion

```c
ssize_t safe_send(int fd, const char *data, size_t len) {
    size_t sent = 0;
    
    while (sent < len) {
        ssize_t ret = send(fd, data + sent, len - sent, MSG_NOSIGNAL);
        
        if (ret == -1) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;  // Réessayer
            }
            return -1;     // Vraie erreur
        }
        
        sent += ret;
    }
    
    return sent;
}
```

## Validation et sécurité

### Validation des entrées

```c
bool validate_command(const char *cmd, size_t len) {
    // 1. Vérifier longueur
    if (len > MAX_COMMAND_LENGTH) return false;
    
    // 2. Vérifier caractères ASCII printables
    for (size_t i = 0; i < len; i++) {
        if (!isprint(cmd[i]) && cmd[i] != '\n') {
            return false;
        }
    }
    
    // 3. Vérifier format spécifique
    return validate_specific_format(cmd);
}
```

### Limites et timeouts

```c
typedef struct client_limits_s {
    size_t max_pending_commands;     // 10
    size_t max_bytes_per_second;     // Anti-flood
    time_t idle_timeout;             // Déconnexion si inactif
    size_t max_message_size;         // Taille max message
} client_limits_t;
```

## Debugging du protocole

### Logging des messages

```c
void log_protocol(const char *direction, int client_id, 
                  const char *msg, size_t len) {
    char timestamp[32];
    get_timestamp(timestamp);
    
    fprintf(log_file, "[%s] %s Client %d: ",
            timestamp, direction, client_id);
    
    // Échapper les caractères non-printables
    for (size_t i = 0; i < len; i++) {
        if (isprint(msg[i])) {
            fputc(msg[i], log_file);
        } else {
            fprintf(log_file, "\\x%02X", (unsigned char)msg[i]);
        }
    }
    
    fputc('\n', log_file);
}
```

### Outils de debug

1. **tcpdump/Wireshark**
   ```bash
   tcpdump -i lo -A 'tcp port 4242'
   ```

2. **netcat pour tests**
   ```bash
   echo -e "GRAPHIC\n" | nc localhost 4242
   ```

3. **strace pour syscalls**
   ```bash
   strace -e network ./zappy_server
   ```

## Tests du protocole

### Tests unitaires

```c
void test_parse_look_response() {
    const char *response = "[player food, , linemate]\n";
    look_result_t result;
    
    assert(parse_look(response, &result) == SUCCESS);
    assert(result.tiles[0].has_player == true);
    assert(result.tiles[0].resources[FOOD] == 1);
    assert(result.tiles[2].resources[LINEMATE] == 1);
}
```

### Tests d'intégration

```python
def test_elevation_sequence():
    # Setup
    clients = [connect_client(f"team{i}") for i in range(4)]
    
    # Move tous au même endroit
    for c in clients:
        move_to_position(c, 5, 5)
    
    # Déposer ressources
    deposit_resources_for_elevation(clients[0], level=3)
    
    # Initier élévation
    clients[0].send("Incantation\n")
    response = clients[0].recv()
    assert response == "Elevation underway\n"
    
    # Attendre fin
    time.sleep(3)  # 300/f avec f=100
    
    # Vérifier succès
    response = clients[0].recv()
    assert response.startswith("Current level:")
```

## Performance et benchmarks

### Métriques importantes

1. **Latence moyenne par commande**
2. **Throughput (messages/seconde)**
3. **Utilisation CPU par client**
4. **Utilisation mémoire**
5. **Temps de parsing moyen**

### Optimisations serveur

```c
// Batch sending pour GUI
void send_gui_updates(gui_client_t *client) {
    // Accumuler updates dans buffer
    for (each update) {
        append_to_buffer(client->out_buf, update);
    }
    
    // Envoyer en une fois
    send_buffer(client->fd, client->out_buf);
}
```

## Checklist d'implémentation

### Serveur
- [ ] Socket TCP avec SO_REUSEADDR
- [ ] Non-blocking sockets
- [ ] Poll avec timeout dynamique
- [ ] Circular buffers pour I/O
- [ ] Parser robuste avec validation
- [ ] Gestion des déconnexions
- [ ] Limite de 10 commandes
- [ ] Logging des protocoles

### Client AI
- [ ] Connexion TCP robuste
- [ ] Handshake complet
- [ ] Parser de réponses
- [ ] Queue de commandes
- [ ] Gestion des erreurs
- [ ] Reconnexion automatique

### Client GUI
- [ ] Thread réseau séparé
- [ ] Parser de messages GUI
- [ ] Buffer pour gros volumes
- [ ] Synchronisation thread-safe
- [ ] Gestion des mises à jour partielles