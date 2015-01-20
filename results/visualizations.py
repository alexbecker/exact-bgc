# visualize the results in various ways
import sys
import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

def plotall(maxi):

	# read data
	distinct = np.zeros(maxi+1)
	total = np.zeros(maxi+1)
	sums_distinct = np.zeros((maxi+1, 2*maxi+1))
	sums_total = np.zeros((maxi+1, 2*maxi+1))
	for i in range(maxi+1):
		fp = open("H^{}(PConf_n(C)).txt".format(i), "r")
		for line in fp:
			partition = line.split(":")[0]
			count = int(line.split(":")[1])

			distinct[i] += 1
			total[i] += count

			sums_distinct[i, sum(map(int, partition[2:len(partition)-1].split(",")))] += 1
			sums_total[i, sum(map(int, partition[2:len(partition)-1].split(",")))] += count

		fp.close()

	plt.plot(range(maxi+1), np.log(distinct), 'ro', np.log(total), 'b^')
	plt.legend(["distinct", "total"])
	plt.xlabel("i")
	plt.ylabel("ln(#Irreps)")
	plt.title("Irrep counts")
	plt.show()

	for i in range(2,maxi+1):
		# normal fit log of sums_total
		p = np.polyfit(range(1,2*i+1), np.log(sums_total[i,1:2*i+1]), 2)
		fit = np.poly1d(p)(np.linspace(1, 2*i, 100))

		plt.plot(range(1, 2*i+1), sums_total[i,1:2*i+1], 'b^', np.linspace(1, 2*i, 100), np.exp(fit), 'b')
		plt.legend(["total", "normal fit"])
		plt.xlabel("Sum")
		plt.ylabel("ln(#Irreps)")
		plt.title("Irrep sum distribution for i={}".format(i))
		plt.show()

if __name__ == "__main__":
	plotall(int(sys.argv[1]))
