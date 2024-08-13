import bitdoku;

#include <bitdoku_gui.hpp>
int main() {
    auto solver = Bitdoku("0000000050201500300040070000100009200000008005087100"
                          "00309000006000006008007209000");
    SudokuVisualizer visualizer(540, solver);
    visualizer.run();
    return 0;
}