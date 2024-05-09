#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include "bitdoku.hpp"

TEST_CASE("Solver", "[library]")
{
  auto board = Bitdoku {
      "060010004070050000940000006000400002000800010001072600000020000690000080"
      "080700030"};
  REQUIRE(board.to_string() == "060010004070050000940000006000400002000800010001072600000020000690000080080700030");
  auto solved = board.solve();
  REQUIRE(solved == true);
  REQUIRE(board.to_string() == "568219374173654928942387156756431892429865713831972645317528469695143287284796531");
  board = Bitdoku {
      "062000300000000080001506000093000004000900000000001260906080000000004670"
      "050000020"};
  solved = board.solve();
  REQUIRE(solved == true);
  REQUIRE(board.to_string() == "762849351549312786381576492293657814614928537875431269926785143138294675457163928");
}

TEST_CASE("Cell", "[library]")
{
  auto cell = Cell('9');
  SECTION("cell is set and data is correct")
  {
    REQUIRE(cell.is_set() == true);
    REQUIRE(cell.data == 0b1100000000);
    REQUIRE(cell.to_char() == '9');
  }
  SECTION("Cell assignment overload is correct")
  {
    cell = static_cast<bit_field>(0b100000000);
    REQUIRE(cell.is_set() == true);
    REQUIRE(cell.data == 0b1100000000);
  }
  SECTION("Cell & overload is correct")
  {
    auto cell8 = Cell('8');  // 0b1010000000
    auto cell9 = Cell('9');  // 0b1100000000
    auto and_cell = cell8 & cell9;
    REQUIRE(and_cell.data == 0b1000000000);
  }
  SECTION("Cell |= overload is correct")
  {
    auto cell8 = Cell('8');  // 0b1010000000
    auto cell9 = Cell('9');  // 0b1100000000
    auto or_cell = cell8 | cell9;
    REQUIRE(or_cell.data == 0b110000000);
  }
}

TEST_CASE("Cell and Board", "[library]")
{
  std::array<Cell, 81> testboard {};
  const std::string sboard {
      "000541000006070008090008000560020010002000400000007000080305009000010006"
      "035000000"};
  for (size_t i = 0; i < 81; i++) {
    testboard[i] = Cell(sboard[i]);
  }
  auto board = Bitdoku {sboard};
  const auto bitboard = board.get_board();
  REQUIRE(testboard == bitboard);
}
