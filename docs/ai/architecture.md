# Architecture et flux de données

Cette page présente l'organisation générale du client IA : ses modules, leurs responsabilités et
la manière dont ils collaborent pour produire une action à chaque tour.

---

## 1. Organisation des modules

Le code est découpé en modules à responsabilité unique, regroupés dans le paquet Python `ai/`.
On peut les classer en trois catégories.

### Catégorie d'entrée / réseau

| Module | Rôle |
|--------|------|
| `main.py` | Point d'entrée. Orchestre l'initialisation puis lance la boucle. |
| `args_parser.py` | Analyse les arguments de la ligne de commande (`-p`, `-n`, `-h`). |
| `connection.py` | Établit la première connexion et réalise la poignée de main du protocole. |
| `loop.py` | Coeur réseau : envoi des commandes, lecture des réponses, boucle principale. |
| `protocol.py` | Décode les chaînes brutes du serveur en structures de données. |

### Catégorie d'état

| Module | Rôle |
|--------|------|
| `trantorian.py` | La classe `Trantorian` : toute la mémoire de l'agent et l'intégration des réponses. |

### Catégorie de décision (la logique de jeu)

| Module | Rôle |
|--------|------|
| `strategy.py` | Le « cerveau » : la machine à états du modèle king-rally qui choisit la prochaine action. |
| `rules.py` | Les prérequis d'incantation (pierres et joueurs présents). |
| `navigation.py` | La géométrie : conversion d'une cible en déplacements, repérage sur carte torique. |
| `fork.py` | La reproduction : croissance de la population gérée par le roi. |
| `constants.py` | Tous les paramètres de réglage centralisés. |

---

## 2. Dépendances entre modules

Les flèches se lisent « dépend de » (importe et utilise).

```
                         main.py
                            │
        ┌───────────────────┼───────────────────┐
        ▼                   ▼                   ▼
 args_parser.py      connection.py            loop.py
                                                 │
                     ┌───────────────────────────┼───────────────┐
                     ▼                           ▼               ▼
                 strategy.py               trantorian.py     protocol.py
                     │                           │
        ┌────────────┼─────────────┐             ▼
        ▼            ▼             ▼          navigation.py
     rules.py   navigation.py    fork.py
        │            │             │
        └────────────┴─────────────┴──────────► constants.py
```

Quelques principes lisibles sur ce schéma :

- `constants.py` ne dépend de rien : c'est une feuille de l'arbre, importée partout.
- `protocol.py` est **pur** (aucune dépendance interne) : il ne fait que transformer des chaînes
  en données. Cela le rend très facile à tester.
- `strategy.py` est au centre : il s'appuie sur `rules`, `navigation` et `fork`, et lit/écrit
  l'état porté par `trantorian`.
- Aucune communication latérale entre joueurs : la seule « mémoire partagée » passe par les
  messages diffusés, traités via le serveur.

---

## 3. Le flux d'une action, de bout en bout

À chaque tour, l'IA suit toujours le même cycle. Le voici décrit pas à pas, avec le module
responsable de chaque étape.

```
   ┌──────────────────────────────────────────────────────────┐
   │  1. build_plan(trantorian)            (strategy.py)      │
   │     Décide d'une liste de commandes à envoyer.           │
   └──────────────────────────────┬───────────────────────────┘
                                   ▼
   ┌──────────────────────────────────────────────────────────┐
   │  2. execute_plan(...)                 (loop.py)          │
   │    Envoie les commandes (pipelining) et lit les réponses.│
   └──────────────────────────────┬───────────────────────────┘
                                  ▼
   ┌──────────────────────────────────────────────────────────┐
   │  3. read_response(...)                (loop.py)          │
   │     Saute les messages asynchrones, renvoie la vraie     |
   │     réponse attendue.                                    │
   └──────────────────────────────┬───────────────────────────┘
                                  ▼
   ┌──────────────────────────────────────────────────────────┐
   │  4. parse_* (...)                     (protocol.py)      │
   │     Transforme la chaîne brute en données structurées.   |
   └──────────────────────────────┬───────────────────────────┘
                                  ▼
   ┌──────────────────────────────────────────────────────────┐
   │  5. update_from_response(...)         (trantorian.py)    │
   │     Met à jour l'inventaire, la vision, la position, etc.│
   └──────────────────────────────┬───────────────────────────┘
                                  ▼
                          (retour à l'étape 1)
```

En résumé : **décider → envoyer → lire → décoder → intégrer**, puis recommencer. Ce cycle ne
s'arrête que lorsque le joueur meurt ou que la connexion est perdue.

---

## 4. Un point de conception important : pas de coordonnées partagées

L'IA ne reçoit jamais sa position absolue de la part du serveur. Chaque agent estime sa position
par **navigation à l'estime** (*dead-reckoning*) : il part de `(x, y)` à l'apparition et cumule
ses déplacements `Forward` / `Right` / `Left` pour suivre où il croit se trouver.

La conséquence est essentielle : **deux joueurs n'ont pas le même repère**. Les coordonnées d'un
joueur n'ont aucun sens pour un autre. C'est précisément pour cela que la coordination repose sur
les **directions sonores** des broadcasts plutôt que sur l'échange de positions : la direction
d'où vient un son est, elle, une information valable pour celui qui l'entend.

L'ancre (`anchor`) du roi n'échappe pas à cette règle : elle n'est utilisée que par le roi
lui-même, pour revenir sur sa propre case s'il s'en fait éjecter. Elle n'est jamais transmise aux
autres.

---

[← Sommaire](ai_doc_main.md) · [Suivant : Démarrage et connexion →](demarrage.md)