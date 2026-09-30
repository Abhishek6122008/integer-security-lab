#!/bin/sh
# builds all six snippets twice, once plain for Valgrind and once with ASan,
# then saves every report into out/
mkdir -p out

for f in init leak doublefree; do
  for v in buggy fixed; do
    # plain build, Valgrind needs a binary without ASan, the two clash
    gcc -Wall -Wextra -g ${f}_${v}.c -o out/${f}_${v}
    # ASan build of the same snippet
    gcc -Wall -Wextra -fsanitize=address -g ${f}_${v}.c -o out/${f}_${v}_asan

    # --leak-check=full makes Memcheck list each leaked block with its stack
    valgrind --leak-check=full ./out/${f}_${v} > out/${f}_${v}_valgrind.txt 2>&1
    ./out/${f}_${v}_asan > out/${f}_${v}_asan.txt 2>&1
  done
done

echo "twelve reports written to question5/out/"
