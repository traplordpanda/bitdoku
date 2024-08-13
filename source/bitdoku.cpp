#include <bit>
#include <cppcoro/recursive_generator.hpp>
#include <fmt/core.h>
#include <stdexcept>
#include <string>

module bitdoku;

/*
   Bitfield is set up to track sudoku values as well
   as possible values in a 0 index.
   The first 9 bits are used to hold information.
   If a single bit 0-8 is set then the value is known.
   When the value is known the 9th bit is set to cache this information.
   Otherwise the information is regarded as possible values.
   for example  0b0010 0001 0000 is a 5 that is set
                                0b0000 0001 0000 is a 5 that is not set
*/
constexpr bit_field value_mask = 0b111111111;
constexpr bit_field set_mask = static_cast<bit_field>((1 << 9));

constexpr auto set_9bit(const bit_field value) noexcept -> bit_field {
    return value | set_mask;
}

constexpr bit_field clear_9bit(const bit_field value) noexcept {
    constexpr bit_field clear_mask = static_cast<bit_field>(~(1 << 9));
    return value & clear_mask;
}

constexpr auto set_or_clear_9bit(const bit_field value) noexcept -> bit_field {
    if (std::has_single_bit(value) == true) {
        return set_9bit(value);
    }
    return clear_9bit(value);
}

Cell::Cell() noexcept : data(0) {}

Cell::Cell(char c) noexcept
    : data(set_9bit(static_cast<bit_field>(1 << (c - '0' - 1)))) {}

Cell::Cell(bit_field bits) noexcept : data(set_or_clear_9bit(bits)) {}

auto Cell::to_char() const noexcept -> const char {
    if (data == 0 || (data & set_mask) == 0) {
        return '0';
    }
    int bit_position = std::countr_zero(data);
    return static_cast<char>('0' + bit_position + 1);
}

bool operator==(const Cell &lhs, const Cell &rhs) {
    return lhs.data == rhs.data;
}

Cell Cell::operator|(const Cell &other) const {
    return Cell(static_cast<bit_field>(data | other.data));
}

Cell Cell::operator&(const Cell &other) const {
    return Cell(static_cast<bit_field>(data & other.data));
}

Cell &Cell::operator|=(const Cell &other) {
    data |= other.data;
    return *this;
}

Cell &Cell::operator=(const Cell &other) {
    if (this != &other) {
        data = other.data;
    }
    return *this;
}

auto Cell::is_set() const noexcept -> bool { return data & set_mask; }

Bitdoku::Bitdoku() noexcept {};

Bitdoku::Bitdoku(const std::string &board) {
    if (board.size() != flat_board_size) {
        throw std::runtime_error(
            "Invalid board size. Must be exactly 81 characters.");
    }
    for (int i = 0; i < flat_board_size; ++i) {
        if (board[i] >= '1' &&
            board[i] <= '9') { // '0' represents an empty cell
            bitboards[i] = board[i];
        }
    }
}

auto Bitdoku::get_board() const -> const std::array<Cell, flat_board_size> {
    return bitboards;
}

auto Bitdoku::set(int row, int col, int num) noexcept -> void {
    bitboards[index_row_col(row, col)] = static_cast<bit_field>(1 << num);
}

auto Bitdoku::set(int index, bit_field value) noexcept -> void {
    bitboards[index] = value;
}

auto Bitdoku::is_valid_row(const int index) const -> bool {
    auto possible = get_row(index);
    return !(possible ^ expected_mask);
}

auto Bitdoku::is_valid_col(const int index) const -> bool {
    auto possible = get_col(index);
    return !(possible ^ expected_mask);
}

auto Bitdoku::is_valid_block(const int index) const -> bool {
    auto possible = get_block(index);
    return !(possible ^ expected_mask);
}

auto Bitdoku::is_valid() const -> bool {
    for (int i = 0; i < 9; i++) {
        if (!is_valid_row(i) || !is_valid_col(i) || !is_valid_block(i)) {
            return false;
        }
    }
    return true;
}

auto Bitdoku::set_possible(const std::size_t board_index) -> bool {
    // early return if already set
    if (bitboards[board_index].is_set()) {
        return true;
    }

    const auto [col, row] = find_col_row(board_index);
    const auto block = (row / 3) * 3 + (col / 3);
    bit_field used = get_row(row) | get_col(col) | get_block(block);
    bit_field possible = ~used & expected_mask;

    if (possible == 0) {
        return false; // No possible values, invalid state
    }

    // board will be set with the possible values
    bitboards[board_index] = possible;
    return true;
}

auto Bitdoku::find_empty_cell() const -> int {
    for (int i = 0; i < flat_board_size; i++) {
        if (!bitboards[i].is_set()) {
            return i;
        }
    }
    return -1;
}

auto Bitdoku::is_valid_move(const int board_index,
                            const bit_field num_bit) const -> bool {
    const auto [col, row] = find_col_row(board_index);
    const auto block = (row / 3) * 3 + (col / 3);

    bit_field used = get_row(row) | get_col(col) | get_block(block);
    return !(used & num_bit);
}

auto Bitdoku::solve() -> bool {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
        return true; // All cells are filled, puzzle is solved
    }

    for (int num = 1; num <= 9; ++num) {
        bit_field num_bit = 1 << (num - 1);
        if (is_valid_move(empty_cell, num_bit)) {
            set(empty_cell, num_bit);

            if (set_possible(empty_cell) && solve()) {
                return true;
            }

            // Backtrack
            set(empty_cell, 0);
        }
    }

    return false; // No valid solution found
}
auto Bitdoku::step_solve() -> cppcoro::recursive_generator<bool> {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
        co_yield true; // All cells are filled, puzzle is solved
        co_return;
    }

    for (int num = 1; num <= 9; ++num) {
        bit_field num_bit = 1 << (num - 1);
        if (is_valid_move(empty_cell, num_bit)) {
            set(empty_cell, num_bit);
            if (set_possible(empty_cell)) {
                auto sub_solve = step_solve();
                bool sub_result = false;
                for (bool solved : sub_solve) {
                    sub_result = solved;
                    co_yield solved; // Propagate intermediate results
                    if (solved)
                        break;
                }
                if (sub_result) {
                    co_return; // Solution found, no need to backtrack
                }
            }
            // Backtrack
            set(empty_cell, 0);
        }
    }
    co_yield false; // No valid solution found for this cell
}

auto Bitdoku::print_board_bits() const -> void {
    for (std::size_t i = 0; i < flat_board_size; i++) {
        fmt::print("{:b} ", bitboards[i].data);
        if ((i + 1) % 9 == 0) {
            fmt::print("\n");
        }
    }
}

auto Bitdoku::to_string() const noexcept -> std::string {
    std::string board;
    board.reserve(flat_board_size);
    for (auto &cell : bitboards) {
        board.push_back(cell.to_char());
    }
    return board;
};

auto Bitdoku::get_cell(const int index) const -> bit_field {
    return bitboards[index].data;
}

auto Bitdoku::print_board() const -> void {
    auto board = to_string();

    fmt::print("{} ", board[0]);
    for (int i = 0; i < board.size(); i++) {
        if (i != 0) {
            if ((i + 1) % 27 == 0) {
                fmt::print("{}\n\n", board[i]);
            } else if ((i + 1) % 9 == 0) {
                fmt::print("{}\n", board[i]);
            } else if ((i + 1) % 3 == 0) {
                fmt::print("{}  ", board[i]);
            } else if ((i + 1) % 27 == 0) {
                fmt::print("{}  ", board[i]);
            } else {
                fmt::print("{} ", board[i]);
            }
        }
    }
}

auto Bitdoku::get_row(const int row) const -> bit_field {
    auto temp = Cell();
    int start = row * 9; // Start at the beginning of the row
    for (int i = start; i < start + 9; ++i) {
        if (bitboards[i].is_set()) {
            temp |= bitboards[i];
        }
    }
    return temp.data;
}

auto Bitdoku::get_col(const int index) const -> bit_field {
    auto temp = Cell();
    const auto stop = index + 73;
    for (int i = index; i < stop;) {
        if (bitboards[i].is_set()) {
            temp |= bitboards[i];
        }
        i += 9;
    }
    return temp.data;
}

auto Bitdoku::get_block(const int block_index) const -> bit_field {
    const int start_row = (block_index / 3) * 3;
    const int start_col = (block_index % 3) * 3;
    auto temp = Cell();
    for (int row = start_row; row < start_row + 3; ++row) {
        for (int col = start_col; col < start_col + 3; ++col) {
            auto idx = index_row_col(row, col);
            if (bitboards[idx].is_set()) {
                temp |= bitboards[idx];
            }
        }
    }
    return temp.data;
}
