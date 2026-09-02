#!/bin/sh
SRC=../question1/unsafe.c
mkdir -p out

gcc $SRC -o out/a1 2> out/1_noflags.txt
./out/a1 >> out/1_noflags.txt 2>&1

gcc -Wall -Wextra $SRC -o out/a2 2> out/2_wall.txt
./out/a2 >> out/2_wall.txt 2>&1

gcc -fsanitize=undefined -g $SRC -o out/a3 2> out/3_ubsan.txt
./out/a3 >> out/3_ubsan.txt 2>&1

gcc -fsanitize=address -g $SRC -o out/a4 2> out/4_asan.txt
./out/a4 >> out/4_asan.txt 2>&1

echo "outputs written to question2/out/"
