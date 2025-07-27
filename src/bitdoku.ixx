module;

#include <algorithm>
#include <array>
#include <cstdint>
#include <future>
#include <numeric>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>
// #include <fmt/format.h>

export module bitdoku;

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
export using bit_field = std::uint16_t;
export constexpr int flat_board_size = 81;

constexpr bit_field value_mask = 0b111111111;
constexpr bit_field set_mask = static_cast<bit_field>((1 << 9));

export constexpr auto find_col_row(const int index) -> std::pair<int, int> {
  int row = 0;
  int col = index;
  while (col >= 9) {
    col -= 9;
    ++row;
  }
  return {col, row};
}

export constexpr auto index_row_col(const int row, const int col) -> int {
  return row * 9 + col; // Compute the flattened index
}

constexpr auto clear_9bit(const bit_field value) noexcept -> bit_field {
  constexpr auto clear_mask = static_cast<bit_field>(~(1 << 9));
  return value & clear_mask;
}

constexpr auto set_9bit(const bit_field value) noexcept -> bit_field {
  return value | set_mask;
}

constexpr auto set_or_clear_9bit(const bit_field value) noexcept -> bit_field {
  if (std::has_single_bit(value)) {
    return set_9bit(value);
  }
  return clear_9bit(value);
}

constexpr auto row_length = 9;
export struct Cell {
  bit_field data;
  Cell() noexcept : data(0) {}

  // Initialize from a char with bitwise ops to set proper field
  Cell(char c) noexcept
      : data(set_9bit(static_cast<bit_field>(1 << (c - '0' - 1)))) {}

  // Initialize from a std::uint16_t
  Cell(bit_field bits) noexcept : data(set_or_clear_9bit(bits)) {}

  // convert the data into a decimal char representation
  [[nodiscard]] auto to_char() const noexcept -> char {
    if (data == 0 || (data & set_mask) == 0) {
      return '0';
    }
    int bit_position = std::countr_zero(data);
    return static_cast<char>('0' + bit_position + 1);
  }

  // Overload the equality operator
  friend auto operator==(const Cell &lhs, const Cell &rhs) -> bool {
    return lhs.data == rhs.data;
  }

  // Overload the bitwise OR operator
  auto operator|(const Cell &other) const -> Cell {
    return {static_cast<bit_field>(data | other.data)};
  }

  // Overload the bitwise AND operator
  auto operator&(const Cell &other) const -> Cell {
    return {static_cast<bit_field>(data & other.data)};
  }

  // Overload the bitwise OR assignment operator
  auto operator|=(const Cell &other) -> Cell & {
    data |= other.data;
    return *this;
  }

  // Overload the assignment operator
  auto operator=(const Cell &other) -> Cell & {
    if (this != &other) {
      data = other.data;
    }
    return *this;
  }

  auto is_set() const noexcept -> bool { return (data & set_mask) != 0; }

  friend auto operator<<(std::ostream &os, const Cell &obj) -> std::ostream & {
    os << obj.to_char();
    return os;
  }
};

using Bitboard = std::array<Cell, flat_board_size>;

export template <class T>
concept StringLike81 = requires(T str) {
  { str.size() } -> std::same_as<std::size_t>;
  { str[0] } -> std::same_as<char>;
  { str[80] } -> std::same_as<char>;
};

export class Bitdoku {
public:
  Bitdoku() noexcept = default;

  // Constructor from a string representing the board
  explicit Bitdoku(const std::string &board) {
    for (int i = 0; i < flat_board_size; ++i) {
      if (board[i] >= '1' && board[i] <= '9') { // '0' represents an empty cell
        bitboards[i] = board[i];
      }
    }
  }

  // equal operator overload for Bitdoku
  friend auto operator==(const Bitdoku &lhs, const Bitdoku &rhs) -> bool {
    return lhs.bitboards == rhs.bitboards;
  }

  auto get_board() const -> std::array<Cell, flat_board_size> {
    return bitboards;
  }

  auto set(int row, int col, int num) noexcept -> void {
    bitboards[index_row_col(row, col)] = static_cast<bit_field>(1 << num);
  }

  auto set(int index, bit_field value) noexcept -> void {
    bitboards[index] = value;
  }

  // Check if a row, column, or block is valid
  // valid columns are 0-8, valid rows are 0-8, valid blocks are 0-8
  auto is_valid_row(const int index) const -> bool {
    auto possible = get_row(index);
    return (possible ^ expected_mask) == 0;
  }

  auto is_valid_col(const int index) const -> bool {
    auto possible = get_col(index);
    return (possible ^ expected_mask) == 0;
  }

  auto is_valid_block(const int index) const -> bool {
    auto possible = get_block(index);
    return (possible ^ expected_mask) == 0;
  }

  // runs all three checks to determine if the board is valid
  auto is_valid() const -> bool {
    for (int i = 0; i < 9; i++) {
      if (!is_valid_row(i) || !is_valid_col(i) || !is_valid_block(i)) {
        return false;
      }
    }
    return true;
  }

  // relies on the behavior of get_row, get_col, get_block
  // to find possible values for cell
  // Row value    ~0b110111111 |
  // Column value ~0b111110111 |
  // Block value  ~0b111111111 |
  // Poss values   0b001001000
  auto set_possible(const int board_index) -> bool {
    // early return if already set
    if (bitboards[board_index].is_set()) {
      return true;
    }

    const auto [col, row] = find_col_row(board_index);
    const auto block = (row / 3) * 3 + (col / 3);
    auto used = (get_row(row) | get_col(col) | get_block(block));
    const bit_field possible = ~used & expected_mask;

    if (possible == 0) {
      return false; // No possible values, invalid state
    }

    // board will be set with the possible values
    bitboards[board_index] = possible;
    return true;
  }

  auto get_possible(const int board_index) -> bit_field {
    if (bitboards[board_index].is_set()) {
      return bitboards[board_index].data;
    }
    const auto [col, row] = find_col_row(board_index);
    const auto block = (row / 3) * 3 + (col / 3);
    bit_field used = get_row(row) | get_col(col) | get_block(block);
    bit_field possible = ~used & expected_mask;
    return possible;
  }

  auto find_empty_cell() const -> int {
    for (int i = 0; i < flat_board_size; i++) {
      if (!bitboards[i].is_set()) {
        return i;
      }
    }
    return -1;
  }

  auto is_valid_move(const int board_index,
                     const bit_field num_bit) const -> bool {
    const auto [col, row] = find_col_row(board_index);
    const auto block = (row / 3) * 3 + (col / 3);

    bit_field used = get_row(row) | get_col(col) | get_block(block);
    return !(used & num_bit);
  }

  auto solve() -> bool {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
      return true; // All cells are filled, puzzle is solved
    }

    for (int num = 1; num <= 9; ++num) {
      const bit_field num_bit = 1 << (num - 1);
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

  // auto step_solve() -> cppcoro::recursive_generator<bool>;
  auto solve_parallel(int num_threads = static_cast<int>(
                          std::thread::hardware_concurrency())) -> bool {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
      return true; // All cells are filled, puzzle is solved
    }

    std::vector<std::future<std::optional<Bitdoku>>> futures;
    futures.reserve(9);

    for (int num = 1; num <= 9; ++num) {
      bit_field num_bit = 1 << (num - 1);
      if (is_valid_move(empty_cell, num_bit)) {
        futures.push_back(
            std::async(std::launch::async, [this, empty_cell, num_bit]() {
              Bitdoku board_copy = *this; // Thread-safe copy constructor call
              board_copy.set(empty_cell, num_bit);
              if (board_copy.set_possible(empty_cell)) {
                if (board_copy.solve()) {
                  return std::optional<Bitdoku>{board_copy};
                }
              }
              return std::optional<Bitdoku>{};
            }));

        if (futures.size() >= static_cast<std::size_t>(num_threads)) {
          break;
        }
      }
    }

    for (auto &future : futures) {
      auto result = future.get();
      if (result) {
        *this = *result; // Update this board with the solution
        return true;
      }
    }

    return false;
  }

  auto to_string() const noexcept -> std::string {
    std::string board;
    board.reserve(flat_board_size);
    for (auto &cell : bitboards) {
      board.push_back(cell.to_char());
    }
    return board;
  }

  auto get_cell(const int index) const -> bit_field {
    return bitboards[index].data;
  }

  auto print_board() const -> void {
    for (std::size_t i = 0; i < flat_board_size; i++) {
      std::print("{:b} ", bitboards[i].data);
      if ((i + 1) % 9 == 0) {
        std::print("\n");
      }
    }
  }

  auto print_board_bits() const -> void {
    for (std::size_t i = 0; i < flat_board_size; i++) {
      std::print("{:b} ", bitboards[i].data);
      if ((i + 1) % 9 == 0) {
        std::print("\n");
      }
    }
  }

  auto count_solutions(int count) -> int {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
      return count + 1;
    }

    for (int num = 1; num <= 9 && count < 2; ++num) {
      const bit_field num_bit = 1 << (num - 1);
      if (is_valid_move(empty_cell, num_bit)) {
        set(empty_cell, num_bit);
        count = count_solutions(count);
      }
    }

    set(empty_cell, 0); // Backtrack
    return count;
  }

  auto fill_random(std::mt19937 &mt) -> bool {
    int empty_cell = find_empty_cell();
    if (empty_cell == -1) {
      return true; // All cells are filled
    }

    std::array<int, 9> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::shuffle(numbers.begin(), numbers.end(), mt);

    for (int num : numbers) {
      const bit_field num_bit = 1 << (num - 1);
      if (is_valid_move(empty_cell, num_bit)) {
        set(empty_cell, num_bit);
        if (fill_random(mt)) {
          return true;
        }
        set(empty_cell, 0); // Backtrack
      }
    }
    return false;
  }

private:
  mutable Bitboard bitboards{};
  static constexpr bit_field expected_mask =
      0b0111111111; // Mask for bits 1-9 set

  auto get_row(const int row) const -> bit_field {
    auto temp = Cell();
    const auto start = row * 9; // Start at the beginning of the row
    for (int i = start; i < start + 9; ++i) {
      if (bitboards[i].is_set()) {
        temp |= bitboards[i];
      }
    }
    return temp.data;
  }

  auto get_col(const int index) const -> bit_field {
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

  auto get_block(const int block_index) const -> bit_field {
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
};

export class BitdokuGenerator {
public:
  BitdokuGenerator(std::optional<unsigned int> seed = std::nullopt)
      : mt(seed.value_or(rd())) {
    board.fill_random(mt);
  }

  auto get_board() -> const Bitdoku & { return board; }

  auto generate(int empty_cells = 45) -> Bitdoku {
    make_puzzle(board, empty_cells);
    return board;
  }

  auto generate_with_permutation(int empty_cells = 45) -> Bitdoku {
    make_puzzle<true>(board, empty_cells);
    return board;
  }

private:
  void permute_rows(Bitdoku &board) {
    std::uniform_int_distribution<int> block_dist(0, 2);
    int block_row = block_dist(mt); // 0, 1, or 2 for the block row

    std::uniform_int_distribution<int> row_dist(0, 2);
    int row1_offset = row_dist(mt);
    int row2_offset;
    do {
      row2_offset = row_dist(mt);
    } while (row1_offset == row2_offset);

    int row1 = block_row * 3 + row1_offset;
    int row2 = block_row * 3 + row2_offset;

    for (int col = 0; col < 9; ++col) {
      int index1 = index_row_col(row1, col);
      int index2 = index_row_col(row2, col);
      bit_field val1 = board.get_cell(index1);
      bit_field val2 = board.get_cell(index2);
      board.set(index1, val2);
      board.set(index2, val1);
    }
  }

  template <bool use_permutation = false>
  void make_puzzle(Bitdoku &board, int empty_cells) {
    std::vector<int> indices(flat_board_size);
    std::ranges::iota(indices, 0);
    std::shuffle(indices.begin(), indices.end(), mt);

    int removed_count = 0;
    for (int i = 0; i < flat_board_size && removed_count < empty_cells; ++i) {
      if constexpr (use_permutation) {
        permute_rows(board);
      }
      int cell_index = indices[i];
      if (board.get_cell(cell_index) == 0)
        continue;

      Bitdoku temp_board = board; // Create a copy to check for solutions
      temp_board.set(cell_index, 0);
      auto solution_count = temp_board.count_solutions(0);
      if (solution_count == 1) {
        board.set(cell_index, 0);
        removed_count++;
      } else {
        return;
      }
    }
  }
  Bitdoku board;
  std::random_device rd;
  std::mt19937 mt;
};
