#include "bitdoku_gui.hpp"
#include "bitdoku.hpp"
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

using namespace std::string_view_literals;
namespace fs = std::filesystem;
static constexpr auto f_path = "C:\\Users\\622807\\source\\repos\\bitdoku\\resources\\open-sans\\OpenSans-Light.ttf"sv;

SudokuVisualizer::SudokuVisualizer(int windowSize, Bitdoku& solver)
    : window(sf::VideoMode(windowSize, windowSize), "Sudoku Solver"),
    solver(solver),
    cellSize(windowSize / BOARD_SIZE),
    isSolving(true),
    font()
{
    window.setFramerateLimit(60);
    if (!font.loadFromFile(std::string(f_path))) {
        std::abort();
    }
}

void SudokuVisualizer::run() {
    while (window.isOpen()) {
        for (auto&& state : solver.step_solve()) {
            handleEvents();
            draw();
            //std::this_thread::sleep_for(std::chrono::microseconds(100));
            if (state == true) { break; };
            if (!window.isOpen()) { break; };
        }

        handleEvents();
        draw();
    }
}

void SudokuVisualizer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}


void SudokuVisualizer::draw() {
    window.clear(sf::Color::White);
    drawGrid();
    drawNumbers();
    window.display();
}

void SudokuVisualizer::drawGrid() {
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        sf::Vertex hLine[] = {
            sf::Vertex(sf::Vector2f(0, i * cellSize)),
            sf::Vertex(sf::Vector2f(window.getSize().x, i * cellSize))
        };
        sf::Vertex vLine[] = {
            sf::Vertex(sf::Vector2f(i * cellSize, 0)),
            sf::Vertex(sf::Vector2f(i * cellSize, window.getSize().y))
        };

        if (i % 3 == 0) {
            hLine[0].color = sf::Color::Black;
            hLine[1].color = sf::Color::Black;
            vLine[0].color = sf::Color::Black;
            vLine[1].color = sf::Color::Black;
        }
        else {
            hLine[0].color = sf::Color(200, 200, 200);
            hLine[1].color = sf::Color(200, 200, 200);
            vLine[0].color = sf::Color(200, 200, 200);
            vLine[1].color = sf::Color(200, 200, 200);
        }

        window.draw(hLine, 2, sf::Lines);
        window.draw(vLine, 2, sf::Lines);
    }
}

void SudokuVisualizer::drawNumbers() {
    const auto& board = solver.get_board();
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i) {
        int row = i / BOARD_SIZE;
        int col = i % BOARD_SIZE;

        char cellValue = board[i].to_char();
        if (cellValue != '0') {
            sf::Text text(cellValue, font, cellSize / 2);
            text.setPosition(col * cellSize + cellSize / 4, row * cellSize + cellSize / 4);
            text.setFillColor(sf::Color::Black);
            window.draw(text);
        }
    }
}

void SudokuVisualizer::loadBoard(const std::string& boardString) {
    solver = Bitdoku(boardString);
}

