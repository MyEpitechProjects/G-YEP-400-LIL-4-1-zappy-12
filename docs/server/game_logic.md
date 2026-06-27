# Mécaniques de Jeu et Gestion du Temps

La logique de jeu (Game Engine) du serveur Zappy est séparée de la logique réseau. Elle gère le temps, l'état du monde (la carte torique), le système d'inventaire, de nourriture, la reproduction (les œufs) et le niveau des IA.

## 1. La Carte Torique
La carte du jeu est une grille 2D mais mathématiquement, elle se comporte comme un Tore (une surface de donut).
Si un joueur dépasse la bordure droite de la carte, il se retrouve instantanément sur le bord gauche (pareil pour l'axe Y). 
Cela est géré au niveau du serveur dans le calcul des positions (modulo la largeur/hauteur `args->width` et `args->height`).
Une case contient un nombre défini de ressources et la liste des joueurs (identifiables par UUID) présents sur celle-ci.

## 2. Le Système de Temps (Fréquence)
La variable `-f` passée au lancement du serveur dicte la fréquence de calcul (valeur par défaut : 100).
Dans l'architecture du code (via `clock.c`), une "unité de temps" équivaut à `1.0 / fréquence` secondes.

Le serveur utilise la fonction système pour récupérer le temps écoulé de façon extrêmement précise et l'injecte dans le `poll()` timeout.
Ainsi, le serveur ne fait jamais d'attente active (busy-waiting). Le `poll()` se met en veille et se réveille exactement au moment où l'action d'un client se termine ou quand des ressources doivent respawn.

## 3. Coût et file d'attente des commandes
Chaque commande a un coût temporel défini par la norme (ex : `Forward` coûte 7 unités).
Lorsqu'un client envoie une commande, elle n'est pas exécutée immédiatement, mais ajoutée dans une file d'attente (queue) propre à ce joueur. La commande est accompagnée d'un "timer cible".
À chaque "tick" du serveur, le temps est vérifié :
Si le temps courant dépasse le timer cible de la commande en tête de file, la commande est extraite, la logique est appliquée (le joueur avance), et un message réseau "ok" est envoyé en réponse. 

Une IA peut empiler au maximum 10 commandes dans sa queue en même temps. Toute commande supplémentaire sera simplement ignorée tant que la queue est pleine.

## 4. Survie et Faim (Le Famine Timer)
La condition de survie de base d'un Trantorien (IA) est la nourriture. 
- Au spawn, une IA possède 10 unités de `food`. 
- Chaque `food` permet au joueur de survivre pendant 126 unités de temps.
- Ce processus est implémenté via un timer persistant rattaché à chaque client. 
- À expiration de ce timer (126/f), une `food` est consommée dans l'inventaire du joueur, et le timer repart à zéro.
- Si le timer expire et qu'il n'y a plus de `food` dans l'inventaire, la fonction `protocol_death` est invoquée : le joueur meurt de faim, la connexion est coupée et le message "mort" est envoyé.

## 5. Régénération des Ressources sur la Carte
La génération des ressources dépend d'une densité fixe par type (ex: 50% de chance sur une case, ou quantité globale sur la carte). 
Toutes les 20 unités de temps, le serveur re-calcule les ressources disponibles sur l'ensemble de la carte et distribue de nouveaux items aléatoirement (linemate, thystame, food...) afin de maintenir une quantité suffisante de ressources pour les joueurs restants. Cette vérification permet d'éviter l'épuisement total des minéraux et garantit l'évolution constante des IA vers le niveau 8.

---

[Précédent : Architecture](architecture.md)

[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)