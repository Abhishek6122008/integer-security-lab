#!/bin/sh
# builds both files with ASan and saves every run into out/
mkdir -p out

# -fsanitize=address turns on ASan, -g keeps the line numbers
gcc -Wall -Wextra -fsanitize=address -g original.c -o out/original
gcc -Wall -Wextra -fsanitize=address -g fixed.c    -o out/fixed

# snippet 1, ASan stops the program itself on a request this large, so
# allocator_may_return_null=1 makes it return NULL like a real malloc and
# the unchecked write through NULL becomes the reported error
ASAN_OPTIONS=allocator_may_return_null=1 ./out/original 1 > out/1_null_deref.txt 2>&1

# snippet 2, read and write through the freed pointer
./out/original 2 > out/2_use_after_free.txt 2>&1

# snippet 3, the same block freed twice
./out/original 3 > out/3_double_free.txt 2>&1

# snippet 4, LeakSanitizer reports the block at exit
./out/original 4 > out/4_leak.txt 2>&1

# the fixed file, all four snippets in one run, ASan reports nothing
ASAN_OPTIONS=allocator_may_return_null=1 ./out/fixed > out/5_fixed_clean.txt 2>&1

echo "outputs written to question4/out/"
