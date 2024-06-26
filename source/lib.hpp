#pragma once

#include <array>
#include <string>
#include <utility>

using bit_field = std::uint16_t;
constexpr int flat_board_size = 81;

struct Cell {
    mutable bit_field data;
    Cell() noexcept;

    // Initialize from a char with bitwise ops to set proper field
    Cell(char c) noexcept;

    // Initialize from a std::uint16_t
    Cell(bit_field bits) noexcept;

    // Overload the equality operator
    friend bool operator==(const Cell &lhs, const Cell &rhs);

    // Overload the bitwise OR operator
    Cell operator|(const Cell &other) const;

    // Overload the bitwise AND operator
    Cell operator&(const Cell &other) const;

    // Overload the bitwise OR assignment operator
    Cell &operator|=(const Cell &other);

    // Overload the assignment operator
    Cell &operator=(const Cell &other);

    auto is_set() const noexcept -> bool;

    // convert the data into a decimal char representation
    [[nodiscard]] constexpr char to_char() const noexcept;
};

class Bitdoku {
  public:
    Bitdoku() noexcept;

    // Constructor from a string representing the board
    Bitdoku(const std::string &board);
    const std::array<Cell, flat_board_size> get_board() const;

    auto set(int row, int col, int num) noexcept -> void;
    auto set(int index, bit_field value) noexcept -> void;

    // Check if a row, column, or block is valid
    // valid columns are 0-8, valid rows are 0-8, valid blocks are 0-8
    auto is_valid_row(const int index) const -> bool;
    auto is_valid_col(const int index) const -> bool;
    auto is_valid_block(const int index) const -> bool;

    // runs all three checks to determine if the board is valid
    auto is_valid() const -> bool;

    // relies on the behavior of get_row, get_col, get_block
    // to find possible values for cell
    // Row value    ~0b110111111 |
    // Column value ~0b111110111 |
    // Block value  ~0b111111111 |
    // Poss values   0b001001000
    auto set_possible(const std::size_t board_index) -> bool;
    auto find_empty_cell() const -> int;
    auto is_valid_move(const int board_index, const bit_field num_bit) const
        -> bool;
    auto solve() -> bool;
    auto to_string() const noexcept -> std::string;
    auto get_cell(const int index) const -> bit_field;
    auto print_board() const -> void;
    auto print_board_bits() const -> void;

  private:
    mutable std::array<Cell, flat_board_size> bitboards{};
    static constexpr bit_field expected_mask =
        0b0111111111; // Mask for bits 1-9 set

    static constexpr auto find_col_row(const int index) -> std::pair<int, int> {
        // looping is slightly faster than using integer division
        int row = 0;
        int col = index;
        while (col >= 9) {
            col -= 9;
            ++row;
        }
        return {col, row};
    }

    static constexpr auto index_row_col(const int row, const int col) -> int {
        return row * 9 + col; // Compute the flattened index
    }

    auto get_row(const int row) const -> bit_field;
    auto get_col(const int index) const -> bit_field;
    auto get_block(const int block_index) const -> bit_field;
};
