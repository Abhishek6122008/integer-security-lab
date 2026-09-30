#!/bin/sh
# builds the module under audit and the remediated module, runs every tool
# and saves each report into out/
mkdir -p out

# the hardening flag set used for deliverable 4, the before and after
# compiler warning comparison. -O2 is needed for _FORTIFY_SOURCE to work.
HARD="-Wall -Wextra -Wconversion -Wsign-conversion -Wformat=2 -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong"

echo "=== compiler warnings, hardening flags, before remediation ==="
gcc $HARD -c vuln.c  -o /dev/null > out/warn_before.txt 2>&1
echo "=== compiler warnings, hardening flags, after remediation ==="
gcc $HARD -c fixed.c -o /dev/null > out/warn_after.txt 2>&1

# plain builds, Valgrind needs binaries without the sanitizers
gcc -Wall -Wextra -g vuln.c  -o out/vuln
gcc -Wall -Wextra -g fixed.c -o out/fixed

# sanitizer builds, ASan for memory errors and UBSan for undefined behaviour
gcc -Wall -Wextra -fsanitize=address -g vuln.c  -o out/vuln_asan
gcc -Wall -Wextra -fsanitize=address -g fixed.c -o out/fixed_asan
gcc -Wall -Wextra -fsanitize=undefined -g vuln.c  -o out/vuln_ubsan
gcc -Wall -Wextra -fsanitize=undefined -g fixed.c -o out/fixed_ubsan

# every defect case of the module under audit, one report each.
# case 1 needs allocator_may_return_null so ASan hands back NULL like a real
# malloc instead of stopping the program on the oversized request itself.
for c in 1 2 3 4 5 6 7 8; do
  ASAN_OPTIONS=allocator_may_return_null=1 ./out/vuln_asan  $c > out/vuln_${c}_asan.txt 2>&1
  ./out/vuln_ubsan $c > out/vuln_${c}_ubsan.txt 2>&1
  valgrind --leak-check=full ./out/vuln $c > out/vuln_${c}_valgrind.txt 2>&1
done

# the remediated module, these three are the clean run evidence
valgrind --leak-check=full ./out/fixed > out/fixed_valgrind.txt 2>&1
./out/fixed_asan  > out/fixed_asan.txt 2>&1
./out/fixed_ubsan > out/fixed_ubsan.txt 2>&1

echo "reports written to question7/out/"
