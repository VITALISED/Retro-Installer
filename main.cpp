// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <curl/curl.h>
#include <iostream>
#include <windowsx.h>

#include "fonts.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_TITLEBAR, GLFW_FALSE);

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

#endif

    int width = 1140; // could declare them as "const int" if you like
    int height = 680;
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(width, height, "Retro NMS Installer", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glfwSetWindowMonitor(window, NULL, (mode->width / 2) - (width / 2), (mode->height / 2) - (height / 2), width, height, GLFW_DONT_CARE);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    ImFont *nms_font = io.Fonts->AddFontFromMemoryTTF((void *)NMSGeoSans_Kerned, sizeof(NMSGeoSans_Kerned), 13, &font_cfg);
    ImFont *nms_font_big = io.Fonts->AddFontFromMemoryTTF((void *)NMSGeoSans_Kerned, sizeof(NMSGeoSans_Kerned), 35, &font_cfg);
    ImFont *nms_font_medium = io.Fonts->AddFontFromMemoryTTF((void *)NMSGeoSans_Kerned, sizeof(NMSGeoSans_Kerned), 20, &font_cfg);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
    style.DisplayWindowPadding = ImVec2(0, 0);
    style.FrameBorderSize = 0.05;
    style.FramePadding = ImVec2(10, 2.5);
    style.FrameRounding = 5.f;
    style.GrabRounding = 3.f;
    style.ChildRounding = 3.f;
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.042, 0.09, 0.08, 0.1);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.042, 0.09, 0.08, 0.3);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.042, 0.09, 0.08, 0.2);
    style.Colors[ImGuiCol_Button] = ImVec4(0.042, 0.09, 0.08, 0.1);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.042, 0.09, 0.08, 0.3);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.042, 0.09, 0.08, 0.2);
    style.Colors[ImGuiCol_Border] = ImVec4(0.2, 0.2, 0.2, 0.5);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int my_image_width = 0;
    int my_image_height = 0;
    GLuint my_image_texture = 0;
    // bool ret = LoadTextureFromFile(OPTIONSBG3, &my_image_texture, &my_image_width, &my_image_height);

    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;

        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one or the other.
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Retro NMS Installer", 0, flags);
            ImGui::GetBackgroundDrawList()->AddImage((void *)(intptr_t)my_image_texture, ImVec2(0, 0), ImVec2(my_image_width, my_image_height));

            {
                // Nav buttons
                ImGui::PushFont(nms_font_medium);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

                ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x - 63, 0));
                if (ImGui::Button("-", ImVec2(40, 20)))
                {
                    HWND hwnd = glfwGetWin32Window(window);
                    ShowWindow(hwnd, SW_MINIMIZE);
                }
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.0f, 0.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));

                ImVec2 cursor = ImGui::GetCursorPos();
                cursor.y -= 5;
                ImGui::SetCursorPos(cursor);

                if (ImGui::Button("x", ImVec2(40, 25)))
                    exit(0);

                ImGui::PopStyleVar(5);
                ImGui::PopStyleColor(3);
                ImGui::PopFont();
            }

            ImGui::PushFont(nms_font_big);
            const char *title_text = "No Man's Sky Retro";
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(title_text).x) * 0.5);
            ImGui::SetCursorPosY(100);
            ImGui::Text(title_text); // Display some text (you can use a format strings too)
            ImGui::PopFont();

#ifdef _DEBUG
            ImGui::PushFont(nms_font);
            const char *debug_text = "Application average %.3f ms/frame (%.1f FPS)";
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(debug_text).x - 20) * 0.5);
            ImGui::Text(debug_text, 1000.0f / io.Framerate, io.Framerate);
            ImGui::PopFont();
#endif

            // switch (GetInstallerState())
            // {
            // case EInstallerState_Start:
            //     DoStartState(nms_font_medium);
            //     break;
            // case EInstallerState_CheckShouldUseExistingInstall:
            //     DoPromptInstallState(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_DownloadFractal:
            //     DoDepotDownloader(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_DoDepotDownloadShit:
            //     DoFractalDownload(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_HandleSteamCredentialsInput:
            //     DoSteamCredsInput(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_DownloadReNMS:
            //     DoDownloadReNMS(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_Finalise:
            //     DoFinaliseInstall(nms_font_medium, nms_font);
            //     break;
            // case EInstallerState_Finished:
            //     DoFinishInstall(nms_font_medium, nms_font);
            //     break;
            // default:
            //     break;
            // }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    curl_global_cleanup();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
