#include <iostream>

import bitdoku;

Bitdoku back_track(const std::string &board) {
  Bitdoku sboard{board};
  std::cout << "\ninitial board\n";
  std::cout << board << '\n';
  sboard.print_board();
  for (int i = 0; i < flat_board_size; i++) {
    sboard.set_possible(i);
  }

  std::cout << "---------------------\n\n";
  std::cout << sboard.to_string() << '\n';
  sboard.print_board();
  return sboard;
}

void debug_board(const Bitdoku &sboard) {
  for (int i = 0; i < 9; i++) {
    if (sboard.is_valid_row(i)) {
      std::cout << "row " << i << " is valid\n";
    }
  }
  for (int i = 0; i < 9; i++) {
    if (sboard.is_valid_col(i)) {
      std::cout << "col " << i << " is valid\n";
    }
  }
  for (int i = 0; i < 9; i++) {
    if (sboard.is_valid_block(i)) {
      std::cout << "block " << i << " is valid\n";
    }
  }
  if (sboard.is_valid()) {
    std::cout << "board is valid\n";
  }
}

// main function that takes a string representing a sudoku board
// and solves it using the backtracking algorithm

int main(int argc, char *argv[]) {
  BitdokuGenerator generator;
  auto board = generator.get_board();
  std::cout << "Initial Board:\n";
  board.print_board();
  std::cout << board.to_string() << '\n';
  generator.generate_with_permutation();
  std::cout << "Generated Puzzle with perms:\n";
  auto new_board = generator.get_board();
  new_board.print_board();
  std::cout << new_board.to_string() << '\n';
  return 0;
}