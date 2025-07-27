# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(bitdokuTests LANGUAGES CXX C)

# ---- Dependencies ----

find_package(Catch2 REQUIRED)
include(Catch)


# ---- Tests ----

add_executable(bitdoku_test
    source/bitdoku_test.cpp
    source/jsolv.c
)
set_source_files_properties(source/jsolv.c PROPERTIES LANGUAGE C)

target_link_libraries(
    bitdoku_test PRIVATE
    bitdoku_lib
    Catch2::Catch2WithMain
)
target_compile_features(bitdoku_test PRIVATE cxx_std_20)
target_include_directories(bitdoku_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/source)


catch_discover_tests(bitdoku_test)

# ---- End-of-file commands ----

add_folders(Test)
