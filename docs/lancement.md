# Documentation de lancement

Assurez-vous d'avoir bien suivi la [documentation d'installation](installation.md).

## 1. Compilation

Pour compiler tout le projet, faites simplement :
```bash
make
```
Si vous ne voulez compiler que le serveur, faites simplement :
```bash
make zappy_server
```
Si vous ne voulez compiler que l'IA, faites simplement :
```bash
make zappy_ai
```
Pour retirer les `.o`, mais pas les binaires, faites simplement :
```bash
make clean
```
Et pour nettoyer les fichiers `.o` et les binaires, faites simplement :
```bash
make fclean
```
En cas de problème de compilation, n'hésitez pas à simplement faire :
```bash
make re
```

Assurez-vous d'avoir bien les binaires `zappy_server`, `zappy_gui` et `zappy_ai`.

## 2. Lancement du serveur

L'exécutable du serveur est `zappy_server`. Pour lancer une partie, lancez d'abord le serveur avec :

```bash
./zappy_server -p port -x width -y height -n name1 name2 name3 ... -c clientsNb -f freq
```

| Option | Description |
|--------|-------------|
| -p port | Le port sur lequel le serveur tourne |
| -x width | La largeur du monde |
| -y height | La hauteur du monde |
| -n name1 name2 name3 | Les noms des équipes |
| -c clientsNb | Le nombre de joueurs maximum par équipe |
| -f freq | La rapidité du jeu : plus f est élevé, plus le jeu se joue rapidement, mais consomme plus de ressources |

Un exemple de commande peut être :
```bash
./zappy_server -p 4242 -x 20 -y 20 -n pomme banane cerise orange -c 40 -f 100
```

Ne fermez pas ce terminal, sinon vous devrez relancer la commande.

## 3. Lancement du graphique

L'exécutable du graphique est `zappy_gui`. Pour lancer une partie, lancez d'abord le GUI avec :

```bash
./zappy_gui -p port -h machine
```

| Option | Description |
|--------|-------------|
| -p port | Port sur lequel votre serveur tourne |
| -h machine | Machine (IP) sur laquelle tourne votre serveur, `localhost` par défaut, ou si vous faites tourner le serveur sur votre PC |

Un exemple de commande peut être :
```bash
./zappy_gui -p 4242 -h localhost
```

Ne fermez pas votre terminal, sinon vous ne pourrez pas voir les IA évoluer.

## 4. Lancement des IA

Attention, il faudra lancer une IA par joueur.

L'exécutable de l'IA est `zappy_ai`. Pour lancer une partie, lancez d'abord l'IA avec :

```bash
./zappy_ai -p port -n name -h machine
```

| Option | Description |
|--------|-------------|
| -p port | Port sur lequel votre serveur tourne |
| -n name | Nom de l'équipe à laquelle l'IA appartiendra |
| -h machine | Machine (IP) sur laquelle tourne votre serveur, `localhost` par défaut, ou si vous faites tourner le serveur sur votre PC |

Ne fermez pas votre terminal, car l'IA associée à un terminal mourra automatiquement si le terminal est fermé.

Profitez du spectacle.

## 5. Fin

Quand une équipe a gagné ou que tous les Trantorians sont morts, le serveur se ferme et vous pouvez fermer vos terminaux.

[Retour à la documentation principale](../README.md)
