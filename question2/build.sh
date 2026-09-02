#!/bin/sh
# builds question1/unsafe.c four times with different flags and saves
# the compiler warnings plus the program output into out/
SRC=../question1/unsafe.c
mkdir -p out

# 1. plain build, no flags at all, compiler stays silent
gcc $SRC -o out/a1 2> out/1_noflags.txt
./out/a1 >> out/1_noflags.txt 2>&1

# 2. -Wall -Wextra turns on the compile time warnings about
#    conversions that lose value, -Wconversion is the one that catches
#    int to short and float to int
gcc -Wall -Wextra -Wconversion $SRC -o out/a2 2> out/2_wall.txt
./out/a2 >> out/2_wall.txt 2>&1

# 3. UBSan instruments the program so undefined behaviour like the
#    float to int conversion is reported while it runs, -g keeps line numbers
gcc -fsanitize=undefined -g $SRC -o out/a3 2> out/3_ubsan.txt
./out/a3 >> out/3_ubsan.txt 2>&1

# 4. ASan checks memory errors at runtime, it catches the heap write
#    past the end of buf that the other three builds miss
gcc -fsanitize=address -g $SRC -o out/a4 2> out/4_asan.txt
./out/a4 >> out/4_asan.txt 2>&1

echo "outputs written to question2/out/"
