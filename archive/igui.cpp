import bitdoku;
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <iostream>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#    include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) &&                                 \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#    pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description)
{
    printf("GLFW Error %d: %s\n", error, description);
}
static GLFWwindow *init_glfw_and_create_window(int width, int height,
                                               const char *title)
{
    if (!glfwInit()) {
        // Handle error, possibly throw or std::terminate
        std::cerr << "Failed to initialize GLFW.\n";
        return nullptr;
    }

    // Configure GLFW for desired OpenGL version; e.g. 3.3 Core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    GLFWwindow *window =
        glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return nullptr;
    }

    // Make the context current (important for glad or other loaders if you use
    // them)
    glfwMakeContextCurrent(window);

    // Enable vsync
    glfwSwapInterval(1);

    return window;
}

// Helper function: initialize ImGui context, style, and the GLFW/OpenGL3
// backends
static void init_imgui(GLFWwindow *window)
{
    // Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Optionally enable keyboard navigation, docking, etc.
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Set up Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize the ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    // Use GLSL version, e.g. "#version 330" for OpenGL 3.3
    ImGui_ImplOpenGL3_Init("#version 330");
}

// Helper function: shutdown ImGui gracefully
static void shutdown_imgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// Render function for our Sudoku board
auto render_board(const std::string &board) -> void
{
    ImGui::Begin("Sudoku Board");
    ImGui::SetWindowFontScale(2.0f);
    constexpr auto cellsize = 50.0f;
    if (ImGui::BeginTable("SudokuTable", 9, ImGuiTableFlags_Borders)) {
		for (int col = 0; col < 9; ++col) {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed,
                                        cellsize);
            }

        for (int row = 0; row < 9; ++row) {
            ImGui::TableNextRow();
            
        

            for (int col = 0; col < 9; ++col) {
                ImGui::TableSetColumnIndex(col);
                int idx = row * 9 + col;

                // Create a small buffer so ImGui::InputText can work (needs
                // null-terminated strings).
                char cellBuffer[2] = {board[idx], '\0'};

                // Use a unique ID so ImGui knows these are different widgets
                ImGui::PushID(idx);

                // Show the Sudoku digit in a single-character read-only field
                ImGui::InputText("", cellBuffer, sizeof(cellBuffer),
                                 ImGuiInputTextFlags_ReadOnly |
                                     ImGuiInputTextFlags_CharsDecimal);

                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

int main(int /*argc*/, char * /*argv*/[])
{
    // Initialize GLFW and create a window
    GLFWwindow *window =
        init_glfw_and_create_window(1080, 1920, "Sudoku with ImGui");
    if (!window) {
        return -1;
    }

    // Initialize ImGui context and backends
    init_imgui(window);

    // Example: create a board instance (Bitdoku is your Sudoku solver or
    // representation)
    auto board = Bitdoku("52000010008069007000600000000004601000490000"
                         "0270185000000800050008007940600004000");
    // Main loop

    while (!glfwWindowShouldClose(window)) {
        // Poll events
        for (auto i : board.step_solve()) {
            glfwPollEvents();

            // Start the ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render our Sudoku board
            auto board_str = board.to_string();
            render_board(board.to_string());
            // If your Bitdoku class has a "step_solve" or similar method:

            // Finalize the frame
            ImGui::Render();

            // Clear the screen
            glClear(GL_COLOR_BUFFER_BIT);

            // Render ImGui over OpenGL
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap buffers
            glfwSwapBuffers(window);
        }
    }
    // Cleanup
    shutdown_imgui();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
