# L'agent Trantorian : état et mémoire

La classe `Trantorian` (`trantorian.py`) est la **mémoire vivante** de l'IA. Elle rassemble tout
ce que l'agent sait sur lui-même (niveau, inventaire, position estimée) et sur ses coéquipiers
(les heartbeats entendus), et c'est elle qui intègre chaque réponse du serveur dans cet état.

La stratégie ne fait que **lire** cet état pour décider, puis l'IA **met à jour** cet état à
partir des réponses : la classe `Trantorian` est donc le point de rencontre entre la perception et
la décision.

---

## 1. Les champs de l'état

À sa création, l'agent initialise un grand nombre d'attributs. Les principaux :

### Identité et monde

| Attribut | Rôle |
|----------|------|
| `team_name` | Nom de l'équipe. |
| `map_width`, `map_height` | Dimensions de la carte torique. |
| `level` | Niveau courant (1 au départ, 8 au maximum). |
| `token` | Jeton aléatoire (1 à 999 999 999) identifiant l'agent dans les broadcasts. |

### Perception

| Attribut | Rôle |
|----------|------|
| `vision` | Liste des cases visibles (résultat du dernier `Look`). |
| `inventory` | Dictionnaire des ressources possédées, dont `food`. |
| `available_slots` | Nombre de places libres dans l'équipe (œufs disponibles). |

### Coordination

| Attribut | Rôle |
|----------|------|
| `heard` | Heartbeats récents, indexés par jeton. |
| `seen` | Dernier tick où chaque jeton a été entendu (sert au recensement). |
| `is_king`, `king_stable` | Statut de roi et nombre de tours consécutifs sous la couronne. |
| `king_direction` | Direction sonore vers le roi (`0` = ici). |
| `has_anchor`, `anchor_x`, `anchor_y` | Le point de ralliement figé du roi. |

### Position estimée (dead-reckoning)

| Attribut | Rôle |
|----------|------|
| `x`, `y`, `orientation` | Position et orientation estimées depuis l'apparition. |
| `visited` | Compteur de visites par case, pour guider l'exploration. |

### Compteurs et garde-fous

| Attribut | Rôle |
|----------|------|
| `tick` | Compteur d'actions local : la « notion de temps » de l'agent. |
| `pending_commands` | Nombre de commandes envoyées dont la réponse n'est pas encore lue. |
| `food_emergency` | Indique si l'agent est en mode survie. |
| `rally_stall` | Compteur de blocage sur la case de ralliement. |
| `last_fork_tick`, `last_eject_tick` | Dates de dernière reproduction / éjection (pour les cooldowns). |
| `state` | Étiquette d'état courant, utilisée pour le journal et les broadcasts. |

**Remarque sur l'inventaire de départ.** L'agent démarre avec `food: 10`, conformément aux règles : 10 unités de vie permettent de survivre 1260 unités de temps. Cette valeur estrapidement corrigée par la première commande `Inventory`.

---

## 2. La gestion de la faim avec hystérésis

```python
def update_food_emergency(self):
    food = self.inventory.get("food", 0)
    if self.food_emergency:
        if food >= FOOD_LOW_LIMIT:
            self.food_emergency = False
    elif food <= FOOD_CRITICAL:
        self.food_emergency = True
    return self.food_emergency
```

La survie utilise **deux seuils différents** plutôt qu'un seul. On entre en mode panique quand la
nourriture descend à `FOOD_CRITICAL` ou moins, mais on n'en sort qu'une fois remonté à
`FOOD_LOW_LIMIT` ou plus. Cet écart entre les deux seuils est une **hystérésis** : il empêche
l'agent de basculer sans cesse entre « je panique » et « ça va » lorsqu'il oscille autour du seuil,
un phénomène d'instabilité appelé *flapping*. Le mode survie reste donc stable une fois enclenché,
le temps de reconstituer une vraie réserve.

---

## 3. La mémoire des coéquipiers

### Enregistrer un heartbeat : `note_heartbeat`

Chaque heartbeat reçu d'un coéquipier est rangé dans `heard[token]`. Deux protections :

- On ignore son propre jeton (un agent ne se coordonne pas avec lui-même).
- On ne garde un message que s'il est **strictement plus récent** (champ `sequence`) que le
  dernier connu pour ce jeton. Les messages anciens ou dupliqués arrivés en retard sont écartés.

Chaque entrée mémorise `[tick local, direction, niveau, drapeau_roi, état, séquence]`.

### Oublier ce qui est périmé : `prune_heard`

Deux durées de vie distinctes :

- `heard` est purgé après `HEARD_TIMEOUT` ticks : une direction vieille de trop longtemps n'est
  plus fiable (le joueur a bougé).
- `seen` est purgé après `POP_MEMORY` ticks, plus long : il sert au **recensement** de la
  population, qui tolère des silences plus longs.

Détail d'implémentation important : la fonction construit d'abord la liste des jetons à supprimer, **puis** les supprime. On ne modifie jamais un dictionnaire pendant qu'on le parcourt, ce qui provoquerait une erreur en Python.

---

## 4. L'élection du roi : `update_role`

C'est la fonction qui décide, à chaque tour, si l'agent est roi ou ouvrier. Le principe : le roi
est l'agent de **plus haut jeton** dans l'équipe, avec un mécanisme de **stickiness** (adhérence)
pour éviter les changements de roi intempestifs.

```python
def update_role(self):
    was_stable_king = self.is_king and self.king_stable >= KING_STABLE_ROUNDS
    best_priority = self.token + (KING_STICKY if was_stable_king else 0)
    best_is_self = True
    for token, info in self.heard.items():
        priority = token + (KING_STICKY if info[3] else 0)   # info[3] = drapeau roi
        if priority > best_priority:
            best_priority = priority
            best_direction = info[1]
            best_is_self = False
    ...
```

Le calcul repose sur une **priorité** : `priorité = jeton + KING_STICKY si roi établi, sinon
jeton`. Comme `KING_STICKY` (deux milliards) dépasse largement tout jeton possible, un roi déjà
établi conserve une priorité supérieure à n'importe quel jeton brut. Conséquences :

- **Au démarrage**, sans roi établi, c'est simplement le plus grand jeton qui gagne.
- **Une fois un roi en place**, il garde la couronne tant qu'il diffuse son drapeau roi, même si
  un nouvel agent au jeton plus élevé apparaît. On évite ainsi de déplacer le point de ralliement
  à chaque arrivée, ce qui désorganiserait tout le groupe.

Si l'agent gagne l'élection, il devient roi, fige son ancre (la première fois) et se déclare
`king_direction = 0` (« le roi, c'est moi, ici »). Sinon il devient ouvrier et retient la
**direction** vers le roi élu, qu'il utilisera pour converger.

`king_is_stable()` complète le tableau : un roi n'est considéré comme **établi** qu'après
`KING_STABLE_ROUNDS` tours consécutifs sous la couronne. Ce délai évite qu'un roi éphémère ne
prenne des décisions structurantes (comme gérer la reproduction) avant d'être confirmé.

---

## 5. Compter la population et le voisinage

| Méthode | Ce qu'elle calcule |
|---------|--------------------|
| `population_estimate()` | `1 + len(seen)` : soi-même plus tous les jetons entendus récemment. |
| `same_tile_same_level_count()` | Coéquipiers de même niveau dont la balise vient de la case actuelle (direction 0). |
| `is_min_token_on_tile()` | Vrai si l'agent a le **plus petit** jeton parmi ses pairs de même niveau présents. |
| `same_tile_team_count()` | Coéquipiers très récemment confirmés sur la case (fenêtre `EJECT_ALLY_MAX_AGE`). |

Deux usages clés du jeton apparaissent ici, et ils sont volontairement **opposés** :

- Le **roi** est choisi par le **plus haut** jeton (via `update_role`).
- L'**initiateur** d'une incantation est choisi par le **plus bas** jeton
  (`is_min_token_on_tile`).

Désigner deux rôles distincts par les deux extrémités de la même échelle de jetons garantit que
chaque rôle est tenu par **exactement un** agent, sans concertation et de façon déterministe.

---

## 6. Intégrer les réponses : `update_from_response`

C'est le grand aiguillage qui répercute chaque réponse du serveur sur l'état :

| Réponse à | Effet sur l'état |
|-----------|------------------|
| `Inventory` | Remplace l'inventaire par celui décodé. |
| `Look` | Remplace la vision par celle décodée. |
| `Connect_nbr` | Met à jour le nombre de places libres. |
| `Incantation` | Applique le nouveau niveau (ou note l'échec), puis invalide la vision. |
| `Take <obj>` | Incrémente l'objet dans l'inventaire (si `ok`). |
| `Set <obj>` | Décrémente l'objet dans l'inventaire (si `ok`). |
| `Forward` | Avance la position estimée (si `ok`). |
| `Right` / `Left` | Tourne l'orientation estimée (si `ok`). |
| `Eject` | Arme le cooldown d'éjection (si `ok`). |
| `Fork` | Met `available_slots` à `None` (inconnu jusqu'au prochain `Connect_nbr`). |

Un détail récurrent : **toute action qui modifie la position ou la case invalide la vision**
(`self.vision = []`). En effet, après un déplacement, une rotation, un ramassage ou un dépôt, la
dernière image renvoyée par `Look` n'est plus fiable. La stratégie verra cette vision vide et
redemandera un `Look` au tour suivant. C'est ce qui garantit que l'IA ne décide jamais sur une
perception périmée.

La méthode `update_level` joue un rôle similaire lors d'une élévation : elle remet à zéro les
compteurs liés au niveau précédent et vide la vision, car le cône de vision **change de taille à
chaque niveau** (la portée augmente d'un cran).

---

[← Le protocole et le décodage](protocole.md) · [Sommaire](ai_doc_main.md) · [Suivant : Navigation et géométrie →](navigation.md)