# La stratégie : le modèle « king-rally »

Cette page est le cœur de la documentation. Elle explique **comment l'IA décide**, c'est-à-dire la
logique du module `strategy.py`. C'est ici que se joue toute l'intelligence du client : transformer
une perception (vision, inventaire, heartbeats entendus) en une action utile au projet collectif.

---

## 1. Le problème à résoudre

Rappelons l'enjeu. Pour gagner, six joueurs doivent atteindre le niveau 8. Or chaque élévation au
delà du niveau 1 exige de réunir **sur une même case** plusieurs joueurs **de même niveau** ainsi
que des pierres précises. Les difficultés :

- Les joueurs sont **autonomes** et **identiques** : aucun n'est « le chef » par nature.
- Ils ne peuvent **pas communiquer hors du jeu** : tout passe par le serveur.
- Ils ne connaissent **pas leur position absolue** : chacun se repère depuis son propre point
  d'apparition.

Comment, dans ces conditions, faire se rejoindre plusieurs joueurs au bon endroit ? La réponse est
le modèle **king-rally**.

---

## 2. Le principe du king-rally en trois rôles

```
            ┌───────────────────────────────────────────────┐
            │                  LE ROI                       │
            │  Immobile. Diffuse sa position par broadcast. │
            │  Sert de point de ralliement fixe.            │
            └────────────────────┬───────────────────────────┘
                                 │ heartbeats (direction du son)
              ┌──────────────────┼──────────────────┐
              ▼                  ▼                  ▼
        ┌──────────┐       ┌──────────┐       ┌──────────┐
        │ OUVRIER  │       │ OUVRIER  │       │ OUVRIER  │
        │ converge │       │ converge │       │ converge │
        │ + récolte│       │ + récolte│       │ + récolte│
        └──────────┘       └──────────┘       └──────────┘
                                 │
                                 ▼
              Une fois rassemblés sur la case du roi :
              dépôt des pierres → l'INITIATEUR (plus petit
              jeton) déclenche l'Incantation pour le groupe.
```

- **Le roi** (*king*) est un point de ralliement immobile. Il se fige sur une case et diffuse
  régulièrement sa présence. Il ne se déplace pas, ce qui donne aux autres une cible stable.
- **Les ouvriers** (*workers*) entendent la direction du roi et **convergent** vers lui, en
  récoltant nourriture et pierres en chemin. Arrivés sur sa case, ils se mettent en **ralliement**.
- **L'initiateur** est l'ouvrier (ou le roi) de plus petit jeton parmi les joueurs de même niveau
  présents sur la case. Lui seul lance l'incantation, ce qui évite que plusieurs joueurs ne la
  déclenchent en même temps.

Toute cette chorégraphie est orchestrée sans aucun message privé : uniquement par des broadcasts
publics, dont seule la **direction** importe pour se rassembler.

---

## 3. La machine à états

L'IA est décrite par quelques **états** (définis dans [`constants.py`](constantes.md)). Ils servent
surtout à journaliser et à renseigner les heartbeats, mais ils résument bien la logique :

| État | Signification |
|------|---------------|
| `SURVIVE` | Urgence nourriture : tout le reste est suspendu. |
| `SOLO` | Niveau 1→2, qui se fait seul, sans coordination. |
| `KING` | L'agent est le roi : il tient son ancre. |
| `CONVERGE` | Ouvrier en route vers le roi. |
| `RALLY` | Ouvrier arrivé sur la case du roi, en attente du groupe. |
| `FORAGE` | Récolte exploratoire, faute de roi localisé. |

Les transitions entre ces états sont décidées à chaque tour par la fonction `build_plan`.

---

## 4. `build_plan` : l'arbre de décision principal

`build_plan` est appelée à chaque tour par la boucle. Elle renvoie la **liste de commandes** à
exécuter. Son ordre de priorité est crucial : il traduit ce qui compte le plus pour l'IA.

```python
def build_plan(trantorian):
    trantorian.tick += 1
    trantorian.prune_heard()        # oublier les coéquipiers périmés
    trantorian.update_role()        # roi ou ouvrier ?
    if not trantorian.inventory:    # 1. connaître son inventaire
        return ["Inventory"]
    if not trantorian.vision:       # 2. connaître son environnement
        return ["Look"]
    tile = trantorian.vision[0]     # la case actuelle
    if trantorian.update_food_emergency():   # 3. SURVIE prioritaire
        ...
        return survive_plan(trantorian, tile)
    if can_initiate(trantorian, tile):       # 4. peut-on élever maintenant ?
        return ["Incantation"]
    eject_plan = eject_strategy_plan(trantorian, tile)   # 5. chasser un intrus ?
    if eject_plan is not None:
        return eject_plan
    if should_broadcast(trantorian):         # 6. émettre un heartbeat ?
        return [heartbeat_command(trantorian)]
    if should_fork(trantorian):              # 7. le roi se reproduit ?
        ...
        return ["Fork"]
    requirements = next_requirement(trantorian.level)
    if requirements is None:                 # 8. déjà au niveau max
        return idle_plan(trantorian, tile)
    if requirements.get("players", 1) <= 1:  # 9. niveau 1->2 : solo
        return solo_plan(trantorian, tile)
    if trantorian.is_king:                   # 10. roi : tenir l'ancre
        return king_plan(trantorian, tile)
    return worker_plan(trantorian, tile)     # 11. ouvrier : converger
```

Lisons cet ordre comme une hiérarchie de besoins :

1. **Se connaître** : sans inventaire ni vision à jour, aucune décision n'est fiable. On les
   réclame d'abord.
2. **Survivre** : la faim prime sur tout. Inutile de monter en niveau si l'on meurt.
3. **Saisir une opportunité** : si les conditions d'élévation sont déjà réunies ici et maintenant,
   on lance l'incantation sans attendre.
4. **Se défendre** : repousser un intrus qui squatte la case (de façon prudente, voir §9).
5. **Communiquer** : émettre périodiquement son heartbeat pour rester visible des autres.
6. **Faire croître l'équipe** : le roi se reproduit pour atteindre la population cible.
7. **Progresser** : selon le niveau et le rôle, agir en solo, en roi ou en ouvrier.

---

## 5. Survivre : `survive_plan`

Quand la nourriture est critique, plus rien d'autre ne compte :

```python
def survive_plan(trantorian, tile):
    if tile.get("food", 0) > 0:
        return ["Take food"]                     # de la nourriture ici : on la prend
    best_tile = find_best_food_tile(trantorian.vision)
    if best_tile is not None and best_tile != 0:
        return build_move_plan(best_tile) + ["Look"]   # vers la nourriture la plus proche
    return explore_plan(trantorian)              # rien en vue : on cherche ailleurs
```

L'agent mange ce qu'il a sous les pieds, sinon il fonce vers la nourriture visible la plus
avantageuse, sinon il explore. Grâce à l'hystérésis (voir [L'agent](agent.md)), il ne quittera ce
mode qu'une fois une vraie réserve reconstituée.

---

## 6. Le niveau 1→2 : `solo_plan`

Le passage du niveau 1 au niveau 2 ne requiert **qu'un seul joueur** et une seule *linemate*.
Aucune coordination n'est donc utile : chaque joueur le fait dans son coin.

```python
def solo_plan(trantorian, tile):
    if tile.get("food", 0) > 0 and inventory.get("food", 0) < FOOD_TRAVEL_TARGET:
        return ["Take food"]                 # faire le plein tant qu'on est sous le seuil
    if inventory.get("linemate", 0) >= 1:
        return ["Set linemate"]              # poser la pierre et s'élever ensuite
    best_tile = find_best_tile(...)          # sinon, aller chercher de quoi progresser
    ...
```

La logique : sécuriser de la nourriture, puis dès qu'on tient une *linemate*, la poser sur sa case
pour pouvoir lancer l'incantation (étape déclenchée par `can_initiate` au tour suivant). Démarrer
en solo permet à toute l'équipe d'atteindre rapidement le niveau 2 avant de basculer dans le jeu
coordonné.

---

## 7. Le roi : `king_plan`

À partir du niveau 2, la coordination s'impose. Le roi devient une **balise strictement
immobile** :

```python
def king_plan(trantorian, tile):
    trantorian.state = KING
    # Si poussé hors de l'ancre, y revenir d'un pas.
    if trantorian.has_anchor and (trantorian.x != trantorian.anchor_x or ...):
        return navigate_to(...) + ["Look"]
    if trantorian.inventory.get("food", 0) < KING_FOOD_KEEP and tile.get("food", 0) > 0:
        return ["Take food"]                 # entretenir sa réserve sur place
    drop = drop_stone_plan(trantorian, tile)
    if drop is not None:
        return drop                          # déposer une pierre que le rite réclame
    return ["Look"]                          # par défaut : observer, sans bouger
```

Le roi ne se déplace jamais de lui-même : son seul mouvement autorisé est de **revenir sur son
ancre** s'il s'en est fait éjecter. Le reste du temps, il mange ce qui passe sur sa case (jusqu'à
`KING_FOOD_KEEP`), constitue le **dépôt de pierres** du rituel, et observe. Cette immobilité est ce
qui rend le ralliement possible : la cible ne bouge pas.

---

## 8. L'ouvrier : `worker_plan`

L'ouvrier suit le son du roi tout en récoltant :

```python
def worker_plan(trantorian, tile):
    direction = trantorian.king_direction
    if direction is None:                    # aucun roi localisé
        takes = forage_takes(trantorian, tile)
        if takes: return takes + ["Look"]
        trantorian.state = FORAGE
        return explore_plan(trantorian)
    if direction != 0:                       # le roi est ailleurs
        takes = forage_takes(trantorian, tile)
        if takes: return takes + ["Look"]
        trantorian.state = CONVERGE
        return moves_toward_sound(direction) + ["Look"]
    return rally_plan(trantorian, tile)      # direction 0 : on est sur la case du roi
```

Trois situations :

- **Pas de roi en vue** : l'ouvrier récolte ce qu'il peut (`forage_takes`) puis explore. C'est
  l'état `FORAGE`.
- **Le roi est ailleurs** : il récolte puis fait un pas vers le son (`moves_toward_sound`). C'est
  l'état `CONVERGE`. Récolter pendant la convergence est rentable : on arrive sur la case du roi
  déjà chargé en pierres.
- **On est sur la case du roi** (`direction == 0`) : on passe en ralliement.

### `forage_takes` : que ramasser ?

```python
def forage_takes(trantorian, tile):
    food_need = FOOD_TRAVEL_TARGET - inventory.get("food", 0)
    # prendre de la nourriture jusqu'à la réserve de voyage
    # prendre chaque pierre jusqu'à HOARD_TARGET
```

L'ouvrier prend de la nourriture jusqu'à `FOOD_TRAVEL_TARGET` (une réserve confortable pour
voyager) et **thésaurise** chaque pierre jusqu'à `HOARD_TARGET`. Ce dernier seuil correspond, pour
chaque pierre, à la **quantité maximale** jamais exigée par un rituel quelconque. En transportant
ce maximum, un ouvrier rassemblé peut fournir les pierres nécessaires à n'importe quel niveau sans
devoir repartir en chercher.

---

## 9. Le ralliement : `rally_plan`

Une fois sur la case du roi, l'ouvrier passe en mode `RALLY`. Son objectif : préparer le rituel
tout en restant sur place pour faire nombre.

```python
def rally_plan(trantorian, tile):
    trantorian.state = RALLY
    food = trantorian.inventory.get("food", 0)
    if food < FOOD_LOW_LIMIT and tile.get("food", 0) > 0:
        return ["Take food"]                 # manger sur le dépôt si on baisse
    if food < FOOD_RALLY_ORBIT:
        ... aller chercher de la nourriture proche (orbite) ...
    drop = drop_stone_plan(trantorian, tile)
    if drop is not None:
        trantorian.rally_stall = 0
        return drop                          # déposer une pierre que le rite réclame
    if food > FOOD_RALLY_HIGH:
        return ["Set food"]                  # surplus : alimenter le dépôt commun
    if rite_blocked_by_missing_stone(trantorian, tile):
        trantorian.rally_stall += 1
        if trantorian.rally_stall > RALLY_STALL_LIMIT:
            ... repartir chercher la pierre manquante ...
    return ["Look"]                          # défaut : tenir position et observer
```

La logique de ralliement, du plus prioritaire au moins prioritaire :

1. **Ne pas mourir sur place** : manger le dépôt si la réserve baisse ; en dernier recours, faire
   une courte « orbite » vers la nourriture proche si la situation devient critique.
2. **Construire le rituel** : déposer une à une les pierres que la case n'a pas encore mais que
   l'on porte.
3. **Mutualiser** : si l'on est largement rassasié (`> FOOD_RALLY_HIGH`), déposer un peu de
   nourriture pour alimenter le dépôt commun dont profiteront les autres.
4. **Débloquer une impasse** : si le rituel manque d'une pierre absente partout, un compteur
   `rally_stall` s'incrémente. Au-delà de `RALLY_STALL_LIMIT` tours de blocage, l'ouvrier repart
   chercher cette pierre plutôt que d'attendre indéfiniment.
5. **Par défaut, tenir position** et continuer à observer la case (`Look`), car sa simple présence
   compte dans le décompte des joueurs nécessaires.

---

## 10. Choisir où aller : le scoring des cases

Deux fonctions évaluent les cases visibles pour décider de la meilleure destination.

### `find_best_tile` : récolte générale

Chaque case reçoit un **score** combinant plusieurs critères :

| Critère | Effet sur le score |
|---------|--------------------|
| Nourriture sur la case | Très valorisée si l'on est sous `FOOD_TRAVEL_TARGET`, sinon modérément. |
| Pierres encore manquantes | Valorisées, mais seulement à hauteur de ce qu'il manque réellement. |
| Distance | Pénalité proportionnelle : les cases lointaines coûtent du temps. |
| Case actuelle | Léger bonus, pour éviter de bouger sans raison. |

La case au meilleur score est retenue. Ce mélange pousse l'IA à privilégier ce dont elle a besoin,
au plus près.

### `find_best_food_tile` : recherche ciblée de nourriture

Une version plus simple, utilisée en situation d'urgence : elle ne regarde que la nourriture,
pondérée par la distance.

### `explore_plan` : explorer l'inconnu

Quand rien de précis n'attire l'agent, il explore intelligemment. Il choisit la case voisine la
**moins visitée** (en s'appuyant sur le compteur `visited`), à coût de rotation minimal. Cette
exploration de frontière évite de tourner en rond sur les mêmes cases.

---

## 11. La défense : `eject_strategy_plan`

Un joueur peut **éjecter** les autres de sa case. L'IA s'en sert pour chasser un **intrus d'une
équipe adverse** qui camperait sur sa case, mais avec beaucoup de prudence pour ne **jamais
éjecter un coéquipier** par erreur.

La fonction n'éjecte que si toutes ces conditions sont réunies :

- Le niveau est supérieur à 1 (rien à protéger avant).
- L'agent n'est pas le roi et n'est pas sur la case du roi (on ne perturbe jamais le ralliement).
- Le cooldown d'éjection (`EJECT_COOLDOWN`) est écoulé.
- Il y a exactement **deux** joueurs sur la case (soi-même plus un seul autre).
- Cet autre n'est **pas** un coéquipier récemment confirmé (`same_tile_team_count`).
- La situation est **confirmée sur plusieurs tours** (`EJECT_CONFIRM_ROUNDS`) : on ne réagit pas
  à un coéquipier qui n'aurait simplement pas encore émis son heartbeat.

Tant que la confirmation est en cours, l'agent ré-émet son heartbeat et refait un `Look`, ce qui
laisse une chance à un éventuel coéquipier silencieux de se signaler. Ce n'est qu'après plusieurs
confirmations qu'il éjecte réellement. La prudence est de mise car éjecter un allié saboterait son
propre ralliement.

---

## 12. Les broadcasts : `heartbeat_command` et `should_broadcast`

`should_broadcast` fait avancer une horloge interne et déclenche un heartbeat à intervalle
régulier. Le roi parle plus souvent que les ouvriers (`KING_HEARTBEAT_PERIOD` plus court que
`WORKER_HEARTBEAT_PERIOD`) : sa position étant la cible commune, il est crucial que sa direction
reste fraîche pour ceux qui convergent.

`heartbeat_command` assemble le message via `build_heartbeat`, en y plaçant le drapeau roi
(`1` seulement si l'agent est un roi **établi**), le niveau, l'état et le tick courant.

---

## 13. Vue d'ensemble du cycle de vie d'une équipe

Pour relier tout cela, voici le parcours typique d'une équipe :

1. **Démarrage** : chaque joueur connecté monte seul au niveau 2 (`solo_plan`).
2. **Élection** : les heartbeats circulent ; le joueur de plus haut jeton devient roi et fige son
   ancre. Les autres deviennent ouvriers.
3. **Convergence** : les ouvriers récoltent et se dirigent vers le son du roi (`CONVERGE`).
4. **Ralliement** : arrivés sur la case du roi, ils déposent leurs pierres et tiennent position
   (`RALLY`).
5. **Élévation** : dès que les pierres et le nombre de joueurs de même niveau sont réunis, le
   joueur de plus petit jeton lance l'`Incantation`. Tout le groupe monte d'un niveau.
6. **Reproduction** : en parallèle, le roi entretient la population par des `Fork` pour disposer
   d'assez de joueurs aux paliers les plus exigeants (6 joueurs requis à partir du niveau 7).
7. **Répétition** jusqu'au niveau 8, l'objectif étant d'y amener six joueurs.

Les règles précises de l'élévation sont détaillées dans [Les règles d'incantation](incantation.md),
et la reproduction dans [La reproduction](reproduction.md).

---

[← Navigation et géométrie](navigation.md) · [Sommaire](ai_doc_main.md) · [Suivant : Les règles d'incantation →](incantation.md)