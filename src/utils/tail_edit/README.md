# Tailored edit distance-based clustering
This clustering algorithm exclude the deletions or insertions in the total edit distance between reads of different length.  
Reads are clustered when the adjusted edit distance between the reads do not exceed a given threshold.  

This algorithm is based on the sphere clustering algorithm of **[Starcode](https://github.com/gui11aume/starcode)**  

## Options
If you run `./tailored_clustering` without any options, the required options are printed.  
You also find the required options in `prop.sh`.  

