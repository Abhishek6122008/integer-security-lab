#!/bin/sh
# runs Bandit on both versions, then runs both programs with the same
# injected input, and saves every output into out/
mkdir -p out

# bandit exits with 1 when it finds something, so no set -e here
bandit insecure.py > out/bandit_insecure.txt 2>&1
bandit secure.py   > out/bandit_secure.txt 2>&1

# the same input to both, ; ends the cat command and the shell runs id next
printf 'admin123\napp.log; id\n' | python3 insecure.py > out/run_insecure.txt 2>&1
printf 'admin123\napp.log; id\n' | APP_PASS=admin123 python3 secure.py > out/run_secure.txt 2>&1

echo "outputs written to question8/out/"
