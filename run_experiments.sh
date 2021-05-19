#!/bin/bash
parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd -P)
cd "${parent_path}"

EXPERIMENTS_DIR="./experiments"
mkdir -p ${EXPERIMENTS_DIR}
FEATURES_DIR="./data/features/"
mkdir -p ${FEATURES_DIR}

LYMPH_DATA="./data/lymphography.data"
LYMPH_DATA_FEATURES="${FEATURES_DIR}/lymphography.data.features"
declare -a LYMPH_RARE_CLASSES=([1]=0 [4]=0) # values of one are ignoarable, only needs to be non-empty
echo "$(date -u): Running on lymphography data (${LYMPH_DATA_FEATURES})"
echo "$(date -u): Preprocessing: ignore first column (class)"
echo "$(date -u): Resulting features are located at ${LYMPH_DATA_FEATURES}"
cut -d, -f1 --complement ${LYMPH_DATA} > ${LYMPH_DATA_FEATURES}

# get total number of rare classes 1 and 4
TOTAL_OUTLIERS=$(cut -f1 -d, ${LYMPH_DATA} | sort | uniq -c | sed 's/^ \+//g'\
                    | cut -f1 -d ' ' | sed '2,3d' | awk '{ SUM += $1 } END { print SUM }')

for k in 7 15 16 22 30; do
    echo -e "$(date -u): Running with k=${k}"
    ./greedy_outlier_entropy -k ${k} -d ${LYMPH_DATA_FEATURES} > "${EXPERIMENTS_DIR}/result.lymphography.k${k}"

    # compute coverage
    ## read outlier indices into an array
    outliers=$(sed '4q;d' ${EXPERIMENTS_DIR}/result.lymphography.k${k}\
                | sed 's/Outliers: //'\
                | sed 's/, /\\n/g')
    readarray outlierIndices <<< $(echo -e ${outliers})

    ## compute percentage of covered outliers
    covered=0
    for i in ${outlierIndices[@]}; do
        index=$((${i} + 1)) # output of outlier detection tool is zero-index
        class=$(cut -d, -f1 ${LYMPH_DATA} | sed "${index}q;d")
        if [[ -n "${LYMPH_RARE_CLASSES[${class}]}" ]]; then
            ((++covered))
        fi
    done

    coverage=$(awk "BEGIN { print ${covered} / ${TOTAL_OUTLIERS} }")
    echo -e "$(date -u): \tOutlier coverage = ${coverage}. Absolute = ${covered}"
done

echo "$(date -u): Finished lymphography data. Results are located at ${EXPERIMENTS_DIR}/"
