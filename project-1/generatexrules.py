from itertools import chain

lines = []

# First quadrant
for k in range(1,4):
    for j in range(4,10):
        for i in range(1,10):
            lines.append(f"-{k}{k}{i} -{j}{j}{i} 0\n")

# Fifth quadrant (topleft -> bottom right)
for k in range(4,7):
    for j in chain(range(1,4), range(7,10)):
        for i in range(1,10):
            lines.append(f"-{k}{k}{i} -{j}{j}{i} 0\n")

# Nineth quadant
for k in range(7,10):
    for j in range(1,7):
        for i in range(1,10):
            lines.append(f"-{k}{k}{i} -{j}{j}{i} 0\n")

# Third quadrant
for k in range(1,4):
    for j in range(4,10):
        for i in range(1,10):
            lines.append(f"-{k}{10-k}{i} -{j}{10-j}{i} 0\n")

# Fifth quadrant (topright -> bottomleft)
for k in range(4,7):
    for j in chain(range(1,4), range(7,10)):
        for i in range(1,10):
            lines.append(f"-{k}{10-k}{i} -{j}{10-j}{i} 0\n")

# Seventh quadrant
for k in range(7,10):
    for j in range(1,7):
        for i in range(1,10):
            lines.append(f"-{k}{10-k}{i} -{j}{10-j}{i} 0\n")       

with open('x-sudoku-rules.txt', 'w') as file:
    file.write(f"p cnf 153 {len(lines)}\n")
    file.writelines(lines)
