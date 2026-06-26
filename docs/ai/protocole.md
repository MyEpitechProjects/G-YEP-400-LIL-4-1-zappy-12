# Le protocole et le décodage des réponses

Le module `protocol.py` est le **traducteur** de l'IA. Il transforme les chaînes brutes envoyées
par le serveur en structures de données Python exploitables, et fabrique/décode les *heartbeats*
qui servent à la coordination. C'est un module **pur** : il ne dépend d'aucun autre module interne
et n'a aucun effet de bord, ce qui le rend simple à comprendre et à tester.

---

## 1. Distinguer les messages : `is_async_message` et `is_incantation_event`

Le serveur mélange, sur la même socket, les réponses aux commandes et des notifications
spontanées. Deux petites fonctions servent d'aiguillage :

- `is_async_message(line)` : vrai si la ligne commence par `message ` (un broadcast) ou
  `eject:` (une éjection). Ces lignes ne correspondent à **aucune** commande envoyée.
- `is_incantation_event(line)` : vrai si la ligne est `Elevation underway` ou commence par
  `Current level:`. Ces lignes accompagnent une incantation et peuvent arriver hors contexte
  pour les participants.

Ces fonctions sont utilisées par la boucle réseau pour décider quoi faire d'une ligne reçue
(voir [Boucle principale](boucle_reseau.md)).

---

## 2. Décoder un broadcast : `parse_broadcast`

Format reçu : `message K, text` où `K` est la direction d'où vient le son.

```python
def parse_broadcast(line):
    body = line[len("message "):]
    direction_str, text = body.split(",", 1)
    ...
    return int(direction_str), text
```

La fonction renvoie le couple `(direction, texte)`, ou `None` si la ligne est malformée
(absence de virgule, direction non numérique). Le `split(",", 1)` ne coupe qu'à la **première**
virgule, ce qui préserve d'éventuelles virgules dans le texte du message.

---

## 3. Décoder l'inventaire : `parse_inventory`

Format reçu : `[food 345, sibur 3, phiras 5, ...]`.

La fonction retire les crochets, découpe sur les virgules, puis pour chaque entrée `nom nombre`
construit un dictionnaire `{ "food": 345, "sibur": 3, ... }`. Les entrées qui ne respectent pas
le format `nom nombre` sont ignorées par prudence. Une réponse `dead` renvoie `None`.

---

## 4. Décoder la vision : `parse_look`

C'est le décodage le plus structurant pour la stratégie. Format reçu :

```
[player, , thystame, , food, ...]
```

Les cases sont séparées par des **virgules**. Sur une case, plusieurs objets sont séparés par des
**espaces**. La fonction produit une **liste de dictionnaires**, un par case visible, chaque
dictionnaire comptant les objets présents :

```python
# "[player, player food, ]" devient :
[ {"player": 1}, {"player": 1, "food": 1}, {} ]
```

Le comptage gère naturellement les doublons : `"player player food"` devient
`{"player": 2, "food": 1}`. Une case vide devient un dictionnaire vide `{}`.

Point essentiel : **la case d'indice 0 est toujours la case sur laquelle se tient le joueur**.
C'est pourquoi le reste du code écrit fréquemment `tile = trantorian.vision[0]` pour examiner sa
propre case. La numérotation des cases suivantes (le cône de vision) est expliquée dans
[Navigation](navigation.md).

---

## 5. Les heartbeats : le langage de la coordination

Les joueurs ne peuvent pas s'échanger de données en dehors du jeu. Toute coordination passe donc
par des messages diffusés à tous via `Broadcast`. L'IA s'est dotée d'un **format de message
unique**, le *heartbeat*, qui encode tout ce qu'un coéquipier a besoin de savoir.

### Construire un heartbeat : `build_heartbeat`

```python
def build_heartbeat(team_name, token, level, king_flag, state, sequence):
    return f"Z:{team_name}:{token}:{level}:{king_flag}:{state}:{sequence}"
```

Le message a la forme `Z:équipe:jeton:niveau:drapeau_roi:état:séquence`. Le préfixe `Z` sert de
signature : il permet de reconnaître nos propres balises parmi d'éventuels autres broadcasts.

| Champ | Signification |
|-------|---------------|
| `Z` | Signature de protocole (toujours `Z`). |
| `team_name` | Nom de l'équipe émettrice. |
| `token` | Jeton aléatoire identifiant l'émetteur. |
| `level` | Niveau de l'émetteur. |
| `king_flag` | `1` si l'émetteur est un roi établi, `0` sinon. |
| `state` | Étiquette d'état (texte non vide : `KING`, `RALLY`, `FORAGE`, ...). |
| `sequence` | Tick local de l'émetteur ; sert à dater les messages. |

### Décoder un heartbeat : `parse_heartbeat`

La fonction réalise une **validation stricte** : elle exige exactement 7 champs, la signature
`Z`, des valeurs numériques là où c'est attendu (jeton, niveau, drapeau, séquence), un drapeau
réduit à `0` ou `1`, et une étiquette d'état non vide. Tout message non conforme renvoie `None`
et est purement ignoré. Cette rigueur protège l'IA contre les messages parasites ou corrompus :
un broadcast mal formé ne peut pas perturber sa logique de coordination.

**À quoi sert le champ `sequence` ?** C'est le tick local de l'émetteur au moment de l'envoi. Comme les messages peuvent arriver dans le désordre, l'agent récepteur ne garde que les heartbeats **strictement plus récents** que ceux déjà connus pour un même jeton (voir `note_heartbeat` dans [L'agent Trantorian](agent.md)). Un vieux message qui arriverait en retard est ainsi écarté.

---

[← Boucle principale et communication réseau](boucle_reseau.md) · [Sommaire](ai_doc_main.md) · [Suivant : L'agent Trantorian →](agent.md)