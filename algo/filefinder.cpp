#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int numfile = 0;
std::vector<std::string> filesPath;

/*void cached(std::string txt) {*/
/*    std::ofstream cache("cache.txt");*/
/*    if (cache.is_open()) {*/
/*        cache << txt << "\n";*/
/*        cache.close();*/
/*    } else {*/
/*        std::cout << "Cannot open file \n";*/
/*    }*/
/*}*/

void listFiles(const std::string &rootPath) {
    try {
        for (const auto &entry : fs::directory_iterator(
                 rootPath, fs::directory_options::skip_permission_denied)) {
            try {
                if (entry.is_symlink()) {
                    continue;
                }

                if (entry.is_regular_file() || entry.is_directory()) {
                    /*numfile++;*/
                    /*std::cout << entry.path() << "\n";*/
                    filesPath.push_back(entry.path());
                }
            } catch (const fs::filesystem_error &e) {
                std::cerr << "Error processing entry: " << e.what() << '\n';
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}

int levenshteinDist(std::string word1, std::string word2) {
    int size1 = word1.size();
    int size2 = word2.size();
    int verif[size1 + 1][size2 + 1];
    if (size1 == 0)
        return size2;
    if (size2 == 0)
        return size1;
    for (int i = 0; i <= size1; i++)
        verif[i][0] = i;
    for (int j = 0; j <= size2; j++)
        verif[0][j] = j;
    for (int i = 1; i <= size1; i++) {
        for (int j = 1; j <= size2; j++) {
            int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;
            verif[i][j] =
                std::min(std::min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                         verif[i - 1][j - 1] + cost);
        }
    }

    return verif[size1][size2];
}

/*int main() {*/
/**/
/*    std::string word1, word2;*/
/**/
/*    std::cout << "Please input the first word: " << std::endl;*/
/*    std::getline(std::cin, word1);*/
/*    std::cout << "Please input the second word: " << std::endl;*/
/*    std::getline(std::cin, word2);*/
/*    std::cout << "The number of modifications needed in order to make one word
 * "*/
/*                 "equal to the other is: "*/
/*              << levenshteinDist(word1, word2) << std::endl;*/
/**/
/*    return 0;*/
/*}*/

int main() {
    std::string rootPath = "/home/duck/";
    std::string sample = "yarn";
    listFiles(rootPath);
    for (std::string x : filesPath) {

        std::cout << "The path of x is" << x << " "
                  << levenshteinDist(x, sample) << "\n";
    }
    return 0;
}
