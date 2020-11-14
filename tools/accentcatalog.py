#!/usr/bin/python3

import re
import os

rx = re.compile('"(.*)":\s*"(.*)"');

sections = []
keylist = []
keymap = {}
revmap = {}
secheader = []

with open("../src/Items/Accents.json") as f:
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
with open("../build/accents.m", "w") as f:
    for n in range(N):
        h = len(sections[n]) * dy / 72 + .1
        if n==2:
            w = 1.3
        else:
            w = 1.2
        f.write("qfigure('accents-%i', %g, %g)\n" % (n, w, h))
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
        f.write("qfont FreeMono 10\n")
        f.write("qalign left base\n")
        for k in sections[n]:
            while k[0] in revmap:
                k = revmap[k[0]] + k[1:]
            k = k.replace("^", "^\^ ")
            k = k.replace("_", "\_")
            k = k.replace("~", "˜")
            #k = '\\\\' + k
            if "'" in k:
                f.write('qtext(10, %i, "%s")\n' % (y, k))
            else:
                f.write("qtext(10, %i, '%s')\n" % (y, k))
            y = y + dy
        y = 0
        f.write("qfont NewCenturySchoolbook 10\n")
        f.write("qalign center base\n")
        for k in sections[n]:
            v = keymap[k]
            f.write("qtext(-5, %i, '%s')\n" % (y, v))
            y = y + dy
        f.write("qshrink 2\n")
        f.write("qsave accents-%i.pdf\n" % n)

os.chdir("../build")
os.system("octave --eval accents")
os.system("placeqpt -o ../doc/accents.pdf ../tools/accents.placeqpt")
