#include "include/draw.h"
#include <cstdlib>
#include <cstring>
#include <menu.h>
#include <ncurses.h>
#include <string>
#include <vector>
#define CTRLD 4

std::vector<std::string> choices = {"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Choice 5", "Choice 6", "Choice 7", "Choice 8", "Choice 9", "Choice 10", "Exit"};

int main() {
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win;
    int n_choices, i;
    int height, width;

    initWin(true, false, true);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    getmaxyx(stdscr, height, width);
    n_choices = choices.size();
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for (i = 0; i < n_choices; ++i)
        my_items[i] = new_item(choices[i].c_str(), nullptr);

    my_menu = new_menu((ITEM **)my_items);

    my_menu_win = newwin(height, width, 0, 0);
    keypad(my_menu_win, TRUE);

    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, height - 4, width - 2, 3, 1));
    set_menu_format(my_menu, height - 6, 1);
    set_menu_mark(my_menu, " * ");

    box(my_menu_win, 0, 0);
    alignRight(my_menu_win, 1, 0, width, "My Menu", COLOR_PAIR(1));
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
    mvwhline(my_menu_win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(my_menu_win, 2, width - 1, ACS_RTEE);

    post_menu(my_menu);
    wrefresh(my_menu_win);

    attron(COLOR_PAIR(2));
    mvprintw(LINES - 2, 0, "Use PageUp and PageDown to scroll down or up a page of items");
    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit)");
    attroff(COLOR_PAIR(2));
    refresh();
    handle_user_input(my_menu, my_menu_win);
    cleanup(my_menu, my_items, n_choices);
}
