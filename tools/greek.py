#!/usr/bin/python3

alphabet = "alpha beta gamma delta epsilon zeta eta theta iota kappa lambda mu nu xi omicron pi rho sigma tau upsilon phi chi psi omega".split(" ")

bold = False
if bold:
    base = 0x1d736
else:
    base = 0x1d6fc

dk = 0
for k, a in enumerate(alphabet):
    if a=='sigma':
        dk += 1
    print(f'''  "{a}": "''' + chr(base + k + dk) + '''",''')

print()

math = True
if math:
    base = 0x1d6a8
else:
    base = 0x391
dk = 0
for k, a in enumerate(alphabet):
    if a=='sigma':
        dk += 1
    b = a[:1].upper() + a[1:]
    print(f'''  "{b}": "''' + chr(base + k + dk) + '''",''')
