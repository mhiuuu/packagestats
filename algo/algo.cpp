#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> files;
std::string rootPath = "/", word;
std::vector<std::pair<int, int>> list;

const int ScoreMatch = 16;
const int ScoreGapStart = -3;
const int ScoreGapExtension = -1;
const int BonusBoundary = ScoreMatch / 2;
const int BonusConsecutive = -(ScoreGapStart + ScoreGapExtension);
const int BonusFirstCharMultiplier = 2;

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

void listFiles() {
    files.clear();
    try {
        for (auto const &entry : fs::directory_iterator(rootPath)) {
            if (entry.is_regular_file() || entry.is_directory()) {
                files.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << "There is an error while listing files of your system, here is the error you want to check it out: " << e.what() << "\n";
    };
}

void comparingFiles(std::string &pattern) {
    for (size_t i = 0; i < files.size(); i++) {
        int score = smithWaterman(files[i], pattern);
        list.push_back({score, i});
    }
    std::sort(list.begin(), list.end(), std::greater<>());
}

void drawResults(WINDOW *win) {
    std::string resList = "";
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, " Search results ");
    for (size_t i = 0; i < files.size(); i++) {
        /*TODO: It will go down a line then overlay the frame */
        resList += files[list[i].second] + "\n";
    }
    mvwprintw(win, 1, 3, "%s", resList.c_str());
    wrefresh(win);
}

void draw() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *results = newwin(height, width, 0, 0);

    int8_t ch;
    while (true) {
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        drawResults(results);
    }
    delwin(results);
    endwin();
}

int main() {
    listFiles();
    comparingFiles(word);
    draw();
}
