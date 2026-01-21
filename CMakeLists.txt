# Needed ai for this ngl

cmake_minimum_required(VERSION 3.22)
project(Connect4 LANGUAGES C)

set(GAME_VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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

# Helper macro to silence warnings for a specific target
function(disable_target_warnings target)
    if(TARGET ${target})
        if(MSVC)
            # /W0 = Disable all warnings
            target_compile_options(${target} PRIVATE /W0) 
        else()
            # -w = Disable all warnings
            target_compile_options(${target} PRIVATE -w) 
        endif()
    endif()
endfunction()

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
    disable_target_warnings(SDL3)
    disable_target_warnings(SDL3-shared)
    disable_target_warnings(SDL3-static)

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
    disable_target_warnings(SDL3_ttf)
    disable_target_warnings(SDL3_ttf-shared) 
    disable_target_warnings(SDL3_ttf-static)

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

# --- SDL3_image SETUP ---
set(SDLIMAGE_SHARED ${SDL_SHARED_MODE} CACHE BOOL "Build shared libraries" FORCE)
set(SDLIMAGE_STATIC ${SDL_STATIC_MODE} CACHE BOOL "Build static libraries" FORCE)
set(SDLIMAGE_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(SDLIMAGE_VENDORED ON CACHE BOOL "Use vendored libraries" FORCE)

# DISABLE complex formats that require external tools (like NASM)
set(SDLIMAGE_AVIF OFF CACHE BOOL "Disable AVIF to avoid NASM dependency" FORCE)

# Disable heavy formats (slow build)
set(SDLIMAGE_JXL  OFF CACHE BOOL "Disable JXL to speed up build" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "Disable WEBP to speed up build" FORCE)
set(SDLIMAGE_TIF  OFF CACHE BOOL "Disable TIFF to speed up build" FORCE)

# Disable other uneeded formats
set(SDLIMAGE_BMP  OFF CACHE BOOL "Disable BMP"  FORCE)
set(SDLIMAGE_GIF  OFF CACHE BOOL "Disable GIF"  FORCE)
set(SDLIMAGE_JPG  OFF CACHE BOOL "Disable JPG"  FORCE)
set(SDLIMAGE_LBM  OFF CACHE BOOL "Disable LBM"  FORCE)
set(SDLIMAGE_PCX  OFF CACHE BOOL "Disable PCX"  FORCE)
set(SDLIMAGE_PNM  OFF CACHE BOOL "Disable PNM"  FORCE)
set(SDLIMAGE_QOI  OFF CACHE BOOL "Disable QOI"  FORCE)
set(SDLIMAGE_STB  OFF CACHE BOOL "Disable STB"  FORCE)
set(SDLIMAGE_TGA  OFF CACHE BOOL "Disable TGA"  FORCE)
set(SDLIMAGE_XCF  OFF CACHE BOOL "Disable XCF"  FORCE)
set(SDLIMAGE_XPM  OFF CACHE BOOL "Disable XPM"  FORCE)
set(SDLIMAGE_XV   OFF CACHE BOOL "Disable XV"   FORCE)

# Enabled formats
set(SDLIMAGE_PNG ON CACHE BOOL "Enable PNG" FORCE)
set(SDLIMAGE_SVG ON CACHE BOOL "Enable SVG" FORCE)

find_package(SDL3_image CONFIG QUIET)

if(NOT SDL3_image_FOUND)
    message(STATUS "SDL3_image not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW OFF
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_image)
    disable_target_warnings(SDL3_image)
    disable_target_warnings(SDL3_image-shared)
    disable_target_warnings(SDL3_image-static)

    if(NOT TARGET SDL3_image::SDL3_image)
        if(TARGET SDL3_image)
            add_library(SDL3_image::SDL3_image ALIAS SDL3_image)
        elseif(TARGET SDL3_image-shared)
            add_library(SDL3_image::SDL3_image ALIAS SDL3_image-shared)
        elseif(TARGET SDL3_image-static)
            add_library(SDL3_image::SDL3_image ALIAS SDL3_image-static)
        endif()
    endif()
endif()

# --- SDL3_mixer SETUP ---
set(SDLMIXER_SHARED ${SDL_SHARED_MODE} CACHE BOOL "Build shared libraries" FORCE)
set(SDLMIXER_STATIC ${SDL_STATIC_MODE} CACHE BOOL "Build static libraries" FORCE)
set(SDLMIXER_SAMPLES OFF CACHE BOOL "Build samples" FORCE)
set(SDLMIXER_VENDORED ON CACHE BOOL "Use vendored libraries" FORCE)

# Force internal dependencies to be static
set(SDLMIXER_DEPS_SHARED OFF CACHE BOOL "Force mixer deps static" FORCE)
set(SDLMIXER_MOD_XMP_SHARED OFF CACHE BOOL "Force XMP static" FORCE)
set(SDLMIXER_VORBIS_SHARED OFF CACHE BOOL "Force Vorbis static" FORCE)
set(SDLMIXER_OPUS_SHARED OFF CACHE BOOL "Force Opus static" FORCE)
set(SDLMIXER_FLAC_SHARED OFF CACHE BOOL "Force FLAC static" FORCE)
set(SDLMIXER_MIDI_TIMIDITY_SHARED OFF CACHE BOOL "Force Timidity static" FORCE)
set(LIBXMP_SHARED OFF CACHE BOOL "Force LibXMP static" FORCE)
set(OGG_SHARED OFF CACHE BOOL "Force OGG static" FORCE)
set(VORBIS_SHARED OFF CACHE BOOL "Force Vorbis static" FORCE)

option(USE_ASSEMBLY_OPTIMIZATIONS "Enable YASM assembly optimizations" OFF)
if(USE_ASSEMBLY_OPTIMIZATIONS)
    set(SDLMIXER_MP3_MPG123 ON CACHE BOOL "Enable mpg123" FORCE)
    set(SDLMIXER_MP3_MINIMP3 OFF CACHE BOOL "Disable minimp3" FORCE)
else()
    set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "Disable mpg123" FORCE)
    set(SDLMIXER_MP3_MINIMP3 ON  CACHE BOOL "Enable minimp3" FORCE)
endif()

# This is a library used for retro consoles. Not needed
set(SDLMIXER_GME OFF CACHE BOOL "Disable Game Music Emu (Fixes Android build)" FORCE)

# Disabled formats (explicitly disable libflac to avoid YASM check)
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "Disable libflac" FORCE)
set(SDLMIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
set(SDLMIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
set(SDLMIXER_MOD OFF CACHE BOOL "Disable MOD"  FORCE)
set(SDLMIXER_MP3 OFF CACHE BOOL "Enable MP3" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "Disable OPUS" FORCE)
set(SDLMIXER_XMP OFF CACHE BOOL "Disable XMP" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "Disable WavPack" FORCE)

# Enabled formats
set(SDLMIXER_OGG ON CACHE BOOL "Enable OGG" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libs" FORCE)
set(OGG_SHARED OFF CACHE BOOL "Disable Ogg Shared" FORCE)
set(VORBIS_SHARED OFF CACHE BOOL "Disable Vorbis Shared" FORCE)

find_package(SDL3_mixer CONFIG QUIET)

if(NOT SDL3_mixer_FOUND)
    message(STATUS "SDL3_mixer not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main 
        GIT_SHALLOW OFF
        SYSTEM
    )

    # For CMake version warnings
    set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "Silence Deprecation Warnings" FORCE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "Silence Dev Warnings" FORCE)
    
    FetchContent_MakeAvailable(SDL3_mixer)

    set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "Silence Deprecation Warnings" FORCE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF CACHE BOOL "Silence Dev Warnings" FORCE)

    disable_target_warnings(SDL3_mixer)
    disable_target_warnings(SDL3_mixer-shared) 
    disable_target_warnings(SDL3_mixer-static)

    if(NOT TARGET SDL3_mixer::SDL3_mixer)
        if(TARGET SDL3_mixer)
            add_library(SDL3_mixer::SDL3_mixer ALIAS SDL3_mixer)
        elseif(TARGET SDL3_mixer-shared)
            add_library(SDL3_mixer::SDL3_mixer ALIAS SDL3_mixer-shared)
        elseif(TARGET SDL3_mixer-static)
            add_library(SDL3_mixer::SDL3_mixer ALIAS SDL3_mixer-static)
        endif()
    endif()
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
list(FILTER SOURCES EXCLUDE REGEX ".*packer.c$")

# --- MAIN TARGET ---
if(ANDROID)
    set(EXE_NAME main)
    # Android uses a Shared Library that Java loads
    add_library(${EXE_NAME} SHARED ${SOURCES})
else()
    set(EXE_NAME ${PROJECT_NAME})
    if(WIN32)
        set(ICON_RC resources/icon.rc)
        # set(WIN_FLAGS WIN32)
    endif()
    if(APPLE)
        set(ICON_MACOS resources/icon.icns)
        # This tells CMake to copy the file to Connect4.app/Contents/Resources/
        set_source_files_properties(${ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif()
    add_executable(${EXE_NAME} MACOSX_BUNDLE ${WIN_FLAGS} ${SOURCES} ${ICON_RC})
endif()

if(WIN32)
    # If Debug: WIN32_EXECUTABLE = FALSE (Console opens)
    # If Release: WIN32_EXECUTABLE = TRUE (No Console)
    set_target_properties(${EXE_NAME} PROPERTIES 
        WIN32_EXECUTABLE "$<NOT:$<CONFIG:Debug>>"
    )
endif()

# --- COMPILER SETTINGS ---
if(MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /W4 /std:clatest)
    target_compile_definitions(${EXE_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra)
    target_compile_features(${EXE_NAME} PRIVATE c_std_23)
    if(APPLE)
        target_compile_options(${EXE_NAME} PRIVATE -Wno-error=incompatible-function-pointer-types)
    endif()
endif()

# --- LINKING ---
target_link_libraries(${EXE_NAME} PRIVATE 
    SDL3::SDL3 
    SDL3_ttf::SDL3_ttf
    SDL3_image::SDL3_image 
    SDL3_mixer::SDL3_mixer
)

if(ANDROID)
    target_link_libraries(${EXE_NAME} PRIVATE m log)
endif()

# Attempt to find JNI on the host machine (Windows/Mac/Linux)
# This allows IntelliSense to find <jni.h> even when not building for Android.
find_package(JNI QUIET)

if(JNI_FOUND)
    target_include_directories(${EXE_NAME} PRIVATE ${JNI_INCLUDE_DIRS})
    message(STATUS "JNI headers found at: ${JNI_INCLUDE_DIRS}")
endif()

if(APPLE)
    # Standard macOS frameworks
    target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation" "-framework AppKit" "-framework QuartzCore")
    
    set_target_properties(${EXE_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.${EXE_NAME}"
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
        XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    )
endif()

target_include_directories(${EXE_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

target_compile_definitions(${EXE_NAME} PRIVATE _USE_MATH_DEFINES)

# --- DISCORD SDK ---
set(IS_MOBILE FALSE)
if(ANDROID)
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

# --- PACKER TOOL SETUP ---
set(PACKER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/tools")

# Asset list generation
file(GLOB_RECURSE RAW_ASSETS "${CMAKE_SOURCE_DIR}/assets/*")
set(ASSET_LIST_TEXT "")
foreach(entry ${RAW_ASSETS})
    if(NOT IS_DIRECTORY ${entry})
        file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${entry}")
        string(APPEND ASSET_LIST_TEXT "${rel_path}\n")
    endif()
endforeach()
set(LIST_FILE_PATH "${CMAKE_BINARY_DIR}/assets_list.txt")
file(WRITE "${LIST_FILE_PATH}" "${ASSET_LIST_TEXT}")

if(CMAKE_CROSSCOMPILING)
    include(ExternalProject)
    set(HOST_BUILD_DIR "${CMAKE_BINARY_DIR}/host_packer_build")
    
    if(CMAKE_HOST_WIN32)
        set(HOST_GENERATOR "MinGW Makefiles")
        set(PACKER_TOOL "${HOST_BUILD_DIR}/packer.exe")
        set(PACKER_INSTALL_CMD ${CMAKE_COMMAND} -E echo "Skipping install step for Windows/Android Host")
    else()
        set(HOST_GENERATOR "Unix Makefiles")
        set(PACKER_TOOL "${HOST_BUILD_DIR}/packer")
        
        # Macos requires signing
        if(APPLE)
             set(PACKER_INSTALL_CMD sh -c "chmod +x ${PACKER_TOOL} && codesign --force --sign - ${PACKER_TOOL}")
        else()
             set(PACKER_INSTALL_CMD ${CMAKE_COMMAND} -E echo "Skipping install step for Linux Host")
        endif()
    endif()

    set(HOST_CMAKE_ARGS 
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${HOST_BUILD_DIR}"
        "-DCMAKE_BUILD_TYPE=Release"
    )
    
    # Clear cross-compile flags
    list(APPEND HOST_CMAKE_ARGS "-DCMAKE_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}")
    list(APPEND HOST_CMAKE_ARGS "-DCMAKE_OSX_ARCHITECTURES=") 
    list(APPEND HOST_CMAKE_ARGS "-DCMAKE_OSX_SYSROOT=")
    list(APPEND HOST_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE=")

    ExternalProject_Add(host_packer_project
        SOURCE_DIR ${PACKER_SOURCE_DIR}
        BINARY_DIR ${HOST_BUILD_DIR}
        PREFIX ${HOST_BUILD_DIR}
        CMAKE_GENERATOR "${HOST_GENERATOR}"
        CMAKE_ARGS ${HOST_CMAKE_ARGS}
        INSTALL_COMMAND ${PACKER_INSTALL_CMD}
        BUILD_ALWAYS 1
    )
    
    set(PACKER_DEPENDS host_packer_project)
    
else()
    # Normal build (Desktop)
    add_subdirectory(src/tools)
    set_target_properties(packer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools")
    set(PACKER_TOOL $<TARGET_FILE:packer>)
    set(PACKER_DEPENDS packer)
endif()

set(MASTER_DAT_NAME ${CMAKE_PROJECT_NAME})
set(MASTER_DAT_PATH "${CMAKE_BINARY_DIR}/${MASTER_DAT_NAME}.dat")
set(GENERATED_HEADER_PATH "${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}/dat.h")
file(WRITE "${GENERATED_HEADER_PATH}" "// Automatically generated by CMake. Do not edit.\n#pragma once\n\n#define MASTER_DAT_PATH \"${MASTER_DAT_NAME}.dat\"\n#define GAME_VERSION ${GAME_VERSION}")

add_custom_command(
    OUTPUT "${MASTER_DAT_PATH}"
    COMMAND ${PACKER_TOOL} "${LIST_FILE_PATH}" "${MASTER_DAT_PATH}" "${GAME_VERSION}"
    DEPENDS ${PACKER_DEPENDS} "${LIST_FILE_PATH}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Packing Assets using ${PACKER_TOOL}..."
)

add_custom_target(PackAssets ALL DEPENDS "${MASTER_DAT_PATH}")
add_dependencies(${EXE_NAME} PackAssets)

# --- COPY TO OUTPUT ---
if(ANDROID)
    set(ANDROID_ASSETS_DIR "${CMAKE_SOURCE_DIR}/android/app/src/main/assets")
    file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}")
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MASTER_DAT_PATH}"
        "${ANDROID_ASSETS_DIR}/${MASTER_DAT_NAME}.dat"
    )
else()    
    add_custom_target(EnsureGameDat ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MASTER_DAT_PATH}"
        "$<TARGET_FILE_DIR:${EXE_NAME}>/${MASTER_DAT_NAME}.dat"
        COMMENT "Verifying assets in output directory..."
    )
    add_dependencies(EnsureGameDat ${EXE_NAME})
endif()