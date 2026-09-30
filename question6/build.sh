#!/bin/sh
# runs cppcheck and gcc on both versions and saves the output into out/
mkdir -p out

# --enable=all turns on the style, performance and unused checks as well as
# the error checks, which is what reports the dead function and the leak
cppcheck --enable=all --suppress=missingIncludeSystem dirty.c > out/cppcheck_dirty.txt 2>&1
cppcheck --enable=all --suppress=missingIncludeSystem clean.c > out/cppcheck_clean.txt 2>&1

# the compiler warnings for the same two files, for comparison
gcc -Wall -Wextra -c dirty.c -o /dev/null > out/gcc_dirty.txt 2>&1
gcc -Wall -Wextra -c clean.c -o /dev/null > out/gcc_clean.txt 2>&1

echo "outputs written to question6/out/"
