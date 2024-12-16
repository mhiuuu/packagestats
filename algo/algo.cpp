/*
 * This code was implemented by myself which is just a part of the learning process while working on this project.
 * The complexity of the code is extremely high O(n^2.m) which is the Smith-Watermen algorithm itself is already O(m.n). The O(n) is allocated for the time where you have to run through n amount of elements in the list
 * The code performs great with a small list which is about only few hundreds. However, when the number is bigger, expect the application to be extremely lag and the program to eat half of your RAM
 * USE AT YOUR OWN RISK
 */
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> files;
std::string rootPath = "/", word = "yarn";
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
        for (auto const &entry : fs::recursive_directory_iterator(rootPath)) {
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
        mvwprintw(win, 1 + i, 2, "%s", files[list[i].second].c_str());
    }
    wrefresh(win);
}

void draw() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, true);
    curs_set(1);

    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *results = newwin(height - 3, width, 3, 0);
    WINDOW *inputWin = newwin(3, width, 0, 0);
    box(inputWin, 0, 0);
    mvwprintw(inputWin, 0, 1, " Search ");
    wrefresh(inputWin);

    std::string input = "";
    int ch;
    bool updateEnabled = true;

    while (true) {
        if (updateEnabled) {
            mvwprintw(inputWin, 1, 1, "%s", input.c_str());
            wclrtoeol(inputWin);
            wrefresh(inputWin);

            list.clear();
            comparingFiles(input);
            drawResults(results);
        }

        ch = getch();
        if (ch == 27) {
            break;
        } else if (ch == 10) {
            updateEnabled = !updateEnabled;
        } else if (updateEnabled) {
            if (ch == KEY_BACKSPACE || ch == 127) {
                if (!input.empty()) {
                    input.pop_back();
                }
            } else if (isprint(ch)) {
                input += ch;
            }
        }
    }

    delwin(results);
    delwin(inputWin);
    endwin();
}

int main() {
    listFiles();
    comparingFiles(word);
    draw();
}
