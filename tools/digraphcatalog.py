#!/usr/bin/python3

import re
import os

rx = re.compile('"(.*)":\s*"(.*)"');

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
with open("../build/digraphs.m", "w") as f:
    for n in range(N):
        h = len(sections[n]) * dy / 72 + .1
        f.write("qfigure('digraphs-%i', 1.2, %g)\n" % (n, h))
        f.write("qpen w\n")
        f.write("qplot([0 1],[0 -1])\n")
        f.write("qpen k\n")
        f.write("qat .25 0\n")
        # f.write("qalign left base\n")
        # f.write("qfont Helvetica bold 10\n")
        # f.write("qtext(0, 0, '%s')\n" % secheader[n])
        # y = 14
        # f.write("qfont Helvetica 10\n")
        y = 0
        seen = {}
        f.write("qfont FreeMono 10\n")
        f.write("qalign left base\n")
        for k_ in sections[n]:
            k = k_
            v = keymap[k]
            if not(v in seen) and not(' ' in k):
                seen[v] = 1
                while k[0] in revmap:
                    k = revmap[k[0]] + k[1:]
                k = k.replace("^", "∧")
                k = k.replace("~", "∼")
                k = k.replace("\\\\", "\\")
                if "'" in k:
                    f.write('qtext(10, %i, "%s")\n' % (y, k))
                else:
                    f.write("qtext(10, %i, '%s')\n" % (y, k))
                if n==1 and '∼∼' in k:
                    space[k_] = 1
                    y = y + 1
                y = y + dy
        y = 0
        seen = {}
        f.write("qfont NewCenturySchoolbook 10\n")
        f.write("qalign center base\n")
        for k in sections[n]:
            v = keymap[k]
            if not(v in seen) and not(' ' in k):
                seen[v] = 1
                f.write("qtext(-10, %i, '%s')\n" % (y, v))
                if k in space:
                    y = y + 1
                y = y + dy
        f.write("qshrink 2\n")
        f.write("qsave digraphs-%i.pdf\n" % n)

os.chdir("../build")
os.system("octave --eval digraphs")
os.system("placeqpt -o ../doc/digraphs.pdf ../tools/digraphs.placeqpt")

