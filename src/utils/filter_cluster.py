# Filtering by read length and Clustering same reads
# Author: Jiyeon Park
# Current version: Nov 5, 2024

import sys
from collections import Counter

def binary_search(arr, target):
    low, high = 0, len(arr) - 1
    while low <= high:
        mid = (low + high) // 2
        if arr[mid] > target:
            high = mid - 1
        if arr[mid] == target:
            return mid
        if arr[mid] < target:
            low = mid + 1
    return -1

# Hyperparameters
tiles = [1101, 1110, 1119, 2101, 2110, 2119]
seed_num = int(sys.argv[1])
sample_num = int(sys.argv[2])
trial_num = int(sys.argv[3])
use_NPF = int(sys.argv[4])
len_org = int(sys.argv[5])

# Load data
f = open("../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF_" + str(trial_num) + ".assembled.fastq", "r")
lines = f.readlines()
f.close()

# Filtering by length
len_filter = []
if use_NPF == 0: # use only PF
    # PF reads filtering
    for tile in tiles:
        f = open("../../ncl/s_1_" + str(tile) + "_PF.txt", "r")
        cl_list = f.readlines()
        f.close()
        cl_list = list(map(int, cl_list))

        for idx, val in enumerate(lines):
            if idx % 4 == 1:
                value = lines[idx-1].split(":")
                tile_num = int(value[2])
                org_cl = int(int(value[3]) * (int(value[3]) - 1) / 2 + int(value[4].split("/")[0])) 

                if binary_search(cl_list, org_cl) != -1 and len(val[:-1]) == 152:
                    len_filter.append(val)

elif use_NPF == 1: # use PF + NPF
    for idx, val in enumerate(lines):
        if idx % 4 == 1:
            if len(val[:-1]) == len_org:
                len_filter.append(val)
else:
    sys.exit(0)

# Clustering same read and sort by frequency
cnt_reads = Counter(len_filter)
reads = []
sizes = []

for key, val in cnt_reads.items():
    reads.append(key)
    sizes.append(val)

result = sorted(zip(reads, sizes), key = lambda x:x[1], reverse=True)
    
# Save file
filepath = ""
if use_NPF == 0:
    filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/PF/len/"
else:
    filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/len/"

f = open(filepath + "clustered_" + str(trial_num) + "_len152.txt", "w")
for idx, val in enumerate(result):
    f.write(val)
f.close()

