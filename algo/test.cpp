#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int levenshteinDistance(const std::string &s1, const std::string &s2) {
    size_t word1 = s1.size();
    size_t word2 = s2.size();
    std::vector<std::vector<int>> dp(word1 + 1, std::vector<int>(word2 + 1));

    for (size_t i = 0; i <= word1; ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= word2; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= word1; ++i) {
        for (size_t j = 1; j <= word2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
        }
    }
    return dp[word1][word2];
}

std::vector<std::string> searchFiles(const std::string &directory, const std::string &query, int maxDistance = 3) {
    std::vector<std::string> results;

    try {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                std::string path = entry.path();
                std::string filename = entry.path().filename();

                if (filename.find(query) != std::string::npos || levenshteinDistance(filename, query) <= maxDistance) {
                    results.push_back(path);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error accessing filesystem: " << e.what() << '\n';
    }

    return results;
}

int main() {
    std::string rootPath = "/home/duck/";
    std::string query;
    int maxDistance = 3;

    std::cout << "Search files: (type and press Enter to search, type 'exit' to quit)\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, query);

        if (query == "exit")
            break;

        std::vector<std::string> results = searchFiles(rootPath, query, maxDistance);

        std::cout << "Matching files:\n";
        for (const auto &path : results) {
            std::cout << " - " << path << '\n';
        }
    }

    return 0;
}
