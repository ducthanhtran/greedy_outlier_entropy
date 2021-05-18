// Copyright (C) 2021 Duc Thanh Tran
// Distributed under the MIT License.
// For further information, see LICENSE file or visit https://opensource.org/licenses/MIT
#ifndef OUTLIER_DETECTION_HPP
#define OUTLIER_DETECTION_HPP

#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Data = std::vector<std::vector<std::string>>;
using Occurences = std::vector<std::unordered_map<std::string, std::size_t>>;

/**
    Dataset which holds data (feature) values and its occurences for each dimension.
*/
struct Dataset
{
    Data data;
    Occurences occurences;
};

/**
    Result of the greedy algorithm which contains outlier indices and the
    entropy value of all non-outlier data points.
*/
struct Result
{
    std::unordered_set<std::size_t> outliers;
    double entropy;
};

Occurences mark_outlier(const Dataset& dataset, const std::size_t index);
[[nodiscard]] Dataset read(const std::filesystem::path& file_path);
[[nodiscard]] double entropy(const Occurences& occurences, const std::size_t numDataPoints);
[[nodiscard]] Result greedy_outliers(Dataset& dataset, const std::size_t k);
void write_data(const std::filesystem::path& out_file_path, const Result& result, const Data& data);
#endif // OUTLIER_DETECTION_HPP
