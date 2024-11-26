import sys

# Hyperparameters
seed_num = sys.argv[1]
sample_num = sys.argv[2]
trial_num = sys.argv[3]
tau_e = sys.argv[4]
tau_adj = sys.argv[5]
stage = int(sys.argv[6])

reads = []
clusters = []
org_path = ""
cl_path = ""
savepath = ""
if stage == 2:
    org_path = "../result/" + seed_num + "/" + sample_num + "/extraNPF/RS_check/RSfail_" + trial_num + ".fasta"
    cl_path = "../result/" + seed_num + "/" + sample_num + "/extraNPF/prop/" + trial_num + "/edit" + tau_e + "/clustered.txt"
    savepath = "../result/" + seed_num + "/" + sample_num + "/extraNPF/prop/" + trial_num + "/edit" + tau_e + "/cluster/"
elif stage == 3:
    org_path = "../result/" + seed_num + "/" + sample_num + "/extraNPF/prop/" + trial_num + "/edit" + tau_e + "/RS_check/RSfail_withAL_" + trial_num + ".fasta"
    cl_path = "../result/" + seed_num + "/" + sample_num + "/extraNPF/prop/" + trial_num + "/edit" + tau_e + "/edit" + tau_adj + "/clustered.txt"
    savepath = "../result/" + seed_num + "/" + sample_num + "/extraNPF/prop/" + trial_num + "/edit" + tau_e + "/edit" + tau_adj + "/cluster/"
else:
    print("Invalid option in post_clustering")
    sys.exit(0)

# Load input reads of clustering
f = open(org_path, "r")
reads_temp = f.readlines()
f.close()

reads = []
for idx, val in enumerate(reads_temp):
    if idx % 2 == 1:
        reads.append(val)

# Load clustered reads
f = open(cl_path, "r")
clusters = f.readlines()
f.close()

cl_size1 = [] # Directly move to stage 3 

# Post-processing to clusters for alignment
for idx, val in enumerate(clusters):
    temp = val.split("\t")[2].split(",")
    seqs = []
    for seq_idx, seq_id in enumerate(temp):
        seqs.append(reads[int(seq_id) - 1])
    if len(seqs) != 1:
        f = open(savepath + str(idx + 1) + ".fasta", "w")
        for i, seq in enumerate(seqs):
            f.write("> " + str(i + 1) + "\n")
            f.write(seq)
        f.close()
    elif len(seqs) == 1:
        cl_size1.append(seqs[0])

f = open(savepath + "cl_size1.txt", "w")
for i in cl_size1:
    f.write(i)
f.close()



