#!/usr/bin/python3

import glob
import os

cc = glob.glob("src/*/*.cpp")
hh = glob.glob("src/*/*.h")

for fn in cc + hh:
    print(fn)
    lines = []
    with open(fn, "r") as fd:
        for line in fd.readlines():
            line = line.replace('part of eln', 'part of NotedELN')
            line = line.replace('part of ELN', 'part of NotedELN')
            line = line.replace('eln is free software', 'NotedELN is free software')
            line = line.replace('eln is distributed', 'NotedELN is distributed')
            line = line.replace('along with eln', 'along with NotedELN')
            lines.append(line)
    with open(fn, "w") as fd:
        fd.write("".join(lines))
