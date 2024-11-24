# Filtering by read length and Save AL reads
# Author: Jiyeon Park
# Current version: Nov 5, 2024

import sys

# Hyperparameters
seed_num = sys.argv[1]
sample_num = sys.argv[2]
trial_num = sys.argv[3]
tau_e = sys.argv[4]
tau_adj = sys.argv[5]
len_org = int(sys.argv[6])
len_min = int(sys.argv[7])
len_max = int(sys.argv[8])

# Load data
f = open("../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF_" + str(trial_num) + ".assembled.fastq", "r")
lines = f.readlines()
f.close()

# Select AL reads
len_filter = []
for idx, val in enumerate(lines):
    if idx % 4 == 1:
        if len(val[:-1]) >= len_min and len(val[:-1]) <= len_max and len(val[:-1]) != len_org:
            len_filter.append(val)

result = list(set(len_filter))

# Save results
filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/len/"
f = open(filepath + "clustered_" + str(trial_num) + "_len" + str(len_min) + str(len_max) + ".txt", "w")
for idx, val in enumerate(result):
    f.write(val)
f.close()

f = open("../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/prop/" + str(trial_num) + "/edit" + str(tau_e) + "/RS_check/RSfail_S2.fasta", "r")
fails = f.readlines()
f.close()

f = open("../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/prop/" + str(trial_num) + "/edit" + str(tau_e) + "/RS_check/RSfail_withAL_" + trial_num + ".fasta", "w")
for i in fails:
    f.write(i)
for i in result:
    f.write(">\n")
    f.write(i)
f.close()
