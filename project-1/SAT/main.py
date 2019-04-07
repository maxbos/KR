import sys

def print_sudoku(true_vars):
    """
    Print sudoku.
    :param true_vars: List of variables that your system assigned as true. Each var should be in the form of integers.
    :return:
    """
    if len(true_vars) != 81:
        print("Wrong number of variables.")
        return
    s = []
    row = []
    for i in range(len(true_vars)):
        row.append(str(int(true_vars[i]) % 10))
        if (i+1) % 9 == 0:
            s.append(row)
            row = []

    print("╔═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╦═" + "═" + "═╗")
    print("║ "+s[0][0]+" | "+s[0][1]+" | "+s[0][2]+" ║ "+s[0][3]+" | "+s[0][4]+" | "+s[0][5]+" ║ "+s[0][6]+" | "+s[0][7]+" | "+s[0][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[1][0]+" | "+s[1][1]+" | "+s[1][2]+" ║ "+s[1][3]+" | "+s[1][4]+" | "+s[1][5]+" ║ "+s[1][6]+" | "+s[1][7]+" | "+s[1][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[2][0]+" | "+s[2][1]+" | "+s[2][2]+" ║ "+s[2][3]+" | "+s[2][4]+" | "+s[2][5]+" ║ "+s[2][6]+" | "+s[2][7]+" | "+s[2][8]+" ║")
    print("╠═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╣")
    print("║ "+s[3][0]+" | "+s[3][1]+" | "+s[3][2]+" ║ "+s[3][3]+" | "+s[3][4]+" | "+s[3][5]+" ║ "+s[3][6]+" | "+s[3][7]+" | "+s[3][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[4][0]+" | "+s[4][1]+" | "+s[4][2]+" ║ "+s[4][3]+" | "+s[4][4]+" | "+s[4][5]+" ║ "+s[4][6]+" | "+s[4][7]+" | "+s[4][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[5][0]+" | "+s[5][1]+" | "+s[5][2]+" ║ "+s[5][3]+" | "+s[5][4]+" | "+s[5][5]+" ║ "+s[5][6]+" | "+s[5][7]+" | "+s[5][8]+" ║")
    print("╠═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╬═" + "═" + "═╣")
    print("║ "+s[6][0]+" | "+s[6][1]+" | "+s[6][2]+" ║ "+s[6][3]+" | "+s[6][4]+" | "+s[6][5]+" ║ "+s[6][6]+" | "+s[6][7]+" | "+s[6][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[7][0]+" | "+s[7][1]+" | "+s[7][2]+" ║ "+s[7][3]+" | "+s[7][4]+" | "+s[7][5]+" ║ "+s[7][6]+" | "+s[7][7]+" | "+s[7][8]+" ║")
    print("╠─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╬─" + "─" + "─┼─" + "─" + "─┼─" + "─" + "─╣")
    print("║ "+s[8][0]+" | "+s[8][1]+" | "+s[8][2]+" ║ "+s[8][3]+" | "+s[8][4]+" | "+s[8][5]+" ║ "+s[8][6]+" | "+s[8][7]+" | "+s[8][8]+" ║")
    print("╚═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╩═" + "═" + "═╝")

def check_sudoku(true_vars):
    """
    Check sudoku.
    :param true_vars: List of variables that your system assigned as true. Each var should be in the form of integers.
    :return:
    """
    import math as m
    s = []
    row = []
    for i in range(len(true_vars)):
        row.append(str(int(true_vars[i]) % 10))
        if (i + 1) % 9 == 0:
            s.append(row)
            row = []

    correct = True
    for i in range(len(s)):
        for j in range(len(s[0])):
            for x in range(len(s)):
                if i != x and s[i][j] == s[x][j]:
                    correct = False
                    print("Repeated value in column:", j)
            for y in range(len(s[0])):
                if j != y and s[i][j] == s[i][y]:
                    correct = False
                    print("Repeated value in row:", i)
            top_left_x = int(i-i%m.sqrt(len(s)))
            top_left_y = int(j-j%m.sqrt(len(s)))
            for x in range(top_left_x, top_left_x + int(m.sqrt(len(s)))):
                for y in range(top_left_y, top_left_y + int(m.sqrt(len(s)))):
                    if i != x and j != y and s[i][j] == s[x][y]:
                        correct = False
                        print("Repeated value in cell:", (top_left_x, top_left_y))
    return correct

if __name__ == "__main__":
    sudoku = sys.argv[1:]
    print_sudoku(sudoku)
    correct = check_sudoku(sudoku)
    print('The sudoku is', (correct and 'correct') or (not correct and 'incorrect'));
