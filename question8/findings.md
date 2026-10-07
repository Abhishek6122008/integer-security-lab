# Bandit Static Analysis of an Insecure Python Program

Original: `insecure.py`. Fixed: `secure.py`. Each finding is marked `F1`..`F4` in
the comments of `insecure.py`. `logs/app.log` is a two line sample file so both
programs have something to show.

## Findings table

| ID | Practice | Location in `insecure.py` | Bandit test, severity / confidence | Why it is unsafe | Standard rule |
|---|---|---|---|---|---|
| F1 | Hard-coded password | line 8, `ADMIN_PASSWORD = "admin123"` | B105 hardcoded_password_string, Low / Medium | The password ships in the source, in every clone of the repository and in the git history, where it stays even after the line is deleted. It is the same on every deployment and cannot be rotated without a code change | CWE-259; OWASP A07 Identification and Authentication Failures |
| F2 | `subprocess` with `shell=True` on user input | line 21, `subprocess.call("cat logs/" + name, shell=True)` | B602 subprocess_popen_with_shell_equals_true, High / High | The whole string is handed to `/bin/sh`, which interprets `;`, `\|`, `&`, `$( )` and backticks. The input `app.log; id` runs `id` as a second command with the program's privileges. Arbitrary command execution | CWE-78; OWASP A03 Injection |
| F3 | `random` used for a security token | line 17, `random.choice(chars)` in `make_token` | B311 blacklist (random), Low / High | `random` is the Mersenne Twister, built for simulations. Its output is fully determined by its internal state, and that state can be recovered from enough observed output, after which every future token can be computed. Anyone who collects enough tokens can forge the next user's session | CWE-330; OWASP A02 Cryptographic Failures |
| F4 | `subprocess` imported | line 6, `import subprocess` | B404 blacklist (import_subprocess), Low / High | Not a defect on its own. Bandit flags the import so that a reviewer reads every place the module starts a process, which is where F2 lives | CWE-78 |

Bandit's severity is about the pattern, not the impact. F1 is rated Low with
Medium confidence because Bandit only sees a password-like variable name assigned a
string, and cannot tell a real credential from a placeholder; the impact of a real
admin password in source is still a full account takeover.

## What the fix changed

- The password is read from the `APP_PASS` environment variable at call time, and
  a missing variable fails the login closed instead of matching an empty string.
  The comparison uses `hmac.compare_digest`, because `==` returns at the first
  wrong character and the time it takes leaks how much of the guess was right. A
  real system would store a salted hash (bcrypt, argon2) rather than the password
  itself. (F1)
- The shell is gone entirely. The file is opened with `pathlib`, so there is
  nothing left to interpret `;` or `$( )` in the name, and the resolved path must
  sit inside `logs/`, so `../../etc/passwd` is refused as well. Removing the shell
  alone would have left that path traversal open. (F2, F4)
- `random.choice` becomes `secrets.token_urlsafe(32)`, 32 bytes from the
  operating system's CSPRNG, the same source as `os.urandom`. (F3)

If an external program were genuinely needed, the fix for F2 would be an argument
list with `shell=False`, `subprocess.run(["cat", str(path)])`, which passes the
arguments straight to `execve` with no shell in between. Bandit would still log
B404 and B603 at Low severity as reminders to review the call, which is why this
program avoids starting a process at all.

## Bandit

```sh
bandit insecure.py
bandit secure.py
```

On `insecure.py` Bandit reports four issues, one High (B602) and three Low (B105,
B311, B404), and exits with 1. On `secure.py` it prints `No issues identified.`
and exits with 0. The after run also shows `Total lines skipped (#nosec): 0`,
which is the proof that the findings were fixed rather than silenced with a
`# nosec` comment.

Bandit is a static analyser: it parses the source into a syntax tree and matches
known bad patterns, without running anything. That is why it flags `shell=True`
whether or not the input is trusted, and also why a clean report is not proof of
safety. It has no test for path traversal, so the `logs/` check in `secure.py`
came from reading the code, not from the tool.

Install it with `pipx install bandit`, not `apt install bandit`. Ubuntu 26.04
ships Bandit 1.7.10, which crashes on Python 3.14, skips the file with
`exception while scanning file`, and still prints `No issues identified.` on
`insecure.py`. A scanner that fails open is worse than no scanner, so the
`Files skipped` line is worth checking on every run.
