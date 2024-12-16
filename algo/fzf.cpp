#include <iostream>
#include <string>

class FileFinder {
  private:
    bool command_exists(const std::string &cmd) { return system(("which " + cmd + " >/dev/null 2>&1").c_str()) == 0; }

    void find_paths(const std::string &start_dir) {
        std::string command = "find '" + start_dir +
                              "' 2>/dev/null | fzf "
                              "--height=80%";
        system(command.c_str());
    }

  public:
    FileFinder() {
        if (!command_exists("fzf")) {
            throw std::runtime_error("Error: fzf is not installed");
        }
    }

    void run(int argc, char *argv[]) {
        std::string start_dir = (argc > 1) ? argv[1] : "/";
        try {
            find_paths(start_dir);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {
    try {
        FileFinder finder;
        finder.run(argc, argv);
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
