# Référence des constantes

Le module `constants.py` centralise **tous les paramètres de réglage** de l'IA. Les regrouper en
un seul endroit permet d'ajuster le comportement sans toucher à la logique, et d'avoir une vue
d'ensemble des compromis effectués. Cette page sert de catalogue commenté.

Rappel utile tiré des règles du jeu : **1 unité de nourriture = 126 unités de temps** de survie.
Beaucoup de seuils ci-dessous s'interprètent à l'aune de cette équivalence.

---

## 1. Données dérivées des règles

### `REQUIREMENTS`

La table des exigences d'élévation, **indexée par niveau cible** (2 à 8). Pour chaque cible, elle
donne le nombre de joueurs et la quantité de chaque pierre. Détaillée dans
[Les règles d'incantation](incantation.md).

### `STONES`

La liste ordonnée des six pierres : `linemate`, `deraumere`, `sibur`, `mendiane`, `phiras`,
`thystame`. Elle sert de référence pour parcourir les pierres de façon cohérente partout.

### `MAX_LEVEL`

Le niveau maximal, `8`.

### `HOARD_TARGET`

Pour chaque pierre, la **quantité maximale jamais exigée** par un rituel. Calculée automatiquement à partir de `REQUIREMENTS` :

```python
HOARD_TARGET = {stone: max(level_req.get(stone, 0) for level_req in REQUIREMENTS.values())
                for stone in STONES}
```

Les ouvriers thésaurisent jusqu'à ce seuil pour pouvoir, une fois rassemblés, fournir les pierre de n'importe quel palier sans repartir en chercher (voir `forage_takes` dans [Stratégie](strategie.md)).

---

## 2. Les états

Étiquettes textuelles servant au journal et aux heartbeats : `SURVIVE`, `SOLO`, `KING`,
`CONVERGE`, `RALLY`, `FORAGE`. Leur signification est décrite dans
[La stratégie](strategie.md).

---

## 3. Gestion de la nourriture

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `FOOD_CRITICAL` | 18 | Seuil d'entrée en mode survie. |
| `FOOD_LOW_LIMIT` | 35 | Seuil de sortie du mode survie (hystérésis) ; aussi un plancher avant de forker. |
| `FOOD_TRAVEL_TARGET` | 75 | Réserve de voyage visée par les ouvriers avant de converger. |
| `FOOD_RALLY_HIGH` | 40 | Au-delà, un rallié dépose son surplus au dépôt commun. |
| `FOOD_RALLY_ORBIT` | 24 | En dessous, un rallié s'autorise une courte sortie vers la nourriture. |
| `KING_FOOD_KEEP` | 55 | Réserve que le roi maintient sur place. |

L'écart entre `FOOD_CRITICAL` (18) et `FOOD_LOW_LIMIT` (35) constitue l'**hystérésis** qui stabilise
le mode survie (voir [L'agent Trantorian](agent.md)).

---

## 4. Scoring des cases

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `HIGH_FOOD_PRIORITY` | 100 | Poids de la nourriture quand la réserve est basse. |
| `NORMAL_FOOD_PRIORITY` | 20 | Poids de la nourriture quand la réserve est confortable. |
| `STONE_PRIORITY` | 80 | Poids d'une pierre encore manquante. |
| `DISTANCE_PENALTY` | 10 | Pénalité par case de distance. |
| `CURRENT_TILE_BONUS` | 5 | Léger bonus pour la case actuelle (éviter de bouger sans raison). |

Ces poids déterminent où l'IA choisit d'aller (voir `find_best_tile` dans
[Stratégie](strategie.md)). Les ajuster modifie l'arbitrage entre se nourrir, ramasser des pierres
et limiter les déplacements.

---

## 5. Coordination (heartbeats)

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `WORKER_HEARTBEAT_PERIOD` | 3 | Cadence de broadcast d'un ouvrier (en tours). |
| `KING_HEARTBEAT_PERIOD` | 2 | Cadence du roi, plus rapide car sa position est la cible commune. |
| `HEARD_TIMEOUT` | 25 | Au-delà, une direction entendue est jugée périmée et oubliée. |
| `RALLY_STALL_LIMIT` | 35 | Tours de blocage tolérés au ralliement avant de repartir chercher une pierre. |
| `KING_STICKY` | 2 000 000 000 | Bonus de priorité d'un roi établi (mécanisme d'adhérence). |

`KING_STICKY` dépasse volontairement tout jeton possible (plage 1 à 999 999 999), ce qui garantit
qu'un roi établi conserve la couronne malgré l'arrivée d'un jeton plus élevé (voir `update_role`
dans [L'agent Trantorian](agent.md)).

---

## 6. Contrôle de la population

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `POP_TARGET` | 10 | Effectif visé pour l'équipe. |
| `POP_MEMORY` | 70 | Durée de mémoire d'un coéquipier pour le recensement. |
| `POP_WARMUP_SECONDS` | 25 | Délai d'échauffement avant que le roi ne gère la reproduction. |
| `FORK_COOLDOWN` | 25 | Délai minimal entre deux pontes. |
| `FILL_BATCH` | 1 | Nombre maximal de clients lancés par tour. |
| `KING_STABLE_ROUNDS` | 4 | Tours sous la couronne avant qu'un roi soit jugé « établi ». |

Détails dans [La reproduction](reproduction.md). `KING_STABLE_ROUNDS` conditionne aussi
`king_is_stable()`, donc l'émission du drapeau roi et le droit de gérer la population.

---

## 7. Stratégie d'éjection

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `EJECT_CONFIRM_ROUNDS` | 4 | Tours de confirmation avant d'éjecter un intrus présumé. |
| `EJECT_ALLY_MAX_AGE` | 6 | Fraîcheur maximale d'un heartbeat pour considérer un voisin comme allié. |
| `EJECT_COOLDOWN` | 30 | Délai minimal entre deux éjections. |

Ces garde-fous, décrits dans [La stratégie](strategie.md), visent surtout à **ne jamais éjecter un
coéquipier** par erreur.

---

## 8. Réseau et cadence de boucle

| Constante | Valeur | Rôle |
|-----------|:------:|------|
| `PIPELINE_CAP` | 1 | Nombre de commandes maintenues en vol avant de lire (le protocole autorise jusqu'à 10). |
| `CONNECT_NBR_PERIOD` | 18 | Cadence des requêtes `Connect_nbr` (places libres). |
| `INVENTORY_REFRESH_PERIOD` | 10 | Cadence de resynchronisation de l'inventaire réel. |

`PIPELINE_CAP` à 1 correspond à un dialogue strictement synchrone, plus simple à raisonner ; on
peut l'augmenter (jusqu'à 10) pour exploiter davantage le pipelining (voir
[Boucle principale](boucle_reseau.md)).

---

## 9. Comment régler l'IA

Ce module est le bon point d'entrée pour **expérimenter** sans risque sur la logique :

- Pour une IA plus **prudente** sur la nourriture : augmenter `FOOD_CRITICAL` et
  `FOOD_TRAVEL_TARGET`.
- Pour une équipe plus **nombreuse** : augmenter `POP_TARGET` (au prix d'une consommation accrue).
- Pour une coordination plus **réactive** : réduire les périodes de heartbeat (au prix d'un trafic
  de broadcasts plus dense).
- Pour un ralliement plus **patient** : augmenter `RALLY_STALL_LIMIT`.

Chaque réglage est un compromis ; les valeurs actuelles résultent d'essais empiriques contre le
serveur de référence.

---

[← La reproduction](reproduction.md) · [Sommaire](ai_doc_main.md)

[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)