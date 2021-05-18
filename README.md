# Greedy Outlier Entropy [![GitHub license](https://img.shields.io/github/license/ducthanhtran/outlier_greedy_entropy)](https://github.com/ducthanhtran/outlier_greedy_entropy/blob/main/LICENSE)
A greedy algorithm which detects outliers in categorical data based on entropy measure. This is a C++20 implementation of
> He Z., Deng S., Xu X., Huang J.Z. (2006) A Fast Greedy Algorithm for Outlier Mining. In: Ng WK., Kitsuregawa M., Li J., Chang K. (eds) Advances in Knowledge Discovery and Data Mining. PAKDD 2006. Lecture Notes in Computer Science, vol 3918. Springer, Berlin, Heidelberg.

# Build
A C++20 supported compiler is required. Build the tool with
```bash
$ make
```

# Usage
```bash
./greedy_outlier_entropy -k <value> -d <data-file-path> [-o <output-file-path>]
```
Arguments:
* -k: Positive integer number of outliers.
* -d: Path to input data file in matrix form. Each row denotes a data point and each column stands for a categorical feature dimension.
      Data are seperated by a comma. Do not use any whitespaces between feature values.
* -o: (Optional) Path to output file with non-outlier data.

## Example Input
Three data points, each with four feature dimensions.
```
0,0,1,0
1,0,2,1
2,1,0,2
```
We can also use arbitrary strings as feature values:
```
James,a,g,k
Richard,b,d,p
David,b,e,o
```

# UCI Machine Learning Data
A bash script is provided to run the experiment on Lymphography data from the UCI Machine Learning Repository. Simply run
```bash
make experiments
```

## Data
* [Lymphography](https://archive.ics.uci.edu/ml/datasets/Lymphography): This lymphography domain was obtained from the University Medical Centre, Institute of Oncology, Ljubljana, Yugoslavia. Thanks go to M. Zwitter and M. Soklic for providing the data.

# Third Party
* [ProgramOptions.hxx](https://github.com/Fytch/ProgramOptions.hxx) for argument parsing (MIT licensed)

# License
This software is released under the [MIT license](LICENSE).
