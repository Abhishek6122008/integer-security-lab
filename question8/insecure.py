# the insecure program, every line marked with a number is a finding in the table
# run with:  python3 insecure.py

import random
import string
import subprocess                                       # F4, subprocess imported

ADMIN_PASSWORD = "admin123"                             # F1, password in source


def login(attempt):
    return attempt == ADMIN_PASSWORD


def make_token():
    chars = string.ascii_letters + string.digits
    return "".join(random.choice(chars) for _ in range(32))    # F3, random for a token


def show_log(name):
    subprocess.call("cat logs/" + name, shell=True)     # F2, shell=True with user input


if __name__ == "__main__":
    if login(input("password: ")):
        print("session token:", make_token())
        show_log(input("log file to view: "))
    else:
        print("access denied")
