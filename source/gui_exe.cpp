#include <bitdoku_gui.hpp>
#include "bitdoku.hpp"
int main() {
    auto solver = Bitdoku("000000005020150030004007000010000920000000800508710000309000006000006008007209000");
    SudokuVisualizer visualizer(540, solver);
    visualizer.run();
    return 0;
}