# Random sampling and merging (with PEAR)
# Author: Jiyeon Park
# Current version: Nov 5, 2024

#!/usr/bin/bash

set -e

seed_num=$1
sample_num=$2
trial_num=$3
r1_filename=$4
r2_filename=$5

# Check arguments
if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
    echo "Error: All three options must be provided."
    echo "Usage: $0 <seed_num> <sample_num> <trial_num>"
    exit 1
fi

mkdir -p ../result/${seed_num}/${sample_num}/ # Make directory
python ./utils/sample.py ${seed_num} ${sample_num} ${trial_num} ${r1_filename} ${r2_filename} # Random sampling
python ./utils/PEAR/pear -f ../result/${seed_num}/${sample_num}/r1_extraNPF_${trial_num}.fastq -r ../result/${seed_num}/${sample_num}/r2_extraNPF_${trial_num}.fastq -o ../result/${seed_num}/${sample_num}/extraNPF_${trial_num} -j 30 # Merging

# Remove extra files after merging
rm -rf ../result/${seed_num}/${sample_num}/extraNPF_${trial_num}.discarded.fastq
rm -rf ../result/${seed_num}/${sample_num}/extraNPF_${trial_num}.unassembled.forward.fastq
rm -rf ../result/${seed_num}/${sample_num}/extraNPF_${trial_num}.unassembled.reverse.fastq








