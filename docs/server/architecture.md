# Architecture du Serveur Zappy

Le serveur Zappy est écrit en C (norme GNU17) et utilise une architecture asynchrone non-bloquante basée sur la fonction système `poll()`. Il est conçu pour être performant, gérant la simulation de dizaines de clients IA et un client graphique sans ralentissement notable.

## 1. La Boucle Principale et l'utilisation de `poll()`

L'intégralité du réseau repose sur le multiplexage d'entrées/sorties via `poll()`. 
Le serveur maintient un tableau dynamique de structures `struct pollfd` (`_fds`) et un tableau parallèle de métadonnées `pollfd_meta_t` (`_fdsm`).

Chaque itération de la boucle principale du serveur effectue les actions suivantes :
1. **Attente d'événements :** Le programme est bloqué dans `poll()` en attendant une activité sur un socket (nouvelle connexion entrante ou données reçues). Le timeout du `poll()` est calculé dynamiquement (voir *Gestion du temps*).
2. **Acceptation des clients :** Si le socket d'écoute (FD_SERVER) reçoit un événement `POLLIN`, un nouveau socket client est accepté.
3. **Lecture des données :** Si un client (FD_CONTROL) reçoit des données, elles sont ajoutées à son buffer circulaire interne.
4. **Exécution des commandes :** Si une ligne entière se trouve dans le buffer, elle est dépilée, parsée, puis mise dans la file d'attente d'exécution du joueur.
5. **Nettoyage :** Si un client se déconnecte, ses descripteurs de fichiers sont fermés, et la mémoire correspondante (métadonnées, buffer, joueur) est libérée via `pop_poller`.

## 2. Structure `connection_t`

L'état global du réseau et du jeu est contenu dans la structure `connection_t` (définie dans `myserver.h`), qui comprend :
- Les sockets ouverts et leurs métadonnées.
- Les arguments passés au lancement (port, taille de carte, fréquences).
- `map` : L'état de la carte du monde en temps réel (ressources, dimensions).
- `zappy` : L'état du jeu et les entités (joueurs, œufs, équipes).

## 3. Buffering Circulaire et Parsing

Toutes les communications TCP sont asynchrones, ce qui signifie que des paquets peuvent arriver découpés. 
Le serveur stocke les données brutes dans le champ `buffer` de la structure métadonnées `pollfd_meta_t` du socket. 

La fonction `get_buffer` analyse ce flux et extrait les chaînes terminées par `\n`. Si la chaîne n'est pas complète, le serveur la laisse en attente jusqu'à la prochaine lecture sur le réseau. Dès qu'une commande est complète, elle est traitée et envoyée au système d'invocation (`get_command_invoke`).

## 4. Architecture de commandes via callbacks (Handler)

L'implémentation des commandes est modulaire. Un grand tableau statique `cmd_t` associe :
- Une chaîne de caractères (par ex : `"Forward"`)
- Une fonction de gestion / callback (ex: `protocol_ia_move()`)
- Un coût temporel (`UNIT_FORWARD`)
- Le nombre d'arguments attendus.

Quand un paquet valide est reçu, le parseur trouve l'index de la commande associée et déclenche le gestionnaire approprié sans recourir à des suites de `if/else` interminables.

## 5. Triggers d'événements (Le lien Serveur <-> GUI)

Chaque action majeure effectuée par un client IA va déclencher une fonction *Trigger* (située dans `src/commands/trigger/`).
Les Triggers s'occupent d'envoyer les notifications au client Graphique. 

Exemple : 
- L'IA appelle `Forward`. 
- Le serveur met à jour les coordonnées du joueur.
- Le serveur invoque `gui_player_move(connection_t *srv, uint64_t player_id)`.
- Ce trigger formate la trame `ppo n X Y O` et l'envoie en boucle à tous les sockets identifiés comme `GRAPHIC`.
