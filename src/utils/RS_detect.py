from errno import EBADE
import sys
import reedsolomon, fieldmath
from collections import Counter

# Parameters
bitPayload = 152 * 2
rs_n = 38
rs_k = 36
rs_parity = 8 * 2

# RS decoder parameters
field = fieldmath.BinaryField(285)
gene = 2
msglen = 36
ecclen = 2
rs = reedsolomon.ReedSolomon(field, gene, msglen, ecclen)

def BitToACGT(bits):
    seq = ""
    for i in range(0, len(bits), 2):
        if bits[i] == 0 and bits[i+1] == 0:
            seq += 'A'
        elif bits[i] == 0 and bits[i+1] == 1:
            seq += 'C'
        elif bits[i] == 1 and bits[i+1] == 0:
            seq += 'G'
        elif bits[i] == 1 and bits[i+1] == 1:
            seq += 'T'
    return seq

def BinToDec(binary):
    return sum(val*(2**idx) for idx, val in enumerate(binary))

def RS_dec(seq):
    rs_seq = []    
    for i in range(0, len(seq), 8):
        rs_temp = seq[i:i+8]
        rs_seq.append(BinToDec(rs_temp[::-1]))
    decoded = rs.decode(rs_seq,0)
    return decoded

# Hyperparameters
seed_num = int(sys.argv[1])
sample_num = int(sys.argv[2])
trial_num = int(sys.argv[3])
use_NPF = int(sys.argv[4])

# Load data
filepath = ""
if use_NPF == 0:
    filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/PF/len/clustered_" + str(trial_num) + "_len152.txt"
else:
    filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/len/clustered_" + str(trial_num) + "_len152.txt"

f = open(filepath, "r")
reads = f.readlines()
f.close()

# Sequence to bit
result = []
for i in reads:
    bits = []
    for j in i[0][:-1]: #for j in i:
        if j == 'A':
            bits.append(0)
            bits.append(0)
        elif j == 'C':
            bits.append(0)
            bits.append(1) 
        elif j == 'G':
            bits.append(1)
            bits.append(0)
        elif j == 'T':
            bits.append(1)
            bits.append(1)
    result.append(bits)

# RS code decode
rs_fail = []
rs_pass = []

for idx, val in enumerate(result):
    if RS_dec(val) != 0:
        rs_fail.append(BitToACGT(val))
    else:
        rs_pass.append(BitToACGT(val))

# Save file
save_filepath = ""

if use_NPF == 0:
    save_filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/PF/RS_check/"
elif use_NPF == 1:
    save_filepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/RS_check/"
else:
    sys.exit(0)

f = open(save_filepath + "RSfail_" + str(trial_num) + ".fasta", "w")
for i in rs_fail:
    f.write(">\n")
    f.write(i + "\n")
f.close()

f = open(save_filepath + "errfree_" + str(trial_num) + "_S1.txt", "w")
for i in rs_pass:
    f.write(i + "\n")
f.close()