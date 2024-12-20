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
    std::string version;
    std::string description;
    std::string architecture;
    std::string url;
    std::string licenses;
    std::string depends_on;
    std::string installed_size;
    std::string install_date;
    bool details_loaded = false;
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

void loadPackageDetails(Package &pkg) {
    if (pkg.details_loaded)
        return;

    std::string command = "yay -Qi " + pkg.name;
    std::string output = exec(command.c_str());
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty())
            continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;

        std::string field = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            value.erase(0, 1);
        while (!value.empty() && (value.back() == ' ' || value.back() == '\t'))
            value.pop_back();

        if (field.find("Version") != std::string::npos)
            pkg.version = value;
        else if (field.find("Description") != std::string::npos)
            pkg.description = value;
        else if (field.find("Architecture") != std::string::npos)
            pkg.architecture = value;
        else if (field.find("URL") != std::string::npos)
            pkg.url = value;
        else if (field.find("Licenses") != std::string::npos)
            pkg.licenses = value;
        else if (field.find("Depends On") != std::string::npos)
            pkg.depends_on = value;
        else if (field.find("Installed Size") != std::string::npos)
            pkg.installed_size = value;
        else if (field.find("Install Date") != std::string::npos)
            pkg.install_date = value;
    }

    pkg.details_loaded = true;
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
                packages.push_back(pkg);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void drawPackageInfo(WINDOW *info_win, Package &pkg) {
    wclear(info_win);
    box(info_win, 0, 0);

    if (!pkg.details_loaded) {
        mvwprintw(info_win, height / 2, (info_width / 2) - 10, "Loading package details...");
        wrefresh(info_win);
        loadPackageDetails(pkg);
        wclear(info_win);
        box(info_win, 0, 0);
    }

    int y = 1;
    int padding = 2;
    int max_label_width = 15;
    int content_width = info_width - max_label_width - padding - 2;

    auto printField = [&](const char *label, const std::string &value) {
        mvwprintw(info_win, y, padding, "%s:", label);

        std::string remaining = value;
        int x_pos = padding + max_label_width;

        while (!remaining.empty() && y < height - 2) {
            std::string chunk = remaining.substr(0, content_width);
            if (chunk.length() == content_width && remaining.length() > content_width) {
                size_t last_space = chunk.find_last_of(" ");
                if (last_space != std::string::npos) {
                    chunk = chunk.substr(0, last_space);
                }
            }

            mvwprintw(info_win, y++, x_pos, "%s", chunk.c_str());

            remaining = remaining.substr(chunk.length());
            if (!remaining.empty()) {
                x_pos = padding + max_label_width;
                remaining = remaining.substr(remaining.find_first_not_of(" "));
            }
        }
        y++;
    };

    printField("Name", pkg.name);
    printField("Version", pkg.version);
    printField("Description", pkg.description);
    printField("Architecture", pkg.architecture);
    printField("URL", pkg.url);
    printField("Licenses", pkg.licenses);
    printField("Depends On", pkg.depends_on);
    printField("Installed Size", pkg.installed_size);
    printField("Install Date", pkg.install_date);

    wrefresh(info_win);
}

void drawMenu() {
    size_t pkg_list_size = packages.size();
    ITEM **pkg = (ITEM **)calloc(pkg_list_size + 1, sizeof(ITEM *));
    for (size_t i = 0; i < pkg_list_size; i++) {
        pkg[i] = new_item(packages[i].name.c_str(), "");
    }
    pkg[pkg_list_size] = nullptr;

    MENU *pkg_list = new_menu(pkg);
    WINDOW *pkg_win = newwin(height, list_width, 0, 0);
    WINDOW *info_win = newwin(height, info_width, 0, list_width);

    keypad(pkg_win, TRUE);
    set_menu_win(pkg_list, pkg_win);
    set_menu_sub(pkg_list, derwin(pkg_win, height - 3, list_width - 2, 3, 1));
    set_menu_format(pkg_list, height - 6, 1);
    set_menu_mark(pkg_list, " * ");

    box(pkg_win, 0, 0);
    box(info_win, 0, 0);
    alignMiddle(pkg_win, 1, 0, list_width, "Packages", COLOR_PAIR(0));
    drawLine(pkg_win, list_width);
    /*alignMiddle(info_win, 1, 0, info_width, "Package Information", COLOR_PAIR(0));*/
    /*drawLine(info_win, info_width);*/

    post_menu(pkg_list);
    wrefresh(pkg_win);
    wrefresh(info_win);

    if (!packages.empty()) {
        drawPackageInfo(info_win, packages[0]);
    }

    attron(COLOR_PAIR(0));
    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (Esc to Exit)");
    /*attroff(COLOR_PAIR(0));*/
    refresh();

    int c;
    while ((c = wgetch(pkg_win)) != 27) {
        switch (c) {
        case KEY_DOWN:
            menu_driver(pkg_list, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(pkg_list, REQ_UP_ITEM);
            break;
        }

        ITEM *cur = current_item(pkg_list);
        int index = item_index(cur);
        if (index >= 0 && index < static_cast<int>(packages.size())) {
            drawPackageInfo(info_win, packages[index]);
        }

        wrefresh(pkg_win);
    }

    unpost_menu(pkg_list);
    free_menu(pkg_list);
    for (size_t i = 0; i < pkg_list_size; i++) {
        free_item(pkg[i]);
    }
    free(pkg);
    delwin(pkg_win);
    delwin(info_win);
}

int main() {
    fillPackages();
    initWin(true, false, true);
    getmaxyx(stdscr, height, width);
    list_width = width / 3;
    info_width = width - list_width;
    drawMenu();
    endwin();
    return 0;
}
