#!/bin/sh
# builds both versions with ASan and saves the diagnostics into out/
mkdir -p out

# -g keeps line numbers so ASan can print which line caused the error
gcc -Wall -Wextra -fsanitize=address -g unsafe.c -o out/unsafe
gcc -Wall -Wextra -fsanitize=address -g safe.c   -o out/safe

# normal run, no error, shows the pointer and size at each stage
./out/unsafe > out/0_normal.txt 2>&1

# error (a), by default ASan aborts on an allocation this large, so
# allocator_may_return_null=1 tells it to return NULL like a real malloc
# would, and then the unchecked write through NULL is the reported error
ASAN_OPTIONS=allocator_may_return_null=1 ./out/unsafe 1 > out/1_null_deref.txt 2>&1

# error (b), the read and write through the freed pointer
./out/unsafe 2 > out/2_use_after_free.txt 2>&1

# corrected version, same option because it makes the huge request too,
# ASan prints one warning line and hands back NULL, the NULL check catches
# it, and no error is reported for the whole run
ASAN_OPTIONS=allocator_may_return_null=1 ./out/safe > out/3_safe.txt 2>&1

echo "outputs written to question3/out/"
