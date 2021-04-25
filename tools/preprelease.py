#!/usr/bin/python3

import os
import sys
import subprocess

myname = sys.argv[0].replace('\\', '/').split('/')[-1]


def usage():
    print(f'Usage: {myname} [path]', file=sys.stderr)
    sys.exit(1)

def backtick(cmd):
    try:
        res = subprocess.check_output(cmd.split(" "))
    except CalledProcessError as e:
        return None
    return str(res, 'utf8').strip()
    
def getbasicinfo():
    name = backtick("git config user.name")
    if name is None:
        raise Exception("No user name configured in git")

    email = backtick("git config user.email")
    if name is None:
        raise Exception("No email configured in git")

    status = backtick("git status --porcelain")
    if status is None:
        raise Exception("Git status failed. Do you have a repository?")
    elif status != "":
        raise Exception("You have uncommitted changes")

    if os.system("git fetch"):
        raise Exception("Could not run git fetch")
    if backtick("git rev-list HEAD..@{upstream}") != "":
        raise Exception("Your branch is behind upstream")
    
    
        
    
    
if len(sys.argv)>=2:
    if sys.argv[1].startswith('-') or len(sys.argv)>=3:
        usage()
    else:
        os.chdir(sys.argv[1])
    
isrepeat = "reprep" in myname

info = getbasicinfo()
