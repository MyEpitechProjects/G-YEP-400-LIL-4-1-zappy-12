# Documentation de la CI

La partie CI est réalisée grâce à un workflow GitHub Actions : [test_compilation.yml](../.github/workflows/test_compilation.yml).

À chaque push (ou manuellement via l'onglet « Actions »), le script exécute les étapes suivantes :
* Se place dans l'environnement officiel EPITECH (container Docker `epitechcontent/epitest-docker`)
* Vérifie que la commande `make zappy_server` fonctionne et produit bien le binaire `zappy_server`
* Vérifie que la commande `make zappy_ai` fonctionne et produit bien le binaire `zappy_ai`
* Vérifie que la commande `make tests_run` lance bien les tests unitaires
* Vérifie que la commande `make all` fonctionne, même lorsque les binaires sont déjà présents
* Vérifie que la commande `make clean` enlève bien les `.o` et les autres fichiers temporaires, sans supprimer les binaires
* Vérifie que la commande `make fclean` enlève bien les binaires

En cas d'échec d'une des étapes, le script s'arrête et l'erreur est signalée en rouge dans l'onglet « Actions » de GitHub. À l'inverse, si tout fonctionne, l'onglet « Actions » affiche en vert que l'ensemble des étapes a réussi.

[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)
