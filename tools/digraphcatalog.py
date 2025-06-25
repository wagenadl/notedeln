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

with open("../src/Items/Digraphs.json") as f:
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
space = {}

for n in range(N):
    h = len(sections[n]) * dy / 72 + .1
    qp.figure('digraphs', 1.2, h)
    qp.pen('w')
    qp.plot([0, 1], [0, -1])
    qp.pen()
    qp.at(.25, 0)
    y = 0
    seen = {}
    qp.font('FreeMono', 10)
    qp.align('left', 'base')
    for k_ in sections[n]:
        k = k_
        v = keymap[k]
        if (v in seen) or (' ' in k):
            continue
        seen[v] = 1
        while k[0] in revmap:
            k = revmap[k[0]] + k[1:]
        k = k.replace("^", "∧")
        k = k.replace("~", "∼")
        k = k.replace("\\\\", "\\")
        qp.text(k, 10, y)
        if n==1 and '∼∼' in k:
            space[k_] = 1
            y = y + 1
        y = y + dy
    y = 0
    seen = {}
    qp.font('CenturySchoolbook', 10)
    qp.align('center', 'base')
    for k in sections[n]:
        v = keymap[k]
        if (v in seen) or (' ' in k):
            continue
        seen[v] = 1
        qp.text(v, -10, y)
        if k in space:
            y = y + 1
        y = y + dy
    qp.shrink(2)
    qp.save(f"../build/digraphs-{n}.pdf")

os.chdir("../build")
os.system("placeqpt -o ../docs/userguide/digraphs.pdf ../tools/digraphs.placeqpt")

