# Proposed sequence analysis
# Author: Jiyeon Park
# Current version: Apr. 2, 2025

#!/usr/bin/bash

set -e

# Hyperparameters
seed_num=$1
sample_num=$2
trial_num=$3
tau_e=$4
tau_sub=$5
tau_del=$6
tau_ins=$7
tau_adj=$8
len_org=$9
len_min=${10}
len_max=${11}

# Check arguments
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ] || [ -z "$7" ] || [ -z "$8" ] || [ -z "$9" ] || [ -z "$10" ] || [ -z "$11" ] ; then
    echo "Error: All eleven options must be provided."
    echo "Usage: $0 <seed_num> <sample_num> <trial_num> <tau_e> <tau_sub> <tau_del> <tau_ins> <tau_adj> <len_org> <len_min> <len_max>"
    echo "If you do not want to adjust the substitution/deletion/insertion thresholds in the tailored edit-distance based clustering, set <tau_sub>, <tau_del>, <tau_ins> to 0, and assign a value only to <tau_adj>."
    echo "If you want to adjust the substitution/deletion/insertion thresholds, values should be assigned to <tau_sub>, <tau_del>, <tau_ins>, while <tau_adj> should be set to 0."
    exit 1
fi

tau_total=$((tau_sub + tau_del + tau_ins))

if [ ${tau_adj} -eq 0 ] && [ ${tau_total} -eq 0 ]; then
    echo "Error: All thresholds of tailored edit-distance based clustering are 0."
    exit 1
fi

if [ ${tau_adj} -gt 0 ] && [ ${tau_total} -gt 0 ]; then
    echo "Error: Only one set of parameters should be used: either <tau_adj> or (<tau_sub>, <tau_del>, <tau_ins>). Mixing them is not allowed."
    exit 1
fi

tau_adj=${tau_total}
diff1=$((len_max - len_org))
diff2=$((len_org - len_min))

if [ "$diff1" -gt "$diff2" ]; then
    bias=$diff1
else
    bias=$diff2
fi

# Run 
echo "Use PF and NPF reads"
# Make directories
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/cluster
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/align
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/RS_check
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/LT_dec
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/cluster
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/align
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/RS_check
mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/LT_dec

# Stage 2 ===========================================================================================
./utils/starcode/starcode -d ${tau_e} -t 30 -s --seq-id -i ../result/${seed_num}/${sample_num}/extraNPF/RS_check/RSfail_${trial_num}.fasta -o ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/clustered.txt
python ./utils/post_clustering.py ${seed_num} ${sample_num} ${trial_num} ${tau_e} ${tau_adj} 2 
for eachfile in `find ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/cluster/ -maxdepth 1 -name '*.fasta'`
do
    f_name=`basename "$eachfile"`
    ./utils/MUSCLE/muscle_v5.0.1428_linux -align $eachfile -output ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/align/${f_name} -threads 30 &
done 
python ./utils/consensus.py ${seed_num} ${sample_num} ${trial_num} ${tau_e} ${tau_adj} ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/align/ 2

# Stage 3 ===========================================================================================
python ./utils/al_filter.py ${seed_num} ${sample_num} ${trial_num} ${tau_e} ${tau_adj} ${len_org} ${len_min} ${len_max}
./utils/tail_edit/tailored_clustering -k ${bias} -l ${len_org} -t 30 -x ${tau_sub} -y ${tau_del} -z ${tau_ins} -d ${tau_adj} -j --seq-id -i ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/RS_check/RSfail_withAL_${trial_num}.fasta -o ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/clustered.txt
python ./utils/post_clustering.py ${seed_num} ${sample_num} ${trial_num} ${tau_e} ${tau_adj} 3
for eachfile in `find ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/cluster/ -maxdepth 1 -name '*.fasta'`
do
    f_name=`basename "$eachfile"`
    ./utils/MUSCLE/muscle_v5.0.1428_linux -align $eachfile -output ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/align/${f_name} -threads 30 &
done 
python ./utils/consensus.py ${seed_num} ${sample_num} ${trial_num} ${tau_e} ${tau_adj} ../result/${seed_num}/${sample_num}/extraNPF/prop/${trial_num}/edit${tau_e}/edit${tau_adj}/align/ 3

# LT erasure decoding
~/matlab -nodisplay -nosplash -nodesktop -r "cd('./utils/'); LT_decode_prop(${seed_num},${sample_num},${trial_num},${tau_e},${tau_adj});exit;" | tail -n +11
