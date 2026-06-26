# Les règles d'incantation

Le module `rules.py` rassemble la **vérification des prérequis d'élévation**. C'est lui qui
répond à la question : « Puis-je monter de niveau ici et maintenant ? » Il s'appuie sur la table
des exigences définie dans [`constants.py`](constantes.md).

---

## 1. La table des exigences

Pour s'élever, un joueur doit réunir, sur sa case, un nombre minimal de joueurs de même niveau et
un certain assortiment de pierres. Ces seuils sont fixés par les règles du jeu :

| Élévation | Joueurs | linemate | deraumere | sibur | mendiane | phiras | thystame |
|-----------|:-------:|:--------:|:---------:|:-----:|:--------:|:------:|:--------:|
| 1 → 2 | 1 | 1 | 0 | 0 | 0 | 0 | 0 |
| 2 → 3 | 2 | 1 | 1 | 1 | 0 | 0 | 0 |
| 3 → 4 | 2 | 2 | 0 | 1 | 0 | 2 | 0 |
| 4 → 5 | 4 | 1 | 1 | 2 | 0 | 1 | 0 |
| 5 → 6 | 4 | 1 | 2 | 1 | 3 | 0 | 0 |
| 6 → 7 | 6 | 1 | 2 | 3 | 0 | 1 | 0 |
| 7 → 8 | 6 | 2 | 2 | 2 | 2 | 2 | 1 |

**Choix d'implémentation important.** Dans le code, cette table (`REQUIREMENTS`) est indexée par le **niveau cible**, c'est-à-dire le niveau que l'on cherche à atteindre (de 2 à 8), et non par le niveau actuel. Cela rend la recherche très naturelle : pour un joueur de niveau `L`, les exigences sont `REQUIREMENTS[L + 1]`. Et comme il n'existe pas de clé `9`, un joueur de niveau 8 ne trouve aucune exigence : il est arrivé au sommet, sans qu'il faille traiter ce cas à part.

On remarque aussi la montée en exigence : les premiers niveaux se font à un ou deux joueurs, mais les paliers 6→7 et 7→8 réclament **six joueurs** de même niveau. C'est ce qui justifie tout l'effort de coordination et de reproduction décrit ailleurs dans cette documentation.

---

## 2. Vérifier strictement : `can_start_incantation`

C'est la vérification **finale et stricte**, celle qui conditionne le lancement réel de
l'incantation. Elle exige que **tout** soit présent **physiquement sur la case** :

```python
def can_start_incantation(level, inventory, vision):
    level_to_reach = level + 1
    if level_to_reach not in REQUIREMENTS:   # déjà au niveau 8
        return False
    current_tile = vision[0]
    if current_tile.get("player", 0) < requirements.get("players", 0):
        return False
    for stone in STONES:                     # chaque pierre requise doit être là
        if current_tile.get(stone, 0) < requirements.get(stone, 0):
            return False
    return True
```

Elle se fonde sur la **vision** (ce que le `Look` rapporte réellement sur la case 0), pas sur ce
que l'agent croit avoir déposé. C'est volontaire : seul ce qui est physiquement sur la case compte
pour le serveur, qui revérifie lui-même les conditions au début et à la fin du rituel.

Cette fonction est complétée, dans la stratégie, par `can_initiate` (voir [Stratégie](strategie.md)), qui ajoute la règle de l'**initiateur unique** : parmi les joueurs de même niveau présents, seul celui au plus petit jeton lance effectivement l'incantation.

---

## 3. Les vérifications « souples » pour la récolte

Avant d'en arriver au lancement, l'IA a besoin d'évaluations plus tolérantes pour guider la
récolte et la convergence. Trois fonctions y pourvoient.

### `has_required_stones_in_inventory` : suis-je assez chargé ?

Cette fonction estime si l'inventaire contient « presque » de quoi équiper le rituel. Plutôt
qu'une égalité stricte, elle tolère un certain manque, **croissant avec le niveau** :

| Niveau cible | Tolérance sur le manque total de pierres |
|--------------|------------------------------------------|
| ≤ 3 | 0 (il faut tout) |
| ≤ 5 | jusqu'à 2 pierres manquantes |
| ≤ 7 | jusqu'à 5 |
| 8 | jusqu'à 8 |

L'idée : aux premiers niveaux, peu de pierres sont nécessaires, on peut donc tout exiger ; aux
niveaux élevés, les besoins explosent et il serait paralysant d'attendre l'assortiment parfait
avant d'avancer. La tolérance reflète cette réalité.

### `missing_stone_visible` : ce qui me manque est-il en vue ?

Indique si l'une des pierres encore manquantes apparaît quelque part dans la vision. Utile pour
décider s'il vaut la peine d'aller la chercher.

### `enough_players_present` : sommes-nous assez nombreux ?

Vérifie simplement que la case actuelle compte assez de joueurs pour le prochain palier, d'après
ce que rapporte le `Look`.

---

## 4. Pourquoi deux niveaux de vérification ?

La distinction entre vérifications **strictes** (`can_start_incantation`) et **souples**
(`has_required_stones_in_inventory`, etc.) est délibérée :

- Les vérifications **souples** servent en amont, pour orienter le comportement : « est-ce que ça
  vaut le coup de continuer à récolter, de converger, de rester sur place ? » Elles évitent que
  l'IA n'attende un état parfait avant d'agir.
- La vérification **stricte** sert au dernier moment, juste avant d'engager l'`Incantation`. Comme
  le serveur revérifie les conditions au début **et** à la fin du rituel, lancer une incantation
  qui ne remplit pas exactement les critères serait du temps perdu (300 unités de temps gelées
  pour rien). La rigueur finale protège donc contre les élévations vouées à l'échec.

---

[← La stratégie king-rally](strategie.md) · [Sommaire](ai_doc_main.md) · [Suivant : La reproduction →](reproduction.md)