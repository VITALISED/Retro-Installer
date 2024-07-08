cmake_minimum_required(VERSION 3.15)

project(
    retro_installer
    LANGUAGES CXX
    VERSION 1.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(BUILD_CURL_EXE OFF)
set(CURL_USE_SCHANNEL ON)

find_package(OpenGL)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/curl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/glfw)

set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imconfig.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui_internal.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imgui.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imstb_rectpack.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imstb_textedit.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/imstb_truetype.h
    ${CMAKE_SOURCE_DIR}/submodules/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/submodules/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(
    retro_installer WIN32
    ${CMAKE_SOURCE_DIR}/download_file.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/setup.cpp
    ${CMAKE_SOURCE_DIR}/steam_auth.cpp
    ${CMAKE_SOURCE_DIR}/texture.cpp
    ${CMAKE_SOURCE_DIR}/versions.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(
    retro_installer
    PRIVATE OpenGL::GL
            glfw
            libcurl
)
target_include_directories(
    retro_installer
    PRIVATE ${CMAKE_SOURCE_DIR}/imgui
            ${CMAKE_SOURCE_DIR}/submodules/curl/include
            ${CMAKE_SOURCE_DIR}/submodules/glfw/include
            ${CMAKE_SOURCE_DIR}/submodules/imgui
)

target_compile_definitions(retro_installer PRIVATE NOMINMAX)
