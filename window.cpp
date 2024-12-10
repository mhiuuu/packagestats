#include <map>
#include <ncurses.h>
#include <string>
#include <vector>
int highlight, height, width, list_width, info_width;

struct Package {
    std::string name;
    std::string description;
    /*std::map<std::string, std::vector<std::string>> dependencies;*/
    std::vector<std::string> dependencies;
};

std::vector<Package> packages = {
    {"Package A", "Description of Package A.", {"python", "c++"}},
    {"Package B", "Description of Package B.", {"python", "c++"}},
    {"Package C", "Description of Package C.", {"python", "c++"}},
    {"Package D", "Description of Package D.", {"python", "c++"}},
    {"Package E", "Description of Package E.", {"python", "c++"}}};

void draw_info(WINDOW *info_win, int highlight) {
    wclear(info_win);
    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 2, " Package Info ");
    if (highlight >= 0 && highlight < (int)packages.size()) {
        Package &pkg = packages[highlight];
        mvwprintw(info_win, 1, 2, "Name: %s", pkg.name.c_str());
        mvwprintw(info_win, 2, 2, "Description:");
        mvwprintw(info_win, 3, 4, "%s", pkg.description.c_str());
        mvwprintw(info_win, 4, 2, "Dependencies:");
        std::string dep_list = "";
        for (std::string x : pkg.dependencies) {
            dep_list += x + " ";
        }
        mvwprintw(info_win, 5, 4, "%s", dep_list.c_str());
    }
    wrefresh(info_win);
}

void draw_list(WINDOW *list_win, int highlight) {
    wclear(list_win);
    box(list_win, 0, 0);
    mvwprintw(list_win, 0, 2, " Packages ");
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i == highlight) {
            wattron(list_win, A_REVERSE);
        }
        mvwprintw(list_win, i + 1, 2, "%s", packages[i].name.c_str());
        if (i == highlight) {
            wattroff(list_win, A_REVERSE);
        }
    }
    wrefresh(list_win);
}

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    getmaxyx(stdscr, height, width);
    list_width = width / 3;
    info_width = width - list_width;

    WINDOW *list_win = newwin(height, list_width, 0, 0);
    WINDOW *info_win = newwin(height, info_width, 0, list_width);

    int ch;
    while (true) {
        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        } else if (ch == KEY_UP) {
            if (highlight > 0)
                --highlight;
        } else if (ch == KEY_DOWN) {
            if (highlight < (int)packages.size() - 1)
                ++highlight;
        }
        draw_info(info_win, highlight);
        draw_list(list_win, highlight);
    }

    delwin(list_win);
    delwin(info_win);
    endwin();

    return 0;
}
