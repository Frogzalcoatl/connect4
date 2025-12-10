# Vibe coded ah CMakeLists.txt file. I was struggling to figure it out myself ngl.

cmake_minimum_required(VERSION 3.28)
project(Connect4 LANGUAGES C)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_PARALLEL_LEVEL 0 CACHE STRING "Number of parallel build jobs")

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

set(SOURCES
    src/main.c
    src/connect4.c
    src/game/board.c
    src/game/random.c
    src/ui/button.c
    src/ui/cursorManager.c
    src/ui/fontManager.c
    src/ui/rectangle_element.c
    src/ui/text_element.c
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

function(make_header_file TARGET_NAME SOURCE_FILE VAR_NAME)
    set(HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/${VAR_NAME}.h")
    
    # Read the file as hex
    file(READ "${SOURCE_FILE}" HEX_CONTENTS HEX)
    # Add 0x prefix and commas
    string(REGEX REPLACE "(..)" "0x\\1, " HEX_C_ARRAY "${HEX_CONTENTS}")
    
    # Write the .h file
    file(WRITE "${HEADER_FILE}" 
        "#pragma once\n"
        "#include <stddef.h>\n\n"
        "const unsigned char ${VAR_NAME}[] = { ${HEX_C_ARRAY} };\n"
        "#define ${VAR_NAME}_len sizeof(${VAR_NAME})"
    )
    
    # Make sure the target can find this header
    target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
endfunction()

make_header_file(${PROJECT_NAME} "assets/fonts/Monocraft.ttf" "FONT_Monocraft")
make_header_file(${PROJECT_NAME} "assets/fonts/Monocraft-Bold.ttf" "FONT_Monocraft_Bold")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)