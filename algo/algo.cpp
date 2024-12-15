#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> files;
std::string rootPath = "/home/duck/", seq1 = "yarn";
std::vector<std::pair<int, int>> list;

typedef enum {
    MATCH = 1,
    MISMATCH = -1,
    GAP = -2,
    SPECIAL_PLACE_BONUS = 2,
} score_t;

/*int smithWaterman(const std::string &seq1, const std::string &seq2) {*/
/*    size_t m = seq1.size();*/
/*    size_t n = seq2.size();*/
/**/
/*    std::vector<std::vector<int>> scoreMatrix(m + 1, std::vector<int>(n + 1, 0));*/
/**/
/*    int maxScore = 0;*/
/*    size_t maxI = 0, maxJ = 0;*/
/**/
/*    for (size_t i = 1; i <= m; ++i) {*/
/*        for (size_t j = 1; j <= n; ++j) {*/
/*            int match = (seq1[i - 1] == seq2[j - 1]) ? MATCH : MISMATCH;*/
/**/
/*            scoreMatrix[i][j] = std::max({0, scoreMatrix[i - 1][j - 1] + match, scoreMatrix[i - 1][j] + GAP, scoreMatrix[i][j - 1] + GAP});*/
/**/
/*            if (scoreMatrix[i][j] > maxScore) {*/
/*                maxScore = scoreMatrix[i][j];*/
/*                maxI = i;*/
/*                maxJ = j;*/
/*            }*/
/*        }*/
/*    }*/
/**/
/*    // Bonus logic*/
/*    if (seq1.compare(0, seq2.size(), seq2) == 0) {*/
/*        maxScore += SPECIAL_PLACE_BONUS;*/
/*    }*/
/*    if (seq1.compare(seq1.size() - seq2.size(), seq2.size(), seq2) == 0) {*/
/*        maxScore += SPECIAL_PLACE_BONUS;*/
/*    }*/
/**/
/*    return maxScore;*/
/*}*/

int smithWaterman(const std::string &seq1, const std::string &seq2) {
    const int ScoreMatch = 16;
    const int ScoreGapStart = -3;
    const int ScoreGapExtension = -1;
    const int BonusBoundary = ScoreMatch / 2;
    const int BonusConsecutive = -(ScoreGapStart + ScoreGapExtension);
    const int BonusFirstCharMultiplier = 2;

    size_t m = seq1.size();
    size_t n = seq2.size();

    std::vector<int> prevRow(n + 1, 0), currRow(n + 1, 0);
    int maxScore = 0;

    for (size_t i = 1; i <= m; ++i) {
        int prevDiag = 0;
        bool inGap = false;

        for (size_t j = 1; j <= n; ++j) {
            int matchScore = (seq1[i - 1] == seq2[j - 1]) ? ScoreMatch : ScoreGapStart;
            int boundaryBonus = (j == 1 || j == n) ? BonusBoundary : 0;

            int diag = prevDiag + matchScore + boundaryBonus;
            int left = currRow[j - 1] + (inGap ? ScoreGapExtension : ScoreGapStart);
            int up = prevRow[j] + ScoreGapStart;

            currRow[j] = std::max({0, diag, left, up});
            maxScore = std::max(maxScore, currRow[j]);

            inGap = (currRow[j] == left);
            prevDiag = prevRow[j];
        }
        std::swap(prevRow, currRow);
    }

    if (seq1.compare(0, seq2.size(), seq2) == 0) {
        maxScore += BonusFirstCharMultiplier * BonusBoundary;
    }
    if (seq1.compare(seq1.size() - seq2.size(), seq2.size(), seq2) == 0) {
        maxScore += BonusBoundary;
    }

    return maxScore;
}

std::vector<std::string> listFiles() {
    std::vector<std::string> results;

    try {
        for (auto const &entry : fs::directory_iterator(rootPath)) {
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
    list.clear();
    for (size_t i = 0; i < files.size(); i++) {
        int score = smithWaterman(files[i], pattern);
        list.push_back({score, i});
    }
}
void search() { std::string search; }

int main() {
    files = listFiles();
    comparingFiles(files, seq1);
    std::sort(list.begin(), list.end(), std::greater<>());
    for (size_t i = 0; i < files.size(); i++) {
        std::cout << files[list[i].second] << " scored " << list[i].first << "\n";
    }
    return 0;
}
