import bitdoku;

#include <bitdoku_gui.hpp>
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <board_string>\n";
        return 1;
    }
    auto board_arg = std::string(argv[1]);
    if (board_arg.size() != 81) {
        std::cerr << "Board string must be 81 characters long\n";
        return 1;
    }
    auto solver = Bitdoku(board_arg);
    SudokuVisualizer visualizer(540, solver);
    visualizer.run();
    return 0;
}