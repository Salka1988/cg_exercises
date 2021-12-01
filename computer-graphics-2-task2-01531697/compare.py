import optparse
import sys
import numpy as np


class Values:
	def __init__(self):
		self.collisions = None
		self.step_list = []


class Step:
	def __init__(self):
		self.num = None
		self.normals = None
		self.verlet = None
		self.windacc = None
		self.fixup = None


def print_comparision(name: str, avg_dist, max_dist):
	print("{}: avg_dist={:10.6f}, max_dist={:10.6f}".format(name.ljust(10), avg_dist, max_dist))


def compare_array(array1: np.ndarray, array2: np.ndarray, name: str):
	distances = np.linalg.norm(array1 - array2, axis=1)
	avg_distance = np.average(distances)
	max_distance = np.max(distances)
	print_comparision(name, avg_distance, max_distance)
	return avg_distance, max_distance


def compare(values1: Values, values2: Values):
	max_dist_normals = max_dist_verlet = max_dist_windacc = max_dist_fixup = 0.
	avgs_dist_normal = []
	avgs_dist_verlet = []
	avgs_dist_windacc = []
	avgs_dist_fixup = []

	for stepv1, stepv2 in zip(values1.step_list, values2.step_list):
		print("-- Step {:2} {}".format(stepv1.num, "-"*40))
		avg_dist, max_dist = compare_array(np.abs(stepv1.normals), np.abs(stepv2.normals), "Normals")
		max_dist_normals = max(max_dist_normals, max_dist)
		avgs_dist_normal.append(avg_dist)

		avg_dist, max_dist = compare_array(stepv1.verlet, stepv2.verlet, "Verlet ")
		max_dist_verlet = max(max_dist_verlet, max_dist)
		avgs_dist_verlet.append(avg_dist)

		avg_dist, max_dist = compare_array(stepv1.windacc, stepv2.windacc, "WindAcc")
		max_dist_windacc = max(max_dist_windacc, max_dist)
		avgs_dist_windacc.append(avg_dist)

		avg_dist, max_dist = compare_array(stepv1.fixup, stepv2.fixup, "Fixup  ")
		max_dist_fixup = max(max_dist_fixup, max_dist)
		avgs_dist_fixup.append(avg_dist)

	print("\n-- Summary {}".format("-"*41))
	compare_array(values1.collisions, values2.collisions, "Collision")
	print_comparision("Normals", np.average(avgs_dist_normal), max_dist_normals)
	print_comparision("Verlet", np.average(avgs_dist_verlet), max_dist_verlet)
	print_comparision("WindAcc", np.average(avgs_dist_windacc), max_dist_windacc)
	print_comparision("Fixup", np.average(avgs_dist_fixup), max_dist_fixup)


def read_array(fin, channels):
	size_in_bytes = np.fromfile(fin, dtype=np.uint32, count=1)[0]
	assert size_in_bytes % np.dtype(np.float32).itemsize == 0
	data = np.fromfile(fin, dtype=np.float32, count=size_in_bytes // np.dtype(np.float32).itemsize).astype(np.float64)
	return np.reshape(data, [data.shape[0]//3, channels])


def read_input(path: str):
	values = Values()
	with open(path, 'rb') as fin:
		values.collisions = read_array(fin, 3)
		while True:
			step = Step()
			try:
				step.num = np.fromfile(fin, dtype=np.uint32, count=1)[0]
			except:
				break
			step.normals = read_array(fin, 3)
			step.verlet = read_array(fin, 3)
			step.windacc = read_array(fin, 3)
			step.fixup = read_array(fin, 3)
			values.step_list.append(step)
	return values


def main(argv):
	parser = optparse.OptionParser()
	parser.add_option('--in1', action="store", type=str, dest="path_input1", default="")
	parser.add_option('--in2', action="store", type=str, dest="path_input2", default="")
	options, args = parser.parse_args()

	if not options.path_input1:
		parser.error('no path for input 1 given')
	if not options.path_input2:
		parser.error('no path for input 2 given')

	values1 = read_input(options.path_input1)
	values2 = read_input(options.path_input2)
	compare(values1, values2)


if __name__ == '__main__':
	main(sys.argv[1:])
