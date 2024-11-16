import sys

orig_path = sys.argv[1]
cl_file = sys.argv[2]
align_file1 = sys.argv[3]
align_file2 = sys.argv[4]
split_num = int(sys.argv[5])
sample_num = sys.argv[6]

file_num = align_file1.split("_")[split_num]

align1_temp1 = align_file1.split(sample_num + "_")[0]
align1_temp2 = align_file1.split(sample_num + "_")[1]
align2_temp1 = align_file2.split(sample_num + "_")[0]
align2_temp2 = align_file2.split(sample_num + "_")[1]
    
# load original reads
f = open(orig_path, "r")
reads_temp = f.readlines()
f.close()

reads = []
for idx, val in enumerate(reads_temp):
    if idx % 2 == 1:
        reads.append(val)

print(len(reads))

# load clustered reads
f = open(cl_file, "r")
clusters = f.readlines()
f.close()

print(len(clusters))
for idx, val in enumerate(clusters):
    temp = val.split("\t")[2].split(",")
    seqs = []
    for seq_idx, seq_id in enumerate(temp):
        seqs.append(reads[int(seq_id) - 1])
    if len(seqs) != 1:
        f = open(align1_temp1 + file_num + "/" + sample_num + "_" + align1_temp2 + str(idx + 1) + ".fasta", "w")
        for i, seq in enumerate(seqs):
            f.write("> " + str(i + 1) + "\n")
            f.write(seq)
        f.close()
    elif len(seqs) == 1:
        f = open(align2_temp1 + file_num + "/" + sample_num + "_" + align2_temp2 + str(idx + 1) + ".fasta", "w")
        f.write("> 1\n")
        f.write(seqs[0])
        f.close()
print("\n")



