import re
import matplotlib.pyplot as plt

f = open('table.txt')
lines = f.readlines()
header = lines[0]
lines = lines[1:]

lines = [re.split(r'[ :\[\]]+', line.strip()) for line in lines if line != '\n']

for i in range(len(lines)):
    lines[i] = list(map(float, lines[i][1:-1]))

M = list(zip(*lines))

perc = ['40%', '50%', '60%', '70%', '80%', '90%']

for line, p in zip(M, perc):
    plt.plot(range(1, 21), line, label=p)

plt.ylabel('ubrzanje')
plt.xlabel('k')
plt.plot(range(1, 21), 20 * [3], 'r--', label='3x ubrzanje')
plt.ylim(ymin=2)
plt.legend()

plt.savefig('speedplot.png')

