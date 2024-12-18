#include "include/draw.h"
#include <array>
#include <cstddef>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <memory>
#include <menu.h>
#include <ncurses.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int highlight, height, width, list_width, info_width;

struct Package {
    std::string name;
    std::string description;
    std::vector<std::string> dependencies;
};

std::vector<Package> packages;

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, void (*)(FILE *)> pipe(popen(cmd, "r"), [](FILE *f) {
        if (f)
            std::ignore = pclose(f);
    });
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void fillPackages() {
    try {
        const char *command = "yay -Q";
        std::string output = exec(command);
        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string packageName;
            if (lineStream >> packageName) {
                Package pkg;
                pkg.name = packageName;
                pkg.description = "Description for " + packageName;
                pkg.dependencies = {"dep1", "dep2"};

                packages.push_back(pkg);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main() {
    fillPackages();
    initWin(true, false, true);

    getmaxyx(stdscr, height, width);
    list_width = width / 3;
    info_width = width - list_width;

    size_t pkg_list_size = packages.size();
    ITEM **pkg = (ITEM **)calloc(pkg_list_size + 1, sizeof(ITEM *));

    for (size_t i = 0; i < pkg_list_size; i++) {
        pkg[i] = new_item(packages[i].name.c_str(), "");
    }
    pkg[pkg_list_size] = nullptr;
    MENU *pkg_list = new_menu(pkg);

    WINDOW *pkg_win = newwin(height, list_width, 0, 0);
    keypad(pkg_win, TRUE);
    set_menu_win(pkg_list, pkg_win);
    set_menu_sub(pkg_list, derwin(pkg_win, height - 4, list_width - 2, 3, 1));
    set_menu_format(pkg_list, height - 6, 1);
    set_menu_mark(pkg_list, " * ");

    box(pkg_win, 0, 0);
    alignMiddle(pkg_win, 1, 0, list_width, "My menu", COLOR_PAIR(0));
    drawLine(pkg_win, list_width);

    post_menu(pkg_list);
    wrefresh(pkg_win);

    attron(COLOR_PAIR(2));
    mvprintw(LINES - 2, 0, "Use PageUp and PageDown to scroll down or up a page of items");
    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit)");
    attroff(COLOR_PAIR(2));
    refresh();

    handle_user_input(pkg_list, pkg_win);

    cleanup(pkg_list, pkg, pkg_list_size);
    delwin(pkg_win);
    endwin();

    return 0;
}
