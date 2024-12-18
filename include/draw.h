#ifndef MENU_HELPERS_H
#define MENU_HELPERS_H

#include <cstddef>
#include <curses.h>
#include <menu.h>
#include <ncurses.h>
#include <string>

void initWin(bool use_color = false, bool enable_echo = false, bool show_cursor = false);

void alignMiddle(WINDOW *win, int starty, int startx, int width, const std::string &string, chtype color);

void alignRight(WINDOW *win, int starty, int startx, int width, const std::string &string, chtype color);

void cleanup(MENU *menu, ITEM **items, size_t size);

void drawLine(WINDOW *win, int width);

void handle_user_input(MENU *menu, WINDOW *menu_win);

#endif // MENU_HELPERS_H
