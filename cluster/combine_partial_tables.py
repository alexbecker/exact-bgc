import glob
import sys

#combine all files in the current directory with the name "S_n.i.j" into a single file "S_n_character_table"
def combine(n, i):
	out = open("S_{}_character_table".format(n), "w")

	fp = []
	for j in range(i):
		fp.append(open("S_{}.{}.{}".format(n, i, j), "r"))

	notDone = True
	while (notDone):
		for j in range(i):
			line = fp[j].readline()

			if not line:
				notDone = False
				break

			out.write(line)

if __name__ == "__main__":
	combine(int(sys.argv[1]), int(sys.argv[2]))
