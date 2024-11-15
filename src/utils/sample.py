import sys
import os
import numpy as np

# Hyperparameters
seed_num = int(sys.argv[1])
sample_num = int(sys.argv[2])
trial_num = int(sys.argv[3])
r1_filename = sys.argv[4]
r2_filename = sys.argv[5]

# Load fastq
r1 = [] ; r2 = []
f = open("../../dataset/" + r1_filename + ".fastq",'r')
r1 = f.readlines()
f.close()

f = open("../../dataset/" + r2_filename + ".fastq",'r')
r2 = f.readlines()
f.close()

# Random sampling
all_idx = [i for i in range(0, int(len(r1) / 4))]
np.random.seed(seed_num + trial_num)
idx = np.random.choice(all_idx, size=sample_num, replace=False) 

sample_r1 = []
sample_r2 = []

for i in idx:
    sample_r1.append([r1[i * 4], r1[i * 4 + 1], r1[i * 4 + 2], r1[i * 4 + 3]])
    sample_r2.append([r2[i * 4], r2[i * 4 + 1], r2[i * 4 + 2], r2[i * 4 + 3]])

savepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/"
os.makedirs(savepath, exist_ok=True)

f = open(savepath + "r1_extraNPF_" + str(trial_num) + ".fastq", "w")
for i in sample_r1:
    f.write(i[0])
    f.write(i[1])
    f.write(i[2])
    f.write(i[3])
f.close()

f = open(savepath + "r2_extraNPF_" + str(trial_num) + ".fastq", "w")
for i in sample_r2:
    f.write(i[0])
    f.write(i[1])
    f.write(i[2])
    f.write(i[3])
f.close()

