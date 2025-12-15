# Vibe coded ah CMakeLists.txt file. I was struggling to figure it out myself ngl.

cmake_minimum_required(VERSION 3.28)
project(Connect4 LANGUAGES C)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_PARALLEL_LEVEL 0 CACHE STRING "Number of parallel build jobs")

# --- SDL3 SETUP ---
set(SDL_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build static libraries" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL test library" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "Don't add install targets for SDL" FORCE)

find_package(SDL3 CONFIG QUIET) 

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# --- SDL3_ttf SETUP ---
set(SDLTTF_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(SDLTTF_STATIC ON  CACHE BOOL "Build static libraries" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "Build SDL3_ttf samples" FORCE)
set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored FreeType/HarfBuzz" FORCE)
set(SDLTTF_NSC_INTROSPECTION OFF CACHE BOOL "Enable NSC Introspection" FORCE)

find_package(SDL3_ttf CONFIG QUIET)

if(NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG main
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3_ttf)
endif()

# Function to read a file and turn it into a C header
function(embed_resource input_file output_file c_name)
    file(READ ${input_file} hex_content HEX)
    
    # Check if file is empty to prevent empty initializer errors
    string(LENGTH "${hex_content}" hex_len)
    if(hex_len EQUAL 0)
        # Create a dummy entry if file is empty
        set(array_values "0x00")
        set(size_def "0")
    else()
        # 1. Add 0x prefix and comma to every byte
        string(REGEX REPLACE "(..)" "0x\\1, " array_values "${hex_content}")
        # 2. REMOVE the trailing comma and space to ensure valid C syntax
        string(REGEX REPLACE ", $" "" array_values "${array_values}")
        set(size_def "sizeof(${c_name}_data)")
    endif()

    # Create the header file content
    set(file_content "
#pragma once
#include <stddef.h>

/* Auto-generated from ${input_file} */
static const unsigned char ${c_name}_data[] = { ${array_values} };
#define ${c_name}_size ${size_def}
")

    file(WRITE ${output_file} "${file_content}")
endfunction()

# Find all files in assets/ recursively
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

message(STATUS "Bundling assets...")

foreach(file_path ${ASSET_FILES})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/assets" "${file_path}")
    
    # Sanitize filename
    string(REPLACE "/" "_" safe_name "${rel_path}")
    string(REPLACE "\\" "_" safe_name "${safe_name}")
    string(REPLACE "." "_" safe_name "${safe_name}")
    string(REPLACE "-" "_" safe_name "${safe_name}") # Handle dashes too
    set(safe_name "assets_${safe_name}")

    set(out_header "${CMAKE_BINARY_DIR}/generated/${safe_name}.h")
    
    embed_resource("${file_path}" "${out_header}" "${safe_name}")
    message(STATUS "  Embedded: ${rel_path} -> ${safe_name}.h")
endforeach()
message(STATUS "Assets embedded successfully.")

set(SOURCES
    src/main.c
    src/connect4.c
    src/assets/fonts.c
    src/assets/sounds.c
    src/game/board.c
    src/game/utils.c
    src/ui/cursorManager.c
    src/ui/elements/button.c
    src/ui/elements/borders.c
    src/ui/elements/popup.c
    src/ui/elements/rectangle.c
    src/ui/elements/text.c
    src/ui/screens/game.c
    src/ui/screens/menu.c
    src/ui/screens/settings.c
)

if(WIN32)
# Add WIN32 before ${SOURCES} to remove the debug console
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} resources/icon.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)