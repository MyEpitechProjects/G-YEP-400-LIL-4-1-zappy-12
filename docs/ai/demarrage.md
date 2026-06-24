# Démarrage et connexion

Cette page couvre le tout début de la vie d'une IA : l'analyse de ses arguments
(`args_parser.py`), l'établissement de la connexion avec le serveur (`connection.py`) et
l'orchestration générale (`main.py`).

---

## 1. Le point d'entrée : `main.py`

`main.py` est le chef d'orchestre. Sa fonction `main` reçoit la liste des arguments
(`sys.argv`, l'équivalent du `char **av` en C/C++) et enchaîne quatre étapes :

1. **Analyse des arguments** via `parse_arguments` → port, nom d'équipe, machine.
2. **Connexion au serveur** via `first_connection` → socket, fichier socket, dimensions du
   monde, nombre de places libres.
3. **Création de l'agent** : une instance de `Trantorian` est construite avec le nom de
   l'équipe et les dimensions de la carte. On lui attache aussi le port, la machine et le
   binaire, informations dont la reproduction aura besoin plus tard pour relancer des copies.
4. **Lancement de la boucle** via `run_ai`, encadré par une gestion propre de l'arrêt.

```python
trantorian = Trantorian(team_name=name, map_width=int(width_str),
                        map_height=int(height_str), initial_slots=client_num)
trantorian.port = port
trantorian.machine = machine
trantorian.binary = sys.argv[0]
```

L'arrêt est soigné : un `try / except KeyboardInterrupt / finally` garantit que la socket est
toujours fermée, même en cas d'interruption au clavier (Ctrl+C) ou d'erreur. C'est ce qui évite
de laisser des connexions ouvertes.

---

## 2. L'analyse des arguments : `args_parser.py`

La fonction `parse_arguments` valide rigoureusement la ligne de commande attendue :

```
./zappy_ai -p port -n name -h machine
```

Elle gère plusieurs cas de figure :

| Situation | Comportement |
|-----------|--------------|
| Aucun argument | Affiche l'usage sur `stderr` et quitte avec le code **84**. |
| `--help` seul | Affiche l'usage sur la sortie standard et quitte avec le code **0**. |
| `-p port -n name` | Forme valide ; la machine vaut `localhost` par défaut. |
| `-p port -n name -h machine` | Forme valide complète. |
| Toute autre forme | Affiche l'usage sur `stderr` et quitte avec le code **84**. |

Deux validations méritent l'attention :

- **Le port doit être un entier.** La conversion `int(...)` est protégée par un `try / except
  ValueError` qui se solde par une sortie en code 84 si la valeur n'est pas numérique.
- **Le nom `GRAPHIC` est interdit.** Ce nom est réservé par le protocole à l'interface
  graphique (le GUI s'authentifie ainsi auprès du serveur). Une IA qui tenterait de l'utiliser
  est rejetée explicitement.

Le code **84** est la convention de sortie en erreur utilisée tout au long du projet ; le code quand a lui **0** signale un arrêt normal.

---

## 3. La connexion initiale : `connection.py`

La fonction `first_connection` ouvre une socket TCP puis déroule la **poignée de main** imposée
par le protocole. Le dialogue attendu est le suivant (le serveur parle d'abord) :

```
serveur  → WELCOME
client   → TEAM-NAME
serveur  → CLIENT-NUM        (nombre de places libres dans l'équipe)
serveur  → X Y               (dimensions du monde)
```

Concrètement, le code :

1. Crée une socket `AF_INET` / `SOCK_STREAM` et se connecte à `(machine, port)`.
2. Enveloppe la socket dans un **objet fichier** (`makefile('rw')`). Cela permet de lire et
   d'écrire des lignes terminées par `\n` simplement avec `readline()` et `write()`, sans
   manipuler les octets à la main.
3. Lit `WELCOME`. Si la ligne est vide, le serveur a fermé la connexion immédiatement → sortie
   en erreur.
4. Envoie le nom de l'équipe, suivi de `\n`, puis `flush()` pour pousser les octets tout de suite.
5. Lit `CLIENT-NUM`. Si la réponse est `ko`, l'équipe est pleine ou inexistante → sortie en
   erreur après fermeture propre de la socket.
6. Lit les dimensions `X Y` du monde.

La fonction renvoie le quadruplet `(client_socket, socket_file, dimensions, client_num)`. Toute
exception réseau imprévue est interceptée et provoque une sortie en code 84, ce qui évite de
laisser le programme dans un état incohérent.

**Pourquoi un objet fichier plutôt que la socket brute ?** Le protocole de Zappy est entièrement orienté **lignes** : chaque message se termine par `\n`. L'objet fichier fournit un tampon de lecture qui découpe automatiquement le flux en lignes, ce qui colle parfaitement au protocole et simplifie tout le reste du code réseau.

---

## 4. Que représente `CLIENT-NUM` ?

`CLIENT-NUM` est le nombre de **places libres** dans l'équipe au moment de la connexion. Sur la
carte, chaque place correspond à un **œuf** en attente d'éclore. Cette valeur initiale est
stockée dans l'agent sous le nom `available_slots` : elle servira plus tard au roi pour décider
combien de nouveaux clients lancer afin d'occuper les œufs disponibles (voir
[La reproduction](reproduction.md)).

---

[← Architecture et flux de données](architecture.md) · [Sommaire](ai_doc_main.md) · [Suivant : Boucle principale et communication réseau →](boucle_reseau.md)