import bitdoku;

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("Solver", "[library]") {
    auto board = Bitdoku{"06001000407005000094000000600040000200080001000107260"
                         "0000020000690000080"
                         "080700030"};
    REQUIRE(board.to_string() == "060010004070050000940000006000400002000800010"
                                 "001072600000020000690000080080700030");
    auto solved = board.solve();
    REQUIRE(solved == true);
    REQUIRE(board.to_string() == "568219374173654928942387156756431892429865713"
                                 "831972645317528469695143287284796531");
    board = Bitdoku{"0620003000000000800015060000930000040009000000000012609060"
                    "80000000004670"
                    "050000020"};
    solved = board.solve();
    REQUIRE(solved == true);
    REQUIRE(board.to_string() == "762849351549312786381576492293657814614928537"
                                 "875431269926785143138294675457163928");
}

TEST_CASE("Cell", "[library]") {
    auto cell = Cell('9');
    SECTION("cell is set and data is correct") {
        REQUIRE(cell.is_set() == true);
        REQUIRE(cell.data == 0b1100000000);
        REQUIRE(cell.to_char() == '9');
    }
    SECTION("Cell assignment overload is correct") {
        cell = static_cast<bit_field>(0b100000000);
        REQUIRE(cell.is_set() == true);
        REQUIRE(cell.data == 0b1100000000);
    }
    SECTION("Cell & overload is correct") {
        auto cell8 = Cell('8'); // 0b1010000000
        auto cell9 = Cell('9'); // 0b1100000000
        auto and_cell = cell8 & cell9;
        REQUIRE(and_cell.data == 0b1000000000);
    }
    SECTION("Cell |= overload is correct") {
        auto cell8 = Cell('8'); // 0b1010000000
        auto cell9 = Cell('9'); // 0b1100000000
        auto or_cell = cell8 | cell9;
        REQUIRE(or_cell.data == 0b110000000);
    }
}

TEST_CASE("Cell and Board", "[library]") {
    std::array<Cell, 81> testboard{};
    const std::string sboard{"0005410000060700080900080005600200100020004000000"
                             "07000080305009000010006"
                             "035000000"};
    for (size_t i = 0; i < 81; i++) {
        if (not(sboard[i] == '0')) {
            testboard[i] = Cell(sboard[i]);
        }
    }
    auto board = Bitdoku{sboard};
    auto bitboard = board.get_board();
    // REQUIRE(testboard == bitboard);
}

TEST_CASE("StepSolver", "[library]") {
    auto board = Bitdoku{"06001000407005000094000000600040000200080001000107260"
                         "0000020000690000080"
                         "080700030"};
    REQUIRE(board.to_string() == "060010004070050000940000006000400002000800010"
                                 "001072600000020000690000080080700030");
    auto solved = false;
    for (auto &&it : board.step_solve()) {
        solved = it;
    }
    REQUIRE(solved == true);
    REQUIRE(board.to_string() == "568219374173654928942387156756431892429865713"
                                 "831972645317528469695143287284796531");
}

TEST_CASE("ParelleSolver", "[library]") {
    auto board = Bitdoku{"06001000407005000094000000600040000200080001000107260"
                         "0000020000690000080"
                         "080700030"};
    REQUIRE(board.to_string() == "060010004070050000940000006000400002000800010"
                                 "001072600000020000690000080080700030");
    auto solved = board.solve_parallel();
    REQUIRE(solved == true);
    REQUIRE(board.to_string() == "568219374173654928942387156756431892429865713"
                                 "831972645317528469695143287284796531");
}
TEST_CASE("BENCHMARKING", "[!benchmark]") {
    BENCHMARK("recursive_solve") {
        auto board = Bitdoku{ "060010004070050000940000006000400002000800010001072600000020000690000080080700030" };
        board.solve();
        return board.to_string();
    };

    BENCHMARK("coroutine_solve") {
        auto board = Bitdoku{ "060010004070050000940000006000400002000800010001072600000020000690000080080700030" };
        for (auto&& it : board.step_solve()) {
            if (it) {
                break;
            }
        }
        return board.to_string();
    };

    BENCHMARK("parallel_solve") {
		auto board = Bitdoku{ "060010004070050000940000006000400002000800010001072600000020000690000080080700030" };
		board.solve_parallel();
		return board.to_string();
	};
	BENCHMARK("parallel_solve_8") {
		auto board = Bitdoku{ "060010004070050000940000006000400002000800010001072600000020000690000080080700030" };
		board.solve_parallel(8);
		return board.to_string();
	};
}

CATCH_REGISTER_REPORTER("console", Catch::ConsoleReporter)



