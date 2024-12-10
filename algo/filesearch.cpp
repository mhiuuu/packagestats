#include <algorithm>
#include <iostream>
#include <math.h>
#include <string>

using namespace std;

int levenshteinDist(string word1, string word2) {
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
            verif[i][j] = min(min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                              verif[i - 1][j - 1] + cost);
        }
    }

    return verif[size1][size2];
}

int main() {
    string word1, word2;

    cout << "Please input the first word: " << endl;
    getline(std::cin, word1);
    cout << "Please input the second word: " << endl;
    getline(std::cin, word2);
    cout << "The number of modifications needed in order to make one word "
            "equal to the other is: "
         << levenshteinDist(word1, word2) << endl;

    return 0;
}
