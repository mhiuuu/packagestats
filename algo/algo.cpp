#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <ter
namespace fs = std::filesystem;
std::vector<std::string> files;
std::vector<std::pair<int, int>> list;
typedef enum {
    MATCH = 1,
    MISMATCH = -1,
    GAP = -2,
} score_t;

int smithWaterman(const std::string &seq1, const std::string &seq2) {
    size_t m = seq1.size();
    size_t n = seq2.size();

    std::vector<std::vector<int>> scoreMatrix(m + 1, std::vector<int>(n + 1, 0));

    int maxScore = 0;
    size_t maxI = 0, maxJ = 0;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int match = (seq1[i - 1] == seq2[j - 1]) ? MATCH : MISMATCH;

            scoreMatrix[i][j] = std::max({0, scoreMatrix[i - 1][j - 1] + match, scoreMatrix[i - 1][j] + GAP, scoreMatrix[i][j - 1] + GAP});

            if (scoreMatrix[i][j] > maxScore) {
                maxScore = scoreMatrix[i][j];
                maxI = i;
                maxJ = j;
            }
        }
    }
    /*std::string alignedSeq1, alignedSeq2;*/
    /*size_t i = maxI, j = maxJ;*/
    /**/
    /*while (i > 0 && j > 0 && scoreMatrix[i][j] > 0) {*/
    /*    if (scoreMatrix[i][j] == scoreMatrix[i - 1][j - 1] + ((seq1[i - 1] == seq2[j - 1]) ? MATCH : MISMATCH)) {*/
    /*        alignedSeq1 = seq1[i - 1] + alignedSeq1;*/
    /*        alignedSeq2 = seq2[j - 1] + alignedSeq2;*/
    /*        --i;*/
    /*        --j;*/
    /*    } else if (scoreMatrix[i][j] == scoreMatrix[i - 1][j] + GAP) {*/
    /*        alignedSeq1 = seq1[i - 1] + alignedSeq1;*/
    /*        alignedSeq2 = '-' + alignedSeq2;*/
    /*        --i;*/
    /*    } else {*/
    /*        alignedSeq1 = '-' + alignedSeq1;*/
    /*        alignedSeq2 = seq2[j - 1] + alignedSeq2;*/
    /*        --j;*/
    /*    }*/
    /*}*/
    /**/
    /*std::cout << "Alignment Score: " << maxScore << std::endl;*/
    /*std::cout << "Aligned Sequence 1: " << alignedSeq1 << std::endl;*/
    /*std::cout << "Aligned Sequence 2: " << alignedSeq2 << std::endl;*/
    return maxScore;
}

std::vector<std::string> listFiles(const std::string &root) {
    std::vector<std::string> results;

    try {
        for (auto const &entry : fs::directory_iterator(root)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                results.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "There is an error: " << e.what() << "\n";
    };

    return results;
}

void comparingFiles(std::vector<std::string> &files, std::string &pattern) {
    for (size_t i = 0; i < files.size(); i++) {
        int score = smithWaterman(files[i], pattern);
        list.push_back({score, i});
    }
}
void search() { std::string search; }
int main() {
    std::string seq1 = "yarn";
    std::string seq2 = "yay";
    std::string rootPath = "/home/duck/";
    files = listFiles(rootPath);
    comparingFiles(files, seq1);
    std::sort(list.begin(), list.end(), std::greater<>());
    for (size_t i = 0; i < files.size(); i++) {
        std::cout << files[list[i].second] << " scored " << list[i].first << "\n";
    }
    return 0;
}
