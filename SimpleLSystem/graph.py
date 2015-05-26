import numpy as np
import matplotlib.pyplot as plt

data = []
f = open('density_effect.txt', 'r')
for line in f:
	rec = []
	for v in line.split(' '):
		rec.append(v)
	data.append(rec)
data = np.array(data);
f.close()

print data

plt.figure(figsize=(20, 10))
for r in range(10):
	for c in range(10):
		plt.subplot(10, 10, r * 10 + c + 1)
		#plt.xlabel('angle')
		#plt.ylabel('#branches')
		plt.plot(data[:,0], data[:, (9 - r) * 10 + c + 1])

plt.savefig('density_effect.png')
plt.show()