# Proposed
# Author: Jiyeon Park
# Current version: Nov 5, 2024

#!/usr/bin/bash

set -e

# Hyperparameters
seed_num=$1
sample_num=$2
trial_num=$3
use_NPF=$4
len_org=$5

# Check arguments
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ]; then
    echo "Error: All five options must be provided."
    echo "Usage: $0 <seed_num> <sample_num> <trial_num> <use_NPF> <len_org>"
    exit 1
fi

# Run 
if [ ${use_NPF} -eq 0 ]; then
    echo "Use only PF reads"
    # Make directories
    mkdir -p ../result/${seed_num}/${sample_num}/PF/len 
    mkdir -p ../result/${seed_num}/${sample_num}/PF/RS_check 
    mkdir -p ../result/${seed_num}/${sample_num}/PF/LT_dec

elif [ ${use_NPF} -eq 1 ]; then
    echo "Use PF and NPF reads"
    # Make directories
    mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/len
    mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/RS_check
    mkdir -p ../result/${seed_num}/${sample_num}/extraNPF/LT_dec

else 
    echo "Error: Invalid option in <use_NPF>"
    exit 1
fi

python ./utils/filter_cluster.py ${seed_num} ${sample_num} ${trial_num} ${use_NPF} ${len_org}
python ./utils/RS_detect.py ${seed_num} ${sample_num} ${trial_num} ${use_NPF}
matlab -nodisplay -nosplash -nodesktop -r "cd('./src/utils/'); LT_decode(${seed_num},${sample_num},${trial_num},${use_NPF},0);exit;" | tail -n +11