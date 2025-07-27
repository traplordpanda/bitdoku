# Set font path
set(USER_FONT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/resources/open-sans/OpenSans-Regular.ttf)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/bitdoku_gui.cpp @ONLY)


add_library(
    bitdoku_gui OBJECT
    source/bitdoku_gui.cpp
)
target_include_directories(
    bitdoku_gui ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)
target_compile_features(bitdoku_gui PRIVATE cxx_std_23)
find_package(SFML COMPONENTS graphics window system REQUIRED)
target_link_libraries(bitdoku_gui PUBLIC sfml-graphics sfml-window sfml-system bitdoku_lib) 

# GUI executable
add_executable(bgui source/gui_exe.cpp)
set_property(TARGET bgui PROPERTY OUTPUT_NAME gui)
target_compile_features(bgui PRIVATE cxx_std_23)
target_link_libraries(bgui PRIVATE bitdoku_gui bitdoku_lib)

# imgui executable
add_executable(imgui_exe source/igui.cpp)
set_property(TARGET imgui_exe PROPERTY OUTPUT_NAME igui)
target_compile_features(imgui_exe PRIVATE cxx_std_23)
find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(imgui_exe PRIVATE bitdoku_lib  imgui::imgui OpenGL::GL glfw module_std)

add_custom_command(
        TARGET bgui PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_LIST_DIR}/resources/open-sans/OpenSans-Regular.ttf
                ${CMAKE_BINARY_DIR}/OpenSans-Regular.ttf)

