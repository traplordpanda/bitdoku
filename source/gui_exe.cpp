#include <bitdoku_gui.hpp>
#include "bitdoku.hpp"
int main() {
    auto solver = Bitdoku("400800009000050000008007100000002000000570021000001406600000000000200800701960005");
    SudokuVisualizer visualizer(540, solver);
    visualizer.run();
    return 0;
}