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
        GIT_TAG d94cd1efad89953a360bcd8835a066104da00003
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
        GIT_TAG 747c5fd1e3668a427c081422574e589dd9111dc0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3_ttf)
endif()

# Function to read a file and turn it into a C file / header
# Function to read a file and turn it into a C file / header
function(embed_resource input_file output_c output_h c_name)
    file(READ ${input_file} hex_content HEX)
    
    # Calculate the size (2 hex characters = 1 byte)
    string(LENGTH "${hex_content}" hex_len)
    math(EXPR file_size "${hex_len} / 2")

    # Handle empty files
    if(hex_len EQUAL 0)
        set(array_values "0x00")
        # Size is technically 0, even though we pad the array with 1 byte to keep C happy
        set(file_size 0) 
    else()
        # Regex magic to format hex dump
        string(REGEX REPLACE "(..)" "0x\\1, " array_values "${hex_content}")
        # Remove trailing comma
        string(REGEX REPLACE ", $" "" array_values "${array_values}")
    endif()

    # --- 1. Write the Header (.h) ---
    # CHANGE: We define the size as a MACRO so it is a compile-time constant.
    set(header_content "
#pragma once
#include <stddef.h>

extern const unsigned char ${c_name}_data[];
#define ${c_name}_size ${file_size}
")
    file(WRITE ${output_h} "${header_content}")

    # --- 2. Write the Source (.c) ---
    # The array definition stays the same
    set(source_content "
#include \"${c_name}.h\"

const unsigned char ${c_name}_data[] = { ${array_values} };
")
    file(WRITE ${output_c} "${source_content}")

endfunction()

# Find all files in assets/ recursively
file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")

message(STATUS "Bundling assets...")

set(GENERATED_SOURCES "")

foreach(file_path ${ASSET_FILES})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/assets" "${file_path}")
    
    # Sanitize filename to create a valid C variable name
    string(REPLACE "/" "_" safe_name "${rel_path}")
    string(REPLACE "\\" "_" safe_name "${safe_name}")
    string(REPLACE "." "_" safe_name "${safe_name}")
    string(REPLACE "-" "_" safe_name "${safe_name}")
    set(safe_name "assets_${safe_name}")
    
    # Define paths for both .c and .h
    set(out_c "${CMAKE_BINARY_DIR}/generated/${safe_name}.c")
    set(out_h "${CMAKE_BINARY_DIR}/generated/${safe_name}.h")
    
    # Call the updated function
    embed_resource("${file_path}" "${out_c}" "${out_h}" "${safe_name}")
    
    # Add the .c file to our list
    list(APPEND GENERATED_SOURCES "${out_c}")
endforeach()

set(SOURCES
    src/main.c
    src/connect4.c
    src/assets/fonts.c
    src/assets/sounds.c
    src/game/board.c
    src/game/events.c
    src/game/utils.c
    src/ui/cursorManager.c
    src/ui/elements/button.c
    src/ui/elements/buttonGroup.c
    src/ui/elements/borders.c
    src/ui/elements/index.c
    src/ui/elements/popup.c
    src/ui/elements/rectangle.c
    src/ui/elements/text.c
    src/ui/screens/game.c
    src/ui/screens/menu.c
    src/ui/screens/settings.c
)

if(WIN32)
# Add WIN32 before ${SOURCES} to remove the debug console
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${GENERATED_SOURCES} resources/icon.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${GENERATED_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)