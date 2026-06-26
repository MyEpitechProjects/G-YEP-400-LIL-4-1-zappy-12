# Navigation et géométrie

Le module `navigation.py` regroupe les outils géométriques de l'IA. Il répond à trois questions
récurrentes : « où se trouve une case que je vois ? », « comment aller vers un son ? » et « où
suis-je après ce déplacement ? ». Toutes ces fonctions sont **pures** (pas d'effet de bord), ce
qui les rend simples à tester.

---

## 1. Le repère et l'orientation

L'orientation est codée par un entier : `0 = Nord`, `1 = Est`, `2 = Sud`, `3 = Ouest`. À chaque
orientation correspond un déplacement élémentaire vers l'avant :

```python
FORWARD_DELTA = [(0, -1), (1, 0), (0, 1), (-1, 0)]
#                  N        E       S        O
```

Avancer revient à ajouter ce delta à la position. Comme la carte est **torique** (un donut en langage plus famillier) (sortir par la
droite fait réapparaître à gauche, etc.), on applique un modulo sur les dimensions.

```python
def step_forward(x, y, orientation, width, height):
    dx, dy = FORWARD_DELTA[orientation]
    return (x + dx) % width, (y + dy) % height
```

Les rotations sont triviales : `turn_right` fait `+1 mod 4`, `turn_left` fait `-1 mod 4`.

---

## 2. Le cône de vision et la numérotation des cases

La commande `Look` renvoie les cases visibles dans un ordre bien précis, en forme de triangle qui
s'élargit devant le joueur. Le joueur occupe la case 0 ; devant lui s'étendent des rangées de plus
en plus larges :

```
      9 10 11 12 13 14 15      <- rangée 3 (7 cases)
         4  5  6  7  8         <- rangée 2 (5 cases)
            1  2  3            <- rangée 1 (3 cases)
               0               <- le joueur
```

Chaque rangée `r` contient `2r + 1` cases et commence à l'indice `r²`. Cette régularité permet,
à partir d'un simple indice de case, de retrouver à quelle distance (quelle rangée) elle se
trouve :

```python
def distance_from_tile_index(tile_index):
    distance = 0
    while (distance + 1) * (distance + 1) <= tile_index:
        distance += 1
    return distance
```

Par exemple : la case 6 appartient à la rangée 2 (car `2² = 4 ≤ 6 < 9 = 3²`), elle est donc à
2 cases devant.

---

## 3. Atteindre une case visible : `build_move_plan`

Connaissant l'indice d'une case du cône de vision, cette fonction construit la **suite de
déplacements** pour s'y rendre. L'idée : une case se situe sur une certaine rangée (distance vers
l'avant) et à un certain décalage latéral par rapport au centre de cette rangée.

```python
row = distance_from_tile_index(tile_index)   # combien de cases en avant
row_center = row * row + row                  # l'indice du centre de la rangée
offset = tile_index - row_center              # <0 = à gauche, >0 = à droite
```

Le plan se construit alors en trois temps :

1. Si la case est décalée à gauche (`offset < 0`) : tourner à gauche, avancer `|offset|` cases,
   puis se réorienter vers l'avant.
2. Si elle est décalée à droite (`offset > 0`) : tourner à droite, avancer `offset` cases, puis
   se réorienter.
3. Dans tous les cas, avancer `row` cases vers l'avant pour atteindre la bonne rangée.

La case 0 (le joueur lui-même) renvoie un plan vide : on y est déjà.

---

## 4. Se diriger vers un son : `moves_toward_sound`

Lorsqu'un broadcast arrive, le serveur indique la **direction** d'où vient le son sous forme d'un
nombre de 1 à 8 (0 signifiant « ici même »). La numérotation tourne autour du joueur, la case 1
étant droit devant. La fonction traduit cette direction en un premier déplacement vers la source :

| Direction | Position relative | Mouvement |
|-----------|-------------------|-----------|
| 1 | Devant | `Forward` |
| 2, 3, 4 | Côté gauche | `Left`, `Forward` |
| 5 | Derrière | `Left`, `Left`, `Forward` (demi-tour) |
| 6, 7, 8 | Côté droit | `Right`, `Forward` |

C'est volontairement **approximatif** : on ne calcule pas un chemin exact, on fait un pas dans la
bonne direction. À chaque nouveau broadcast du roi, la direction se rafraîchit, et l'ouvrier se
rapproche pas à pas. C'est suffisant et robuste, car l'émetteur (le roi) est immobile : tant qu'on
réduit la distance, on finit par arriver.

---

## 5. Le chemin le plus court sur un tore : `navigate_to`

Cette fonction sert au roi pour revenir sur son ancre s'il s'en fait éjecter. Sur une carte
torique, il existe deux façons d'atteindre une cible sur chaque axe : par exemple aller vers
l'Est, ou faire le tour par l'Ouest. La fonction choisit toujours la **plus courte**.

```python
dx = (target_x - x) % width        # distance vers l'Est
dy = (target_y - y) % height       # distance vers le Sud
if dx != 0:
    target_orientation = 1 if dx <= width - dx else 3   # Est(1) vs Ouest(3)
    return turns_to_face(orientation, target_orientation) + ["Forward"]
```

Elle compare `dx` (aller vers l'Est) à `width - dx` (faire le tour par l'Ouest) et retient le
sens le moins coûteux, puis renvoie les rotations nécessaires suivies d'un `Forward`. Elle traite
d'abord l'axe horizontal, puis le vertical : un pas à la fois, jusqu'à ce que la cible soit
atteinte.

La fonction `turns_to_face` calcule le minimum de rotations pour passer d'une orientation à une
autre : aucune si déjà alignée, un `Right` ou un `Left` pour un quart de tour, un demi-tour
(`Right`, `Right`) pour l'opposé.

---

[← L'agent Trantorian](agent.md) · [Sommaire](ai_doc_main.md) · [Suivant : La stratégie king-rally →](strategie.md)