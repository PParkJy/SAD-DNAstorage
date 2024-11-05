# Sequence analysis and decoding with extra low-quality reads for DNA data storage
This repository is for the study **"Sequence analysis and decoding with extra low-quality reads for DNA data storage"** which submitted to **_Bioinformatics_** in 2024.  
Here, we provide the source code and sequencing data.  
(Current version was revised in Nov. 5, 2024)  

## Dataset
We use pass filter (PF) reads and non-pass filter (NPF) reads of Illumina NGS sequencing.  
- PF: pass the chastity filter with an identified index pattern  
- NPF: fail to pass the filter  

NPF reads are not provided as FASTQ files in Illumina NGS sequencing.  
Therefore, we obtained raw sequencing data from Illumina sequencer and performed base-calling on NPF reads from the raw data.  
The detailed sequencing informations are described in "supplementary.docx".  
Based on MiSeq [configurations](https://support.illumina.com/downloads/miseq-product-documentation.html), we obtained the following raw sequencing data: cif, filter, and locs files.    

### Raw data
- *.cif (./dataset/raw/cif/)
- *.filter (./dataset/raw/filter/)
- *.locs (./dataset/raw/locs/)
  
  ![raw_format](./img/raw_format.png)

### FASTQ 
The provided FASTQ include all sequening reads: PF (pass filter), NPF, and reads with an invalid index.   
- AYB-basecalled FASTQ (./dataset/fastq/AYB_fastq/)
- Illumina-basecalled FASTQ (./dataset/fastq/Illumina_fastq/)

## Sequence analysis and decoding (To be updated...)
Our sequence analysis workflow is shown in below figure.  

  ![workflow](./img/workflow.png)

### Requirements
#### Languages
- Python  
- Matlab  
- C  

#### Open-source Software (used in proposed workflow and decoding)
- Edit distance based-clustering **[Starcode](https://github.com/gui11aume/starcode)**
- Sequence alignment **[MUSCLE](https://github.com/rcedgar/muscle)** (version 5.0.1428)
- Paired-end read merging **[PEAR](https://github.com/tseemann/PEAR)** (version 0.9.11)

### Run
- Erlich

## Reference in README
[1]  
[2]  
[3]  


## Contact
E-mail: wldus8677@gmail.com  
Homepage: [CICL](http://cctl.jnu.ac.kr/)  