# Documentation des tests unitaires

## 1. Pré-requis

Assurez-vous d'avoir les binaires suivants avant de procéder à la suite :
* zappy_server
* zappy_ai

Si ce n'est pas le cas, vous pouvez suivre la [documentation de lancement](lancement.md).

## 2. Compilation des tests unitaire

Exécutez la commande suivante :
```bash
make tests_run
```

Vous verrez ensuite les tests qui passent et qui ne passent pas.

## 3. Tests de fonctionnels et de charges

Exécutez la commande suivante :
```bash
make integration_tests
```

De meme vous devriez voir les tests fonctionnels et de chargent qui passent

## 3. Coverage

Si vous voulez voir le coverage, exécutez les commandes suivantes :

#### Serveur en C
```bash
gcovr --gcov-executable "llvm-cov gcov" -r . .

```

#### IA en Python
```bash
python3 -m coverage report -m
```

## 4. Coverage HTML

Si vous préférez voir le coverage en HTML, exécutez les commandes suivantes :

#### Serveur en C
```bash
gcovr --gcov-executable "llvm-cov gcov" -r . --html-details -o coverage_server.html && xdg-open coverage_sever.html
```

#### IA en Python
```bash
python3 -m coverage html && xdg-open htmlcov/index.html
```

**Cela génère beaucoup de fichier, n'hésitez pas a les nettoyer avec make clean**

[Retour à la documentation principale](https://github.com/EpitechPGE2-2025/G-YEP-400-LIL-4-1-zappy-12/tree/main)
