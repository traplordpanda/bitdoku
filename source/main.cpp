import bitdoku;

#include <fmt/core.h>
#include <iostream>
#include <string>

Bitdoku back_track(const std::string &board) {
    Bitdoku sboard{board};
    fmt::print("\ninitial board\n");
    fmt::print("{}\n", board);
    sboard.print_board();
    for (int i = 0; i < flat_board_size; i++) {
        sboard.set_possible(i);
    }
    auto iterations = 0;
    for (auto i : sboard.step_solve()) {
        fmt::print("{}\n", iterations++);
    }

    fmt::print("---------------------\n\n");
    fmt::print("{}\n", sboard.to_string());
    sboard.print_board();
    return sboard;
}

void debug_board(const Bitdoku &sboard) {
    for (int i = 0; i < 9; i++) {
        if (sboard.is_valid_row(i)) {
            fmt::print("row {} is valid\n", i);
        }
    }
    for (int i = 0; i < 9; i++) {
        if (sboard.is_valid_col(i)) {
            fmt::print("col {} is valid\n", i);
        }
    }
    for (int i = 0; i < 9; i++) {
        if (sboard.is_valid_block(i)) {
            fmt::print("block {} is valid\n", i);
        }
    }
    if (sboard.is_valid()) {
        fmt::print("board is valid\n");
    }
}

// main function that takes a string representing a sudoku board
// and solves it using the backtracking algorithm

int main(int argc, char *argv[]) {
    Cell testCell('5');
    std::cout << "Testing cell output: " << testCell.to_char() << std::endl;
    const std::string board{"381624957"
                            "694857213"
                            "527319684"
                            "269471835"
                            "148563792"
                            "753982461"
                            "415798326"
                            "846245179"
                            "972136548"};
    const std::string board2{"001624007"
                             "690007013"
                             "500010080"
                             "069001800"
                             "040060790"
                             "050082400"
                             "010700026"
                             "806200079"
                             "002130008"};
    const std::string board3{"000000020"
                             "105004006"
                             "000006900"
                             "604000070"
                             "020000080"
                             "503060000"
                             "040700100"
                             "200003040"
                             "007020003"};

    const std::string board4{
        "0000000500006307100020003061000006000004000299087005000009070000150080"
        "00000000067"};

    if (argc != 2) { // Expect exactly one additional argument: the board string
        std::cerr << "Usage: " << argv[0] << " <sudoku_board_string>\n";
        return 1; // Return with error code
    }

    std::string board_in = argv[1]; // Get the board string from command line
    if (board_in.length() !=
        81) { // Validate that the input string has exactly 81 characters
        std::cerr
            << "Error: The Sudoku board must consist of exactly 81 digits "
               "(0-9).\n";
        return 1; // Return with error code
    }

    // Initialize the Sudoku board with the provided input
    auto solvedBoard = back_track(board_in);
    debug_board(
        solvedBoard); // Display the solved board and other debug information

    return 0; // Successful execution
}