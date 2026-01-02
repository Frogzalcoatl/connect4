# Ngl couldnt figure this file out without ai

cmake_minimum_required(VERSION 3.22)
# Including CXX and OBJC for discord-rpc
project(Connect4 LANGUAGES C CXX)

# For the one macos specific objective c file in discord-rpc
# Android doesnt have an objective c compiler apparently
if(APPLE)
    enable_language(OBJC)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_PARALLEL_LEVEL 0 CACHE STRING "Number of parallel build jobs")

set(SDL_SHARED_MODE OFF)
set(SDL_STATIC_MODE ON)

if(ANDROID)
    set(SDL_SHARED_MODE ON)
    set(SDL_STATIC_MODE OFF)
endif()

# --- SDL3 SETUP ---
set(SDL_SHARED ${SDL_SHARED_MODE} CACHE BOOL "Build shared libraries" FORCE)
set(SDL_STATIC ${SDL_STATIC_MODE} CACHE BOOL "Build static libraries" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL test library" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "Don't add install targets for SDL" FORCE)

find_package(SDL3 CONFIG QUIET) 

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.28
        GIT_SHALLOW OFF
        EXCLUDE_FROM_ALL
        SYSTEM PRIVATE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3)
    if(NOT TARGET SDL3::SDL3)
        if(TARGET SDL3-static)
            add_library(SDL3::SDL3 ALIAS SDL3-static)
        elseif(TARGET SDL3)
            add_library(SDL3::SDL3 ALIAS SDL3)
        endif()
    endif()
endif()

# --- SDL3_ttf SETUP ---
set(SDLTTF_SHARED ${SDL_SHARED_MODE} CACHE BOOL "Build shared libraries" FORCE)
set(SDLTTF_STATIC ${SDL_STATIC_MODE} CACHE BOOL "Build static libraries" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "Build SDL3_ttf samples" FORCE)
set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored FreeType/HarfBuzz" FORCE)

find_package(SDL3_ttf CONFIG QUIET)

if(NOT SDL3_ttf_FOUND)
    message(STATUS "SDL3_ttf not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
        GIT_SHALLOW OFF
        EXCLUDE_FROM_ALL
        SYSTEM PRIVATE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        if(TARGET SDL3_ttf-static)
            add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf-static)
        elseif(TARGET SDL3_ttf)
            add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf)
        endif()
    endif()
endif()

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
    # Write the Header (.h)
    set(header_content "#pragma once
#include <stddef.h>

extern const unsigned char ${c_name}_data[];
#define ${c_name}_size ${file_size}")
    file(WRITE ${output_h} "${header_content}")
    # Write the Source (.c)
    set(source_content "#include \"${c_name}.h\"

const unsigned char ${c_name}_data[] = { ${array_values} };")
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

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

if(WIN32)
    set(WIN_FLAGS WIN32)
    set(ICON_RC resources/icon.rc)
endif()

if(ANDROID)
    set(EXE_NAME main)
    add_library(${EXE_NAME} SHARED ${SOURCES} ${GENERATED_SOURCES})
else()
    set(EXE_NAME ${PROJECT_NAME})
    add_executable(${EXE_NAME} MACOSX_BUNDLE ${WIN_FLAGS} ${SOURCES} ${GENERATED_SOURCES} ${ICON_RC})
endif()

if(APPLE)
    set_target_properties(${EXE_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.${EXE_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.yourname.${EXE_NAME}"
    )

    # Disable Code Signing (Required for GitHub Actions to pass)
    set_target_properties(${EXE_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )
endif()

if(MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /W3 /std:clatest)
else()
    target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra)
    target_compile_features(${EXE_NAME} PRIVATE c_std_23)
    if(APPLE)
        target_compile_options(${EXE_NAME} PRIVATE -Wno-error=incompatible-function-pointer-types)
    endif()
endif()

target_link_libraries(${EXE_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        # iOS Frameworks
        target_link_libraries(${EXE_NAME} PRIVATE 
            "-framework Foundation" 
            "-framework UIKit" 
            "-framework QuartzCore" 
            "-framework CoreGraphics"
        )
    else()
        # macOS Frameworks
        target_link_libraries(${EXE_NAME} PRIVATE 
            "-framework Foundation" 
            "-framework AppKit" 
            "-framework QuartzCore"
        )
    endif()
endif()

target_include_directories(${EXE_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

# --- DISCORD SDK SETUP ---
set(IS_MOBILE FALSE)
if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(IS_MOBILE TRUE)
endif()

set(USE_DISCORD OFF)

# Only enable Discord for Desktop platforms
if((WIN32 OR UNIX OR APPLE) AND NOT IS_MOBILE)
    set(USE_DISCORD ON)
endif()

if(USE_DISCORD)
    set(DISCORD_RPC_DIR "${CMAKE_SOURCE_DIR}/deps/discord-rpc")

    target_include_directories(${EXE_NAME} PRIVATE
        "${DISCORD_RPC_DIR}/include"
        "${DISCORD_RPC_DIR}/thirdparty/rapidjson-1.1.0/include"
    )

    set(DISCORD_SRC
        "${DISCORD_RPC_DIR}/src/discord_rpc.cpp"
        "${DISCORD_RPC_DIR}/src/rpc_connection.cpp"
        "${DISCORD_RPC_DIR}/src/serialization.cpp"
    )

    if(WIN32)
        list(APPEND DISCORD_SRC 
            "${DISCORD_RPC_DIR}/src/connection_win.cpp"
            "${DISCORD_RPC_DIR}/src/discord_register_win.cpp"
        )
        target_compile_definitions(${EXE_NAME} PRIVATE DISCORD_WINDOWS)
    elseif(APPLE)
        list(APPEND DISCORD_SRC 
            "${DISCORD_RPC_DIR}/src/connection_unix.cpp"
            "${DISCORD_RPC_DIR}/src/discord_register_osx.m"
        )
        target_compile_definitions(${EXE_NAME} PRIVATE DISCORD_OSX)
    else() # Linux
        list(APPEND DISCORD_SRC 
            "${DISCORD_RPC_DIR}/src/connection_unix.cpp"
            "${DISCORD_RPC_DIR}/src/discord_register_linux.cpp"
        )
        target_compile_definitions(${EXE_NAME} PRIVATE DISCORD_LINUX)
    endif()

    target_sources(${EXE_NAME} PRIVATE ${DISCORD_SRC})
    target_compile_definitions(${EXE_NAME} PRIVATE USE_DISCORD)
    message(STATUS "discord-rpc enabled")
endif()

# So math.h always includes math defines (ex: M_PI)
target_compile_definitions(${EXE_NAME} PRIVATE _USE_MATH_DEFINES)