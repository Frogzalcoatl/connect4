cmake_minimum_required(VERSION 3.22)
project(Connect4 LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(ANDROID)
    # Android needs Shared Libraries (.so)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
    set(SDL_SHARED_MODE ON)
    set(SDL_STATIC_MODE OFF)
else()
    # Windows/Desktop defaults to Static Libraries (.lib/.a)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(SDL_SHARED_MODE OFF)
    set(SDL_STATIC_MODE ON)
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
        GIT_TAG release-3.4.0
        GIT_SHALLOW OFF
        SYSTEM
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL3)

    if(NOT TARGET SDL3::SDL3)
        if(TARGET SDL3)
            add_library(SDL3::SDL3 ALIAS SDL3)
        elseif(TARGET SDL3-shared)
            add_library(SDL3::SDL3 ALIAS SDL3-shared)
        elseif(TARGET SDL3-static)
            add_library(SDL3::SDL3 ALIAS SDL3-static)
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
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_ttf)

    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        if(TARGET SDL3_ttf)
            add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf)
        elseif(TARGET SDL3_ttf-shared)
            add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf-shared)
        elseif(TARGET SDL3_ttf-static)
            add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf-static)
        endif()
    endif()
endif()

# --- ASSETS ---
function(embed_resource input_file output_c output_h c_name)
    file(READ ${input_file} hex_content HEX)
    string(LENGTH "${hex_content}" hex_len)
    math(EXPR file_size "${hex_len} / 2")
    if(hex_len EQUAL 0)
        set(array_values "0x00")
        set(file_size 0) 
    else()
        string(REGEX REPLACE "(..)" "0x\\1, " array_values "${hex_content}")
        string(REGEX REPLACE ", $" "" array_values "${array_values}")
    endif()
    set(header_content "#pragma once
#include <stddef.h>
extern const unsigned char ${c_name}_data[];
#define ${c_name}_size ${file_size}")
    file(WRITE ${output_h} "${header_content}")
    set(source_content "#include \"${c_name}.h\"
const unsigned char ${c_name}_data[] = { ${array_values} };")
    file(WRITE ${output_c} "${source_content}")
endfunction()

file(GLOB_RECURSE ASSET_FILES "${CMAKE_SOURCE_DIR}/assets/*")
set(GENERATED_SOURCES "")

foreach(file_path ${ASSET_FILES})
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}/assets" "${file_path}")
    string(REPLACE "/" "_" safe_name "${rel_path}")
    string(REPLACE "\\" "_" safe_name "${safe_name}")
    string(REPLACE "." "_" safe_name "${safe_name}")
    string(REPLACE "-" "_" safe_name "${safe_name}")
    set(safe_name "assets_${safe_name}")
    set(out_c "${CMAKE_BINARY_DIR}/generated/${safe_name}.c")
    set(out_h "${CMAKE_BINARY_DIR}/generated/${safe_name}.h")
    embed_resource("${file_path}" "${out_c}" "${out_h}" "${safe_name}")
    list(APPEND GENERATED_SOURCES "${out_c}")
endforeach()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

# --- MAIN TARGET ---
if(ANDROID)
    set(EXE_NAME main)
    # Android uses a Shared Library that Java loads
    add_library(${EXE_NAME} SHARED ${SOURCES} ${GENERATED_SOURCES})
else()
    set(EXE_NAME ${PROJECT_NAME})
    if(WIN32)
        set(ICON_RC resources/icon.rc)
        set(WIN_FLAGS WIN32)
    endif()
    add_executable(${EXE_NAME} MACOSX_BUNDLE ${WIN_FLAGS} ${SOURCES} ${GENERATED_SOURCES} ${ICON_RC})
endif()

# --- COMPILER SETTINGS ---
if(MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /W3 /std:clatest)
    target_compile_definitions(${EXE_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra)
    target_compile_features(${EXE_NAME} PRIVATE c_std_23)
    if(APPLE)
        target_compile_options(${EXE_NAME} PRIVATE -Wno-error=incompatible-function-pointer-types)
    endif()
endif()

# --- LINKING ---
target_link_libraries(${EXE_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

if(ANDROID)
    target_link_libraries(${EXE_NAME} PRIVATE m)
endif()

# Attempt to find JNI on the host machine (Windows/Mac/Linux)
# This allows IntelliSense to find <jni.h> even when not building for Android.
find_package(JNI QUIET)

if(JNI_FOUND)
    target_include_directories(${EXE_NAME} PRIVATE ${JNI_INCLUDE_DIRS})
    message(STATUS "JNI headers found at: ${JNI_INCLUDE_DIRS}")
endif()

if(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation" "-framework UIKit" "-framework QuartzCore" "-framework CoreGraphics")
    else()
        target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation" "-framework AppKit" "-framework QuartzCore")
    endif()
    set_target_properties(${EXE_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.${EXE_NAME}"
    )
endif()

target_include_directories(${EXE_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

target_compile_definitions(${EXE_NAME} PRIVATE _USE_MATH_DEFINES)

# --- DISCORD SDK ---
set(IS_MOBILE FALSE)
if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(IS_MOBILE TRUE)
endif()

set(USE_DISCORD OFF)

if((WIN32 OR UNIX OR APPLE) AND NOT IS_MOBILE)
    set(USE_DISCORD ON)
endif()

if(USE_DISCORD)
    enable_language(CXX)
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
        enable_language(OBJC)
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