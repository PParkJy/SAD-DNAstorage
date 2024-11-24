import sys, os
import numpy as np
import copy
import reedsolomon, fieldmath

## RS decoder parameters
bitPayload = 152 * 2
rs_n = 38
rs_k = 36
rs_parity = 8 * 2
field = fieldmath.BinaryField(285)
gene = 2
msglen = 36
ecclen = 2
rs = reedsolomon.ReedSolomon(field, gene, msglen, ecclen)

## Functions
def combination(arr, n):
    result = []

    if n == 0:
        return [[]]

    for i in range(len(arr)):
        elem = arr[i]
        for rest in combination(arr[i + 1:], n - 1):
            result.append([elem] + rest)

    return result

def BinToDec(binary):
    return sum(val*(2**idx) for idx, val in enumerate(binary))

def DecToBit(deci, nbit):
    temp_bin = bin(deci).replace("0b", "")
    npad = nbit - len(temp_bin)
    if npad < 0:
        print("decimal to bit error")
        sys.exit(0)
    temp_pad = ""
    for i in range(0, npad):
        temp_pad += "0"
    temp_pad += temp_bin
    real_bin = list(map(int, list(temp_pad)))

    return real_bin

def ACGTtoBit(seq):
    bits = []
    for i in seq:
        if i == 'A':
            bits.append(0)
            bits.append(0)
        elif i == 'C':
            bits.append(0)
            bits.append(1)
        elif i == 'G':
            bits.append(1)
            bits.append(0)
        elif i == 'T':
            bits.append(1)
            bits.append(1)
        elif i == '-': # null == 2
            bits.append(2)
            bits.append(2)
    return bits

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

def RS_dec(seq):
    rs_seq = []    
    for i in range(0, len(seq), 8):
        rs_temp = seq[i:i+8]
        rs_seq.append(BinToDec(rs_temp[::-1]))

    decoded = rs.decode(rs_seq,0)
    
    return decoded

## Hyperparameters
seed_num = sys.argv[1]
sample_num = sys.argv[2]
trial_num = sys.argv[3]
tau_e = sys.argv[4]
tau_adj = sys.argv[5]
pathdir = sys.argv[6]
stage = int(sys.argv[7])

seedNum = 951004
np.random.seed(seedNum) 

fileList = os.listdir(pathdir)

result = []
fails = []

for filename in fileList:
    chk_candi = 0
    align_result = []

    f = open(pathdir + filename, "r")
    lines = f.readlines()
    f.close()

    # Consensus process
    if len(lines) > 2: # consensus is condeucted when cluster size > 1
        temp = []
        temp_fail = []
        temp_seq = ""
        cl_size = 0
        
        for idx, val in enumerate(lines):
            if val[0] == '>':
                cl_size += 1
                if idx != 0:
                    temp.append(ACGTtoBit(temp_seq))
                    temp_fail.append(temp_seq.replace("-", ""))
                temp_seq = ""
            elif val[0] != '>':
                temp_seq += val[:-1]
            
        temp.append(ACGTtoBit(temp_seq))
        temp_fail.append(temp_seq.replace("-", ""))
        temp = list(map(list, zip(*temp)))

        candidates = []
        prob_candi = []
        del_candi = []

        # Save candidates
        # Alignment length == original length
        if len(temp) == bitPayload:
            for idx, val in enumerate(temp):
                # ignore the null
                zero_cnt = val.count(0)
                one_cnt = val.count(1)

                if zero_cnt > 0 and one_cnt > 0:
                    prob_zero = float(zero_cnt) / (zero_cnt + one_cnt)
                    prob_one = float(one_cnt) / (zero_cnt + one_cnt)
                    prob_candi.append([prob_zero, prob_one, idx])
                    align_result.append(-1)
                elif zero_cnt == 0:
                    align_result.append(1)
                elif one_cnt == 0:
                    align_result.append(0)

            len_candi = pow(2, len(prob_candi))

            if len_candi == 1:
                if RS_dec(align_result) == 0:
                    chk_candi = 1
                    result.append([cl_size, align_result])
            else:                 
                for i in range(0, len_candi):
                    temp_candi = DecToBit(i, len(prob_candi))
                    temp_prob = 1
                    for idx, val in enumerate(temp_candi):
                        if val == 0:
                            temp_prob *= prob_candi[idx][0]
                        elif val == 1:
                            temp_prob *= prob_candi[idx][1]
                    candidates.append([temp_prob, temp_candi]) 
                        
                candidates.sort(key=lambda x : -x[0])
                chk_candi = 0

                for i in candidates:
                    for idx, val in enumerate(i[1]):
                        align_result[prob_candi[idx][2]] = val

                    # RS check pass
                    if RS_dec(align_result) == 0:
                        chk_candi = 1
                        result.append([cl_size, align_result])
                        break
                
                if chk_candi != 1:
                    for k in temp_fail:
                        fails.append(k)   

        # align length > original length -> consider null
        elif len(temp) > bitPayload:
            alldel = 0
            for idx, val in enumerate(temp):
                # ignore the deletion
                zero_cnt = val.count(0)
                one_cnt = val.count(1)
                del_cnt = val.count(2)
                
                if zero_cnt == 0 and one_cnt == 0: # only del #sibal...fix this...
                    align_result.append(2)
                    alldel += 1
                elif one_cnt == 0 and del_cnt == 0: # only 1
                    align_result.append(0)
                elif zero_cnt == 0 and del_cnt == 0: # only 0
                    align_result.append(1)
                else:
                    if del_cnt > 0 and idx % 2 == 0:
                        prob_del = float(del_cnt) / (zero_cnt + one_cnt + del_cnt)
                        del_candi.append([prob_del, idx])
                    
                    prob_zero = float(zero_cnt) / (zero_cnt + one_cnt)
                    prob_one = float(one_cnt) / (zero_cnt + one_cnt)    
                    prob_candi.append([prob_zero, prob_one, idx])
                    align_result.append(-1)

            if len(del_candi) == 0:
                for k in temp_fail:
                    fails.append(k)
                #break

            comb_del = combination([x for x in range(len(del_candi))], (len(temp) - bitPayload) / 2 - alldel)
            comb_delProb = []

            for i in comb_del:
                many_delProb = 1
                many_delIdx = []
                for j in i:
                    many_delProb *= del_candi[j][0]
                    many_delIdx.append(del_candi[j][1])
                comb_delProb.append([many_delProb, many_delIdx])
                
            comb_delProb.sort(key=lambda x : -x[0])
            
            for i in comb_delProb:
                candidates = []
                chk_candi = 0
                temp_align = copy.deepcopy(align_result)
                for j in i[1]:
                    temp_align[j] = 2
                    temp_align[j+1] = 2 

                temp_probCandi = []
                test2 = []
                
                for j in prob_candi:
                    if j[2] not in i[1]:
                        if j[2] - 1 not in i[1]:    
                            
                            if j[0] > 0 and j[1] > 0:
                                temp_probCandi.append(j)
                                test2.append(j)

                            if j[0] == 0:
                               temp_align[j[2]] = 1
                            elif j[1] == 0:
                               temp_align[j[2]] = 0

                len_candi = pow(2, len(temp_probCandi))

                if len(temp_probCandi) == 0:
                    temp_alignList = []
                    for k in temp_align:
                        if k != 2:
                            temp_alignList.append(k)
                    
                    if RS_dec(temp_alignList) == 0:
                        chk_candi = 1
                        result.append([cl_size, temp_alignList])
                        break
                    else:       
                        for k in temp_fail:
                            fails.append(k)

                else:
                    for j in range(0, len_candi):
                        if len(temp_probCandi) == 0:
                            print(len_candi)
                        temp_candi = DecToBit(j, len(temp_probCandi))
                        temp_prob = 1 
                        for idx, val in enumerate(temp_candi):
                            if val == 0:
                                temp_prob *= temp_probCandi[idx][0]
                            elif val == 1:
                                temp_prob *= temp_probCandi[idx][1]
                        candidates.append([temp_prob, temp_candi])

                    candidates.sort(key=lambda x : -x[0])
                
                    for j in candidates:
                        for idx, val in enumerate(j[1]):
                            temp_align[temp_probCandi[idx][2]] = val
                        
                        temp_alignList = []
                        for k in temp_align:
                            if k != 2:
                                temp_alignList.append(k)
                        
                        if RS_dec(temp_alignList) == 0:
                            chk_candi = 1
                            result.append([cl_size, temp_alignList])
                            break
                    
                if chk_candi == 1:
                    break
            if chk_candi != 1:
                for k in temp_fail:
                    fails.append(k)

    else: # cluster size == 1 -> cannot try the consensus 
        fails.append(lines[1][:-1])

# Save results
if stage == 2:
    orig_file = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/RS_check/errfree_" + str(trial_num) + "_S1.txt"

    f = open(orig_file, "r")
    origs = f.readlines()
    f.close()

    real_result = []

    for i in result:
        if BitToACGT(i[1]) + "\n" not in origs:   
            real_result.append(BitToACGT(i[1]))

    real_result = list(set(real_result))
    savepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/prop/" + str(trial_num) + "/edit" + str(tau_e) + "/RS_check/"

    f = open(savepath + "RSfail_S2.fasta", "w")
    for i in fails:
        f.write(">\n")
        f.write(i + "\n")
    f.close()

    f = open(savepath + "errfree_S2.txt", "w")
    for i in real_result:
        f.write(i + "\n")    
    f.close()

elif stage == 3:
    origs = []

    orig_file1 = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/RS_check/errfree_" + str(trial_num) + "_S1.txt"
    f = open(orig_file1, "r")
    origs.extend(f.readlines())
    f.close()

    orig_file2 = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/prop/" + str(trial_num) + "/edit" + str(tau_e) + "/RS_check/errfree_S2.txt"
    f = open(orig_file2, "r")
    origs.extend(f.readlines())
    f.close()

    real_result = []

    for i in result:
        if BitToACGT(i[1]) + "\n" not in origs:   
            real_result.append(BitToACGT(i[1]))

    real_result = list(set(real_result))
    savepath = "../../result/" + str(seed_num) + "/" + str(sample_num) + "/extraNPF/prop/" + str(trial_num) + "/edit" + str(tau_e) + "/edit" + str(tau_adj) + "/RS_check/"

    f = open(savepath + "RSfail_S3.fasta", "w")
    for i in fails:
        f.write(">\n")
        f.write(i + "\n")
    f.close()

    f = open(savepath + "errfree_S3.txt", "w")
    for i in real_result:
        f.write(i + "\n")    
    f.close()

    f = open(savepath+ "errfree_prop.txt", "w")
    for i in origs:
        f.write(i)
    for i in real_result:
        f.write(i + "\n")    
    f.close()

else:
    print("Invalid stage: consensus.py")   
    sys.exit(0)

