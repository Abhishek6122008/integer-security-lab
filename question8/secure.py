# the fixed program, every finding from the table is addressed
# run with:  APP_PASS=yourpassword python3 secure.py

import hmac
import os
import secrets
from pathlib import Path

LOG_DIR = Path("logs").resolve()       # the only folder show_log may read from
TOKEN_BYTES = 32


def login(attempt):
    # the expected password comes from the environment, not from the source,
    # and a missing variable fails closed instead of matching an empty string
    expected = os.environ.get("APP_PASS")
    if not expected:
        return False
    # compare_digest takes the same time wherever the first wrong character is
    return hmac.compare_digest(attempt.encode(), expected.encode())


def make_token():
    # secrets draws from the operating system's CSPRNG, the same source as os.urandom
    return secrets.token_urlsafe(TOKEN_BYTES)


def show_log(name):
    # no shell and no subprocess, the file is opened directly, so there is
    # nothing left to interpret ; | or $( ) in the name.
    # the resolved path must still sit inside LOG_DIR, so ../../etc/passwd is refused
    path = (LOG_DIR / name).resolve()
    if not path.is_relative_to(LOG_DIR) or not path.is_file():
        print("refused:", name)
        return
    print(path.read_text(), end="")


if __name__ == "__main__":
    if login(input("password: ")):
        print("session token:", make_token())
        show_log(input("log file to view: "))
    else:
        print("access denied")
