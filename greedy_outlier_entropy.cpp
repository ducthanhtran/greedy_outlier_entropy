// Copyright (C) 2021 Duc Thanh Tran
// Distributed under the MIT License.
// For further information, see LICENSE file or visit https://opensource.org/licenses/MIT
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "greedy_outlier_entropy.hpp"


/**
    Mark a data point as an outlier and return occurrences for entropy computation.

    @param dataset Dataset with occurences that take into account previous marked outliers
    @param index Index of data point which is to be marked as an outlier
*/
Occurences mark_outlier(const Dataset& dataset, const std::size_t index)
{
    auto markedOccurences = dataset.occurences;
    for(std::size_t i{0}; i < dataset.data[index].size(); ++i)
    {
        const auto value = dataset.data[index][i];
        --markedOccurences[i][value];
        if(markedOccurences[i][value] == static_cast<std::size_t>(0))
        {
            markedOccurences[i].erase(value);
        }
    }
    return markedOccurences;
}

/**
    Reads in a data file which contains a two-dimensional feature matrix.
    Each row represents a data point and its features. Features can be arbitrary strings
    and are seperated by a comma. No whitespaces are allowed.

    Example:
        1,2,a
        0,1,b
        0,2,c

    @param file_path Data file path
    @return Dataset with occurences of each feature value in each dimension
*/
[[nodiscard]]
Dataset read(const std::filesystem::path& file_path)
{
    // Read each data point into a vector
    std::ifstream infile{file_path};
    if(infile)
    {
        Data data;
        for(std::string line; std::getline(infile, line); )
        {
            std::vector<std::string> dataPoint;
            std::istringstream ss {line};
            for(std::string value; std::getline(ss, value, ','); )
            {
                dataPoint.push_back(value);
            }
            data.push_back(dataPoint);
        }

        // Check whether all data points have the same number of dimensions
        const auto dimensions{data.front().size()};
        const auto dataIsValid = std::ranges::all_of(data,
            [dimensions](const auto &x){ return x.size() == dimensions; });

        if(!dataIsValid)
        {
            throw std::invalid_argument("Data points have uneven number of dimensions");
        }

        // Count occurences of each dimension
        Occurences occurences(dimensions);
        for(const auto &dataPoint : data)
        {
            for(std::size_t i{0}; i < dimensions; ++i)
            {
                const auto value{dataPoint[i]};
                ++occurences[i][value];
            }
        }
        return Dataset{data, occurences};
    }
    else
    {
        throw std::runtime_error("Could not read data file");
    }
}

/**
    Computes the entropy of a dataset, taking into account occurences and the
    total number of non-outlier data points.
    Assumption of feature independence, hence entropy can be summed up by
    considering each dimension entropy. Therefore, Shannon's entropy value is
    approximated.

    @param occurences Occurences of features of non-outlier data points
    @param numDataPoints Total number of non-outlier data points
    @return Shannon's entropy value
*/
[[nodiscard]]
double entropy(const Occurences& occurences, const std::size_t numDataPoints)
{
    double entropy{0.0};
    for(const auto& occ : occurences)
    {
        for(const auto& [v, number] : occ)
        {
            const auto p{static_cast<double>(number) / numDataPoints};
            entropy -= p * log2(p);
        }
    }
    return entropy;
}

/**
    As proposed in

        He Z., Deng S., Xu X., Huang J.Z. (2006)
        A Fast Greedy Algorithm for Outlier Mining.
        In: Ng WK., Kitsuregawa M., Li J., Chang K. (eds) Advances in Knowledge Discovery and Data Mining. 
        PAKDD 2006. Lecture Notes in Computer Science, vol 3918. Springer, Berlin, Heidelberg.

    A greedy algorithm which takes Shannon's entropy to identify outliers on categorical data. A data point
    which reduces the dataset's entropy at most is considered an as outlier. The algorithm terminates after
    having marked exactly k data points as outliers.

    @param dataset Dataset
    @param k Number of outliers to be marked by the algorithm
    @return Result outlier indices and entropy of non-outlier data
*/
[[nodiscard]]
Result greedy_outliers(Dataset& dataset, const std::size_t k)
{
    if(k > dataset.data.size())
    {
        throw std::runtime_error("K parameter exceeds number of data points");
    }

    auto bestEntropy{entropy(dataset.occurences, dataset.data.size())};

    std::unordered_set<std::size_t> outliers;
    outliers.reserve(k);
    while(outliers.size() < k)
    {
        std::optional<std::size_t> bestIndex;
        for(std::size_t index{0}; index < dataset.data.size(); ++index)
        {
            if(!outliers.contains(index))
            {
                const auto markedOccurences = mark_outlier(dataset, index);
                const auto markedEntropy = entropy(markedOccurences,
                                                   dataset.data.size() - outliers.size() - 1);
                if(markedEntropy < bestEntropy)
                {
                    bestEntropy = markedEntropy;
                    bestIndex = index;
                }

            }
        }
        if(!bestIndex)
        {
            throw std::invalid_argument("k is set too high, no improvement by marking outliers");
        }
        outliers.insert(*bestIndex);
        dataset.occurences = mark_outlier(dataset, *bestIndex);
    }
    return Result{outliers, bestEntropy};
}

/**
    Writes non-outlier features into an output file.

    @param out_file_path Output file path
    @param result Result of greedy algorihm
    @param data Data values
*/
void write_data(const std::filesystem::path& out_file_path, const Result& result, const Data& data)
{
    std::ofstream out_file(out_file_path);
    const auto write = [&out_file](const std::vector<std::string>& x)
    {
        for(bool first{true}; const auto& value : x)
        {
            out_file << (first ? first = false, "" : ",") << value;
        }
        out_file << '\n';
    };

    for(std::size_t index = 0; const auto& x : data)
    {
        if(!result.outliers.contains(index))
        {
            write(x);
        }
        ++index;
    }
}
