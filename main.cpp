// Copyright (C) 2021 Duc Thanh Tran
// Distributed under the MIT License.
// For further information, see LICENSE file or visit https://opensource.org/licenses/MIT 
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include "greedy_outlier_entropy.hpp"
#include "ProgramOptions.hxx"


int main(int argc, char* argv[])
{
    po::parser parser;
    auto& help = parser["help"]
        .abbreviation('h')
        .description("Print help")
        .callback([&]{ std::cout << parser << '\n'; });
    
    auto& outliers = parser["outliers"]
        .abbreviation('k')
        .description("Number of outliers")
        .type(po::u64);

    auto& data = parser["data"]
        .abbreviation('d')
        .description("Path to input data file")
        .type(po::string);

    auto& output = parser["output"]
        .abbreviation('o')
        .description("OPTIONAL: Path to output file which contains non-outlier data")
        .type(po::string);

    if(!parser(argc, argv))
    {
        std::cout << "Could not parse arguments\n";
        std::cout << parser << '\n';
        return 1;
    }

    if(help.was_set())
    {
        return 0;
    }

    if(!outliers.available() || !data.available())
    {
        std::cout << "Number of outliers and data file are required\n";
        std::cout << parser << '\n';
        return 1;
    }

    try
    {
        const std::filesystem::path file_path{data.get().string};
        auto dataset = read(file_path);

        std::cout << "Loaded " << dataset.data.size() << " datapoints, each with " << dataset.data.front().size() << " dimensions.\n";
        std::cout << "Entropy of original data: " << entropy(dataset.occurences, dataset.data.size()) << '\n';

        const auto numOutliers{static_cast<std::size_t>(outliers.get().u64)};
        const auto result = greedy_outliers(dataset, numOutliers);
        std::cout << "Entropy of data without outliers: " << result.entropy << '\n';

        std::vector<std::size_t> sorted_outliers(result.outliers.cbegin(), result.outliers.cend());
        std::ranges::sort(sorted_outliers);
        std::cout << "Outliers: ";
        for(auto first{true}; const auto out : sorted_outliers)
        {
            std::cout << (first ? first = false, "" : ", ") << out;
        }
        std::cout << '\n';
        
        if(output.available())
        {
            const std::filesystem::path out_file_path(output.get().string);
            write_data(out_file_path, result, dataset.data);
        }
    }
    catch(const std::exception &e)
    {
        std::cout << "Error: " << e.what() << '\n';
    }
    return 0;
}
