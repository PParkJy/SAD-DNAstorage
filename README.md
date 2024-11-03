# Sequence analysis and decoding with extra low-quality reads for DNA data storage
This repository is for the study **"Sequence analysis and decoding with extra low-quality reads for DNA data storage"** which submitted to _Bioinformatics_ in 2024.  
Here, we provide the source code and experimental sequencing data.  
(Current version was revised in Nov. 3, 2024)  

## Dataset
We performed Illumina NGS sequencing (MiSeq) with cycles 151-6-151 (R1-index-R2).  
Based on MiSeq [configurations](https://support.illumina.com/downloads/miseq-product-documentation.html), we obtain some raw sequencing data to get NPF (non-pass chastity filter) reads.   
We 

### Raw data (./dataset/raw/)
- *.cif
- *.filter
- *.locs
  
  [raw_format](./img/raw_format.png)

### FASTQ (./dataset/fastq/)
- AYB-basecalled FASTQ
- Illumina-basecalled FASTQ

## Sequence analysis and decoding
### Requirements
#### Language
- Python
- Matlab

#### Used open-source software
- Edit distance based-clustering **[Starcode](https://github.com/gui11aume/starcode)**
- Paired-end read merging **[PEAR](https://github.com/tseemann/PEAR)**
- Sequence alignment **[MUSCLE](https://github.com/rcedgar/muscle)**

### Run
- The run files for Erlich-PF, Erlich-ExtraNPF, Prop-ExtraNPF are in `src/'.  
- The generated results are saved in 'result/'.  
- 

## Contact
E-mail: wldus8677@gmail.com  
Homepage: [CICL](http://cctl.jnu.ac.kr/)  