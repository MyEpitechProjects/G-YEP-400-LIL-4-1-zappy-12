# Boucle principale et communication réseau

Cette page décrit `loop.py`, le module qui fait tourner l'IA : il envoie les commandes, lit les
réponses, gère les messages asynchrones et orchestre le cycle « décider → agir → intégrer ».

---

## 1. Le contrat du protocole

Avant d'entrer dans le code, rappelons les règles de communication avec le serveur :

- Chaque commande est une chaîne terminée par `\n`.
- Le client peut envoyer **jusqu'à 10 commandes d'affilée** sans attendre les réponses. Au-delà,
  le serveur ignore les commandes supplémentaires.
- Le serveur exécute les commandes **dans l'ordre de réception** et répond dans ce même ordre.
- Le temps d'exécution d'une commande ne bloque **que le joueur concerné**, pas les autres.

Cette possibilité d'empiler des commandes s'appelle le **pipelining**. Elle permet, en théorie,
de masquer la latence réseau en gardant toujours du travail en attente côté serveur.

---

## 2. Envoyer une commande : `send_command`

```python
def send_command(socket_file, command):
    try:
        socket_file.write(command + "\n")
        socket_file.flush()
    except (BrokenPipeError, ConnectionResetError, OSError) as error:
        print(f"Connection lost while sending '{command}': {error}")
        return False
    return True
```

La fonction ajoute le `\n` exigé par le protocole, puis force l'écriture immédiate avec
`flush()`. Si la socket est rompue, elle renvoie `False` au lieu de laisser l'exception se
propager : la boucle pourra ainsi s'arrêter proprement.

---

## 3. Lire la bonne réponse : `read_response`

C'est l'une des fonctions les plus subtiles du module, car le serveur n'envoie pas que des
réponses aux commandes : il envoie aussi des **messages asynchrones** non sollicités. La fonction
boucle donc jusqu'à isoler la **vraie réponse attendue**, en écartant tout le reste.

Sont écartés (mais traités) au passage :

- **Les lignes vides** : ignorées, on lit la suivante.
- **Les messages asynchrones** (`message ...`, `eject: ...`) : confiés à `handle_async_line`,
  puis on continue la lecture.
- **Les événements d'incantation reçus hors contexte** : un joueur *participant* (qui n'a pas
  lui-même lancé l'incantation) reçoit `Current level: K` de manière non sollicitée lorsque le
  groupe est élevé par un autre. Il applique alors son nouveau niveau, sauf s'il est justement
  en train d'attendre sa propre réponse d'`Incantation`.

Deux conditions signalent une **connexion fermée** : une exception réseau, ou un `readline()`
qui renvoie une chaîne vide (signe d'un *EOF*, donc d'une socket close). Dans ces cas, la
fonction renvoie `None`.

**Pourquoi traiter l'incantation à part ?** Seul l'initiateur envoie la commande `Incantation` et reçoit `Elevation underway` puis `Current level: K`. Les autres joueurs du rituel montent aussi de niveau, mais l'apprennent par un message spontané. `read_response` est l'endroit où ce cas est intercepté pour que les participants restent synchronisés sur leur niveau réel.

---

## 4. Gérer les messages asynchrones : `handle_async_line`

```python
def handle_async_line(line, trantorian):
    if line.startswith("eject:"):
        trantorian.reset_eject_check()
        trantorian.vision = []
        ...
    if line.startswith("message "):
        broadcast = parse_broadcast(line)
        ...
        heartbeat = parse_heartbeat(text)
        ...
        if team == trantorian.team_name:
            trantorian.note_heartbeat(...)
```

Deux types de messages spontanés sont traités :

- **`eject: K`** : le joueur vient d'être poussé par un autre. Sa vision est invalidée (il a
  bougé sans le décider) et la vérification d'éjection en cours est réinitialisée.
- **`message K, text`** : un broadcast. S'il s'agit d'un *heartbeat* émis par un membre de la
  **même équipe**, il est enregistré dans la mémoire de l'agent via `note_heartbeat`. C'est ici
  que se construit, broadcast après broadcast, la connaissance que l'IA a de ses coéquipiers.

La direction `K` du message indique d'où vient le son ; c'est cette information qui permettra de
converger vers le roi (voir [Navigation](navigation.md) et [Stratégie](strategie.md)).

---

## 5. Intégrer une réponse : `handle_response`

Cette fonction applique une réponse unique à l'état de l'agent et journalise une ligne lisible
de débogage du type :

```
[L1 FORAGE w buf0] Forward     -> ok
```

soit le niveau, l'état courant, le drapeau roi/ouvrier, le nombre de commandes en attente, le
verbe envoyé et la réponse reçue.

- Si la réponse est `dead`, le joueur est mort : la fonction renvoie `False` pour arrêter la
  boucle.
- Sinon, elle délègue la mise à jour de l'état à `trantorian.update_from_response`.
- Cas particulier : après un `Fork` réussi, elle appelle immédiatement `spawn_child` pour lancer
  un nouveau processus client qui viendra occuper l'œuf fraîchement pondu (voir
  [Reproduction](reproduction.md)).

---

## 6. Exécuter un plan : `execute_plan`

Un *plan* est une liste de commandes produite par la stratégie. `execute_plan` les envoie et en
lit les réponses, en respectant le pipelining :

```python
while plan_index < len(plan) or sent:
    while plan_index < len(plan) and len(sent) < PIPELINE_CAP:
        # envoyer la commande, mémoriser (verbe, objet)
        ...
    # lire la réponse de la plus ancienne commande envoyée
    ...
```

Le fonctionnement est celui d'une **file** : on remplit le tampon côté serveur jusqu'à
`PIPELINE_CAP` commandes en vol, puis on dépile la plus ancienne pour en lire la réponse.

Le cas de l'incantation est géré spécialement : après avoir reçu `Elevation underway`, la
fonction effectue **une seconde lecture** pour obtenir le résultat final (`Current level: K` ou
`ko`), puis combine les deux en une seule réponse.

À chaque étape, le compteur `pending_commands` est tenu à jour, et la perte de connexion (réponse
`None`) ou la mort du joueur (`handle_response` renvoie `False`) interrompent proprement
l'exécution en renvoyant le nombre de commandes effectivement réalisées.

**Note sur `PIPELINE_CAP`.** Le protocole autorise jusqu'à 10 commandes en vol. La constante `PIPELINE_CAP` règle combien l'IA en garde réellement en attente avant de lire. Une valeur de 1 équivaut à un dialogue strictement synchrone (une commande, une réponse), ce qui simplifie le raisonnement de la stratégie au prix d'un pipelining moins agressif. C'est un curseur ajustable dans [`constants.py`](constantes.md).

---

## 7. La boucle maîtresse : `run_ai`

```python
def run_ai(trantorian, socket_file):
    since_inventory = 0
    since_connect = 0
    while True:
        if since_inventory >= INVENTORY_REFRESH_PERIOD:
            plan = ["Inventory"]
            since_inventory = 0
        elif since_connect >= CONNECT_NBR_PERIOD:
            plan = ["Connect_nbr"]
            since_connect = 0
        else:
            plan = build_plan(trantorian)
        alive, executed = execute_plan(socket_file, trantorian, plan)
        if not alive:
            break
        if plan == ["Connect_nbr"]:
            spawn_children(trantorian, fill_count(trantorian))
        since_inventory += executed
        since_connect += executed
```

La boucle a une priorité claire à chaque tour :

1. **Resynchroniser l'inventaire** périodiquement (tous les `INVENTORY_REFRESH_PERIOD` actions).
   L'IA suit son inventaire « à l'estime » entre deux requêtes, mais une remise à zéro
   régulière par une vraie commande `Inventory` corrige les écarts (par exemple la nourriture
   consommée par le temps qui passe).
2. **Demander le nombre de places libres** périodiquement (`Connect_nbr`). Après cette réponse,
   le roi peut lancer de nouveaux processus pour remplir les œufs disponibles.
3. **Sinon, demander un plan à la stratégie** (`build_plan`), qui décide de l'action la plus
   pertinente.

La boucle ne s'arrête que si `execute_plan` signale que le joueur est mort ou que la connexion a
été perdue.

---

[← Démarrage et connexion](demarrage.md) · [Sommaire](ai_doc_main.md) · [Suivant : Le protocole et le décodage →](protocole.md)