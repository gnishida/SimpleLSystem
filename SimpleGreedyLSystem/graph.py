import numpy as np
import matplotlib.pyplot as plt

data = []
f = open('results.txt', 'r')
for line in f:
	rec = []
	for v in line.split(' '):
		rec.append(v)
	data.append(rec)
data = np.array(data);
f.close()

print data

plt.plot(data[:,0], data[:,1], 'o-', label="greedy")
plt.plot(data[:,0], data[:,2], '-', label="baseline")
		#plt.xlabel('angle')
plt.xlabel('# hidden rows of indicator')
plt.ylabel('RMSE')
plt.legend(loc='lower right')
plt.savefig('results.png')
plt.show()