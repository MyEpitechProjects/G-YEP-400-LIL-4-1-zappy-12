# Documentation des tests unitaires

## 1. Pré-requis

Assurez-vous d'avoir les binaires suivants avant de procéder à la suite :
* zappy_server
* zappy_ai

Si ce n'est pas le cas, vous pouvez suivre la [documentation de lancement](lancement.md).

## 2. Compilation des tests

Exécutez la commande suivante :
```bash
make tests_run
```

Vous verrez ensuite les tests qui passent et qui ne passent pas.

## 3. Coverage

Si vous voulez voir le coverage, exécutez les commandes suivantes :

#### Serveur en C
```bash
./coverage.sh
```

#### IA en Python
```bash
python3 -m coverage report -m
```

## 4. Coverage HTML

Si vous préférez voir le coverage en HTML, exécutez les commandes suivantes :

#### Serveur en C
```bash
xdg-open coverage_report/index.html```
```

#### IA en Python
```bash
python3 -m coverage html && xdg-open htmlcov/index.html
```

[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)
