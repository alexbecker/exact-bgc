# let lambda be a partition of k
# tests the conjecture that the multiplicity of V(lambda) in H^i stabilizes by i + k + 1
import sys

def test(maxi):
	# read in data
	partitions = [0] * (maxi + 1)
	for i in range(maxi + 1):
		partitions[i] = [0] * (3*maxi + 2)
		for n in range(i + 1,3*maxi + 2):
			partitions[i][n] = {}
			filename = "H^{}(PConf_{}(C)).txt".format(i, n)
			fp = open(filename, "r")
			for line in fp:
				partitions[i][n][line.split(":")[0]] = int(line.split(":")[1])
			fp.close()

	# check conjecture
	for i in range(maxi+1):
		for n in range(i + 1, 3*maxi + 1):
			for k in partitions[i][n]:
				x = i + sum(map(int,k[2:len(k)-1].split(","))) + 1
				if x < i:
					if partitions[i][x][k] != partitions[i][3*i + 1][k]:
						print((i, n, k))
						return False

	return True

if __name__ == "__main__":
	print(test(int(sys.argv[1])))
