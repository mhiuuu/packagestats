#include <cstddef>
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <string>

void initWin(bool use_color = false, bool enable_echo = false, bool show_cursor = false) {
    initscr();
    if (use_color)
        start_color();
    enable_echo ? echo() : noecho();
    curs_set(show_cursor);
}

void alignMiddle(WINDOW *win, int starty, int startx, int width, const std::string &string, chtype color) {
    size_t length;
    int x, y;
    win = win == nullptr ? stdscr : win;
    getyx(win, y, x);
    x = startx != 0 ? startx : x;
    y = starty != 0 ? starty : y;
    width = width == 0 ? 80 : width;
    length = string.size();
    x = startx + static_cast<int>((width - length) / 2);
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string.c_str());
    wattroff(win, color);
    refresh();
}

void alignRight(WINDOW *win, int starty, int startx, int width, const std::string &string, chtype color) {
    size_t length;
    int x, y;
    win = win == nullptr ? stdscr : win;
    width = width == 0 ? 80 : width;
    getyx(win, y, x);
    x = startx != 0 ? startx : x;
    y = starty != 0 ? starty : y;
    length = string.size();
    x = startx + width - length - 1;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string.c_str());
    wattroff(win, color);
    refresh();
}

void cleanup(MENU *menu, ITEM **items, size_t size) {
    unpost_menu(menu);
    free_menu(menu);
    for (size_t i = 0; i < size; ++i) {
        free_item(items[i]);
    }
    free(items);
    endwin();
}
void drawLine(WINDOW *win, int width) {
    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, width - 2);
    mvwaddch(win, 2, width - 1, ACS_RTEE);
}
void handle_user_input(MENU *menu, WINDOW *menu_win) {
    int c;
    ITEM *prev_item = current_item(menu);

    while ((c = wgetch(menu_win)) != KEY_F(1)) {
        switch (c) {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case KEY_NPAGE:
            menu_driver(menu, REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            menu_driver(menu, REQ_SCR_UPAGE);
            break;
        }

        // Check if selection changed
        ITEM *current = current_item(menu);
        if (current != prev_item) {
            mvprintw(20, 2, "You selected: %s", item_name(current));
            prev_item = current;
        }

        wrefresh(menu_win);
    }
}
