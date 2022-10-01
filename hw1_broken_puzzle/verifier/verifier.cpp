#include <stdlib.h>
#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "board.h"

using namespace std;

#define ARG                                                                              \
    do {                                                                                 \
        fprintf(stdout, "Usage: ./verifier -i [executable_program] -d [input_data] \n"); \
        fprintf(stdout, " -i [your program] \n");                                        \
        fprintf(stdout, " -d [test case] \n");                                           \
        return (1);                                                                      \
    } while (0)

bool can_exec(const char *file) {
    // https://stackoverflow.com/questions/5719694/how-to-check-if-file-is-executable-in-c
    struct stat st;
    if (stat(file, &st) < 0) {
        return (false);
    }
    if ((st.st_mode & S_IEXEC) != 0) {
        return (true);
    }
    return (false);
}

int main(int argc, char **argv) {
    int p_index, d_index;
    if (argc != 5) ARG;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            p_index = ++i;
        } else if (strcmp(argv[i], "-d") == 0) {
            d_index = ++i;
        } else
            ARG;
    }
    if (!can_exec(argv[p_index])) {
        throw std::runtime_error("specified executable program is not executable\n");
    }
    int n, m;
    Board b;
    ifstream in;
    in.open(argv[d_index]);
    if (!in.is_open()) {
        throw std::runtime_error("Fail to open input data\n");
    }
    in >> n >> m;
    in >> b;
    string p(argv[p_index]);
    string d(argv[d_index]);
    cout << b << '\n';
    string command = "timeout 12 " + p + " < " + d + " > " + "temp";
    cout << command << "\n\n\n";
    system(command.c_str());

    ifstream t;
    t.open("temp");
    // if(!t.is_open()){
    //	throw std::runtime_error("temp file fail to open\n");
    // }
    n = 0;
    t >> n;
    int position, move;
    for (int i = 0; i < n; i++) {
        cout << "slide" << '\n';
        t >> position >> move;
        cout << "position: " << position << '\n';
        if (move == 0) {
            cout << "move: "
                 << "up" << '\n';
            b.slide(position, Board::Action::up);
        } else if (move == 1) {
            cout << "move: "
                 << "down" << '\n';
            b.slide(position, Board::Action::down);
        } else if (move == 2) {
            cout << "move: "
                 << "left" << '\n';
            b.slide(position, Board::Action::left);
        } else if (move == 3) {
            cout << "move: "
                 << "right" << '\n';
            b.slide(position, Board::Action::right);
        }
        cout << b << '\n';
    }

    if (n != 0 && b.finish()) {
        cout << "sequence of move is valid" << '\n';
    } else {
        cout << "Something is Wrong" << '\n';
    }
    return 0;
}
