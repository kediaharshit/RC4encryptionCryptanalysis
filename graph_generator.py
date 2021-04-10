import matplotlib.pyplot as plt 
import numpy as np

table = {}
runs=25
for i in range(1,runs+1):
    fname = 'outputs/run' + str(i) +'.txt'
    file = open(fname, 'r')

    for line in file:
        vals = [float(x) for x in line.split()]
        c= int(vals[0])
        n= int(vals[1])
        r= float(vals[2])
        if((c,n) in table.keys()):
            table[c,n] += r
        else:
            table[c,n] = r

for key in table.keys():
    table[key] = table[key]/runs

# print(table)        
N = [2, 4, 8, 32, 128, 1024]

for n in N:
    y = []
    x = []
    for key in table.keys():
        if(n==key[1]):
            x.append(key[0])
            y.append(table[key])
    # print(x)
    # print(y)
    plt.plot(x, y)

plt.legend(('for N=2','for N=4','for N=8','for N=32','for N=128','for N=1024'), loc='upper right')
plt.xlabel('Number of bits flipped')
plt.ylabel('Randomness')
plt.show()
