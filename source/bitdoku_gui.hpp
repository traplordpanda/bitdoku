// SudokuVisualizer.hpp
import bitdoku;

#include <SFML/Graphics.hpp>
#include <string>

class SudokuVisualizer {
  public:
    SudokuVisualizer(int windowSize, Bitdoku &solver);
    void run();

  private:
    void handleEvents();
    void update();
    void draw();
    void drawGrid();
    void drawNumbers();
    void loadBoard(const std::string &boardString);

    sf::RenderWindow window;
    Bitdoku solver;
    sf::Font font;
    int cellSize;
    bool isSolving;

    static constexpr int BOARD_SIZE = 9;
};
