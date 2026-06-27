#!/bin/bash

cd server || exit 1

make tests_run

python3 functional_test.py

python3 load_test.py

echo "Compilation des tests fini"