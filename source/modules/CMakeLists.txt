# sources/modules/CMakeLists.txt
add_library(module_fmt)
add_library(module_std)

# Configure fmt module
target_sources(
    module_fmt
    PUBLIC
        FILE_SET CXX_MODULES FILES
        fmt/fmt.ixx
    PRIVATE
        fmt/fmt.hpp
)
target_include_directories(
    module_fmt
    PUBLIC
        ${PROJECT_SOURCE_DIR}/source/modules
)
target_include_directories(
    module_fmt ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/modules/fmt>"
    "$<INSTALL_INTERFACE:include>"
)
find_package(fmt REQUIRED)
target_link_libraries(module_fmt PRIVATE fmt::fmt)

#configure std module
target_sources(
    module_std
    PUBLIC
        FILE_SET CXX_MODULES FILES
        stl/stl.ixx
    PRIVATE
        stl/stl.hpp
)
target_include_directories(
	module_std ${warning_guard}
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/modules/stl>"
		"$<INSTALL_INTERFACE:include>"
)
