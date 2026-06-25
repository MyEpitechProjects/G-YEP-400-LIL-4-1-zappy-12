# La reproduction

Le module `fork.py` gère la **croissance de la population** de l'équipe. Comme les paliers
supérieurs réclament jusqu'à six joueurs de même niveau, une équipe trop peu nombreuse ne peut
tout simplement pas gagner. La reproduction est donc un pilier de la stratégie collective.

---

## 1. Le mécanisme du jeu

Un joueur peut pondre un **œuf** avec la commande `Fork`. Cet œuf crée une nouvelle **place** dans
l'équipe. Pour qu'un joueur occupe réellement cette place, il faut qu'un **nouveau client** se
connecte au serveur et réclame ce slot.

Le client IA exploite cela en **se relançant lui-même** : lorsqu'une place se libère, il démarre
un nouveau **processus** `zappy_ai` qui se connectera et incarnera un nouveau Trantorian. C'est ce
qui permet à l'équipe de grandir à partir d'un seul client lancé à la main.

---

## 2. Lancer un nouveau client : `spawn_child`

```python
def spawn_child(trantorian):
    command = [sys.executable, "-m", "ai.main", "-p", str(trantorian.port),
               "-n", trantorian.team_name, "-h", trantorian.machine]
    output = child_output(trantorian)
    subprocess.Popen(command, cwd=PACKAGE_ROOT, stdout=output, stderr=subprocess.STDOUT)
```

La fonction relance le programme lui-même, avec les mêmes paramètres de connexion (port, équipe,
machine). Quelques détails utiles :

- `sys.executable` est l'interpréteur Python courant : on relance donc exactement le même
  environnement, sans dépendre d'un chemin codé en dur.
- `cwd=PACKAGE_ROOT` positionne le répertoire de travail du processus enfant à la racine du
  projet, pour que `python -m ai.main` retrouve et importe correctement le paquet `ai`.
- La sortie de l'enfant est dirigée soit vers un fichier de log (utile aux tests), soit vers
  `/dev/null`, selon la fonction `child_output` qui consulte la variable d'environnement
  `ZAPPY_AI_LOGDIR`.

`spawn_children` n'est qu'une commodité qui appelle `spawn_child` plusieurs fois d'affilée.

---

## 3. Qui a le droit de gérer la population ? `is_population_manager`

```python
def is_population_manager(trantorian):
    return trantorian.king_is_stable() and \
           (time.time() - trantorian.start_time) > POP_WARMUP_SECONDS
```

**Seul le roi établi** gère la reproduction, et seulement après un délai d'échauffement
(`POP_WARMUP_SECONDS`). Deux raisons :

- **Un seul gestionnaire** évite que plusieurs joueurs ne forkent en même temps et ne fassent
  exploser la population de façon incontrôlée. Le roi étant unique, il est le candidat naturel.
- **Le délai d'échauffement** laisse le temps aux heartbeats de circuler. Tant que ce délai n'est
  pas écoulé, le recensement de la population n'est pas fiable : forker sur une estimation fausse
  conduirait à créer trop, ou trop peu, de joueurs.

---

## 4. Faut-il pondre un œuf ? `should_fork`

```python
def should_fork(trantorian):
    if not is_population_manager(trantorian):
        return False
    if trantorian.population_estimate() >= POP_TARGET:
        return False
    if trantorian.tick - trantorian.last_fork_tick < FORK_COOLDOWN:
        return False
    if trantorian.inventory.get("food", 0) < FOOD_LOW_LIMIT:
        return False
    return True
```

Le roi pond un œuf si **toutes** ces conditions sont réunies :

1. Il est bien le gestionnaire de population (roi établi, échauffement passé).
2. La population estimée est **sous la cible** `POP_TARGET`.
3. Le **cooldown** `FORK_COOLDOWN` est écoulé depuis le dernier fork : on ne pond pas en rafale.
4. Il dispose d'assez de nourriture (`FOOD_LOW_LIMIT`) : pondre coûte du temps (42 unités), il ne
   faut pas le faire au bord de la famine.

---

## 5. Combien de clients lancer ? `fill_count`

Pondre un œuf et **occuper** un œuf sont deux choses distinctes. `fill_count` répond à la seconde :
combien de nouveaux processus le roi doit-il démarrer pour occuper les places **déjà libres** ?

```python
def fill_count(trantorian):
    if not is_population_manager(trantorian):
        return 0
    if trantorian.population_estimate() >= POP_TARGET:
        return 0
    slots = trantorian.available_slots
    if slots is None or slots <= 0:          # None = un fork est en cours
        return 0
    return min(slots, FILL_BATCH)            # jamais plus de FILL_BATCH par tour
```

Le `FILL_BATCH` limite le nombre de clients lancés par tour, pour une croissance progressive et
maîtrisée. La valeur `None` de `available_slots` signale qu'un fork est en vol (le décompte est
provisoirement inconnu) : par prudence, on ne lance rien tant que le prochain `Connect_nbr` n'a pas
rafraîchi l'information.

---

## 6. Les deux chemins de spawn

Au final, deux mécanismes complémentaires font grandir l'équipe :

```
   ┌────────────────────────────────────────────────────────────┐
   │ A. Fork réussi (dans la boucle, handle_response)           │
   │    Le roi pond un œuf -> spawn_child() immédiat pour       │
   │    occuper aussitôt la place créée.                        │
   └────────────────────────────────────────────────────────────┘

   ┌────────────────────────────────────────────────────────────┐
   │ B. Après un Connect_nbr (dans run_ai)                      │
   │    Le roi apprend le nombre de places libres existantes    │
   │    -> spawn_children(fill_count(...)) pour les occuper.    │
   └────────────────────────────────────────────────────────────┘
```

- Le chemin **A** réagit à une ponte que le roi vient de provoquer.
- Le chemin **B** rattrape les places restées libres (par exemple celles disponibles au tout
  début de partie, ou des œufs apparus autrement).

Les deux respectent la même cible de population et les mêmes garde-fous, ce qui maintient
l'effectif de l'équipe autour de `POP_TARGET` sans dérive.

---

[← Les règles d'incantation](incantation.md) · [Sommaire](ai_doc_main.md) · [Suivant : Référence des constantes →](constantes.md)