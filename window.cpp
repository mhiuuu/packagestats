#include <ncurses.h>
#include <string>
#include <vector>

// Sample data
struct Package {
    std::string name;
    std::string description;
};

std::vector<Package> packages = {{"Package A", "Description of Package A."},
                                 {"Package B", "Description of Package B."},
                                 {"Package C", "Description of Package C."},
                                 {"Package D", "Description of Package D."},
                                 {"Package E", "Description of Package E."}};

void draw_interface(WINDOW *list_win, WINDOW *info_win, int highlight) {
    wclear(list_win);
    wclear(info_win);

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

    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 2, " Package Info ");
    if (highlight >= 0 && highlight < (int)packages.size()) {
        mvwprintw(info_win, 1, 2, "Name: %s", packages[highlight].name.c_str());
        mvwprintw(info_win, 2, 2, "Description:");
        mvwprintw(info_win, 3, 4, "%s",
                  packages[highlight].description.c_str());
    }

    wrefresh(list_win);
    wrefresh(info_win);
}

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height, width;
    getmaxyx(stdscr, height, width);

    int list_width = width / 3;
    int info_width = width - list_width;

    WINDOW *list_win = newwin(height, list_width, 0, 0);
    WINDOW *info_win = newwin(height, info_width, 0, list_width);

    int highlight = 0;

    draw_interface(list_win, info_win, highlight);

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
        draw_interface(list_win, info_win, highlight);
    }

    delwin(list_win);
    delwin(info_win);
    endwin();

    return 0;
}
