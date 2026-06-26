# Documentation technique de l'IA

Cette documentation décrit en détail le fonctionnement du client **`zappy_ai`**, écrit en
Python 3. Il s'agit du programme qui **pilote un joueur** (un *Trantorian*) de façon totalement
autonome : une fois lancé, aucune intervention humaine n'est possible. L'IA observe le monde,
décide d'une action, l'envoie au serveur, intègre la réponse, puis recommence.

L'objectif de ces pages est double : expliquer **comment l'IA raisonne** (la logique de jeu,
le modèle de coordination) et documenter **comment le code est organisé** (les modules, leurs
responsabilités, leurs interactions).

#### Si vous cherchez à *installer*, *compiler* ou *lancer* le projet, reportez-vous d'abord à la
* [documentation d'installation](../installation.md) et à la
* [documentation de lancement](../lancement.md).

---

## Sommaire

1. [Architecture et flux de données](architecture.md) : Vue d'ensemble des modules, de leurs dépendances et du chemin que suit une action de bout en bout.
2. [Démarrage et connexion](demarrage.md) : Le point d'entrée, l'analyse des arguments et la poignée de main initiale avec le serveur.
3. [Boucle principale et communication réseau](boucle_reseau.md) : L'envoi des commandes, la lecture des réponses, le *pipelining* et la gestion des messages asynchrones.
4. [Le protocole et le décodage des réponses](protocole.md) : Comment les chaînes brutes du serveur deviennent des données exploitables, et le format des *heartbeats*.
5. [L'agent Trantorian : état et mémoire](agent.md) : La classe centrale qui mémorise tout ce que l'IA sait sur elle-même et sur ses coéquipiers.
6. [Navigation et géométrie](navigation.md) : Comment l'IA se repère sur une carte torique et traduit une cible en une suite de déplacements.
7. [La stratégie : le modèle « king-rally »](strategie.md) : Le coeur décisionnel de l'IA : l'élection d'un roi, la convergence des ouvriers, l'incantation.
8. [Les règles d'incantation](incantation.md)
   La vérification des prérequis d'élévation (pierres et nombre de joueurs).
9. [La reproduction](reproduction.md) : Comment le roi fait croître et entretient la population de l'équipe.
10. [Référence des constantes](constantes.md) : Le catalogue commenté de tous les paramètres de réglage de l'IA.

---

## Vue d'ensemble : l'idée directrice

Dans Zappy, une équipe gagne lorsque **six de ses joueurs atteignent le niveau 8**. Or, monter
en niveau (l'*élévation* ou *incantation*) exige de réunir, **sur une même case**, un certain
nombre de joueurs **du même niveau** ainsi que des pierres précises. Le défi n'est donc pas
seulement individuel (survivre, ramasser des ressources), il est surtout **collectif** :
plusieurs joueurs autonomes, qui ne partagent aucune mémoire et ne peuvent communiquer **que par
le serveur**, doivent réussir à se rassembler au bon endroit, au bon moment, avec le bon matériel.

Pour résoudre ce problème, l'IA adopte le modèle **« king-rally »**
(roi et ralliement) :

- Un joueur de l'équipe est élu **roi** (*king*). Le roi devient une **balise immobile** : il se
  fige sur une case et y reste.
- Tous les autres joueurs sont des **ouvriers** (*workers*). En écoutant les messages diffusés
  (*broadcasts*) par le roi, ils en perçoivent la **direction** et **convergent** vers lui tout
  en récoltant nourriture et pierres en chemin.
- Une fois rassemblés sur la case du roi, les ouvriers déposent les pierres nécessaires. Lorsque
  les conditions sont réunies, **un seul** d'entre eux déclenche l'incantation pour tout le groupe.

Toute la coordination repose ainsi sur un unique mécanisme : des messages périodiques appelés
**heartbeats** (battements de cœur), diffusés par radio. La direction d'où provient le son
remplace le partage de coordonnées, qui serait impossible puisque chaque joueur ne connaît sa
position que par rapport à son propre point d'apparition.

---

## Vocabulaire

| Terme | Signification |
|-------|---------------|
| **Trantorian** | Un habitant de la planète Trantor, c'est-à-dire un joueur piloté par une instance de l'IA. |
| **King (roi)** | Le joueur élu comme point de ralliement immobile de l'équipe. |
| **Worker (ouvrier)** | Tout joueur qui n'est pas roi ; il converge vers le roi. |
| **Heartbeat (battement)** | Message diffusé périodiquement contenant l'état d'un joueur (équipe, jeton, niveau, etc.). |
| **Token (jeton)** | Nombre aléatoire identifiant un joueur ; sert à départager les rôles de façon déterministe. |
| **Anchor (ancre)** | La case précise où le roi s'est figé ; son point de ralliement. |
| **Rally (ralliement)** | L'état d'un ouvrier arrivé sur la case du roi, en attente du groupe. |
| **Incantation / élévation** | Le rituel qui fait monter de niveau les joueurs présents sur une case. |
| **Vision** | L'ensemble des cases visibles autour du joueur, renvoyé par la commande `Look`. |
| **Dead-reckoning** | Estimation de la position par cumul des déplacements depuis l'apparition (navigation à l'estime). |
| **Pipelining** | Envoi de plusieurs commandes d'affilée avant d'en lire les réponses, autorisé par le protocole. |

---

[Sommaire](ai_doc_main.md) · [Suivant : Architecture et flux de données →](architecture.md)
 
[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)