#!/usr/bin/python3

import re
import os
import qplot as qp

rx = re.compile(r'"(.*)":\s*"(.*)"');

sections = []
keylist = []
keymap = {}
revmap = {}
secheader = []

with open("../src/Items/TeXCodes.json") as f:
    fr = f.read()
    for line in fr.split("\n"):
        m = rx.search(line)
        if m:
            k = m.group(1)
            v = m.group(2)
            if k == "#":
                sections.append(keylist)
                secheader.append(v)
                keylist = []
            else:
                keylist.append(k)
                keymap[k] = v
                revmap[v] = k

sections.append(keylist)
sections = sections[1:]

N = len(sections)
dy = 12
for n in range(N):
    h = len(sections[n]) * dy / 72 + .1
    if n==2:
        w = 1.3
    else:
        w = 1.2
    qp.figure('texcodes', w, h)
    qp.pen('w')
    qp.plot([0, 1],[0, -1])
    qp.pen()
    qp.at(.25, 0)
    y = 0
    qp.font('FreeMono', 10)
    qp.align('left', 'base')
    for k in sections[n]:
        while k[0] in revmap:
            k = revmap[k[0]] + k[1:]
        k = k.replace("^", "∧")
        if k[0]=='*':
            if len(k)==2:
                k = k[1]
            else:
                k = k[1:] + "^*"
        qp.text(k, 10, y)
        y = y + dy
    y = 0
    if n==6:
        qp.font('Georgia', 10, italic=True)
    else:
        qp.font('CenturySchoolbook', 10)
    qp.align('center', 'base')
    for k in sections[n]:
        v = keymap[k]
        qp.text(v, -5, y)
        y = y + dy
    qp.shrink(2)
    qp.save(f"../build/texcodes-{n}.pdf")

os.chdir("../build")
os.system("placeqpt -o ../docs/userguide/texcodes.pdf ../tools/texcodes.placeqpt")
