# Needed ai for this ngl

cmake_minimum_required(VERSION 3.22)
project(Connect4 LANGUAGES C CXX)

set(GAME_VERSION 1.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# SDL_Mixer Config, since it applies to both android and desktop
# Build Type
set(SDLMIXER_SHARED OFF CACHE BOOL "Force static" FORCE)
set(SDLMIXER_STATIC ON  CACHE BOOL "Force static" FORCE)
set(SDLMIXER_VENDORED ON CACHE BOOL "Use vendored deps" FORCE)

# Enabled Formats
set(SDLMIXER_OGG ON CACHE BOOL "Enable OGG" FORCE)

# Disabled Formats (Keep lightweight)
set(SDLMIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
set(SDLMIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
set(SDLMIXER_MP3 OFF CACHE BOOL "Disable MP3" FORCE)
set(SDLMIXER_MOD  OFF CACHE BOOL "Disable MOD"  FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "Disable OPUS" FORCE)
set(SDLMIXER_GME  OFF CACHE BOOL "Disable Game Music Emu" FORCE)
set(SDLMIXER_XMP  OFF CACHE BOOL "Disable XMP" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "Disable WavPack" FORCE)

# Advanced Optimizations (Optional, good for Android)
option(USE_ASSEMBLY_OPTIMIZATIONS "Enable YASM assembly optimizations" OFF)
if(USE_ASSEMBLY_OPTIMIZATIONS)
    set(SDLMIXER_MP3_MPG123 ON CACHE BOOL "Enable mpg123" FORCE)
    set(SDLMIXER_MP3_MINIMP3 OFF CACHE BOOL "Disable minimp3" FORCE)
else()
    set(SDLMIXER_MP3_MPG123 OFF CACHE BOOL "Disable mpg123" FORCE)
    set(SDLMIXER_MP3_MINIMP3 ON  CACHE BOOL "Enable minimp3" FORCE)
endif()

# Explicitly disable libflac to avoid searching for YASM/NASM
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "Disable libflac" FORCE)


# --- DEPENDENCY STRATEGY ---
if(ANDROID)
    # ANDROID: Build dependencies from source
    message(STATUS "Android build detected: Fetching dependencies from source...")
    
    include(FetchContent)

    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
        GIT_SHALLOW TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3)
    # Create alias so linking logic matches vcpkg names
    if(NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 ALIAS SDL3)
    endif()

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
    
    FetchContent_Declare(SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_image)
    if(NOT TARGET SDL3_image::SDL3_image)
        add_library(SDL3_image::SDL3_image ALIAS SDL3_image)
    endif()

    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.0
        GIT_SHALLOW TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        add_library(SDL3_ttf::SDL3_ttf ALIAS SDL3_ttf)
    endif()
    
    FetchContent_Declare(SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_mixer)

    # Android specific setup
    find_package(JNI REQUIRED)
    
else()
    # DESKTOP: Use vcpkg for most, Manual Fetch for Mixer
    message(STATUS "Desktop build detected: Using vcpkg...")

    if(NOT ANDROID)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    endif()

    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_image CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)
    
    # Discord RPC (Desktop Only)
    find_path(DISCORD_INCLUDE_DIR discord_rpc.h)
    find_library(DISCORD_LIBRARY NAMES discord-rpc)

    if(DISCORD_INCLUDE_DIR AND DISCORD_LIBRARY)
        add_library(discord-rpc::discord-rpc STATIC IMPORTED)
        set_target_properties(discord-rpc::discord-rpc PROPERTIES
            IMPORTED_LOCATION "${DISCORD_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DISCORD_INCLUDE_DIR}"
        )
    endif()

    # --- SDL3_mixer (Desktop Hybrid) ---
    # Note: Configuration is already done at the top of the file!
    
    # Silence Deprecation Warnings for Mixer
    set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)

    include(FetchContent)
    FetchContent_Declare(SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main 
        GIT_SHALLOW TRUE
        SYSTEM
    )
    FetchContent_MakeAvailable(SDL3_mixer)
endif()

# SILENCE WARNINGS (Mixer dependencies)
foreach(target SDL3_mixer SDL3_mixer-static SDL3_mixer-shared ogg vorbis vorbisenc vorbisfile opus)
    if(TARGET ${target})
        if(MSVC)
            target_compile_options(${target} PRIVATE /W0)
        else()
            target_compile_options(${target} PRIVATE -w)
        endif()
    endif()
endforeach()

# --- SOURCES ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
list(FILTER SOURCES EXCLUDE REGEX ".*packer.c$")

set(PLATFORM_LIBS "")

if(ANDROID)
    # Android requires a shared library named 'main'
    add_library(main SHARED ${SOURCES})
    set(EXE_NAME main)
    # Android specific system libs
    list(APPEND PLATFORM_LIBS m log android)
else()
    # Desktop Build
    set(EXE_NAME ${PROJECT_NAME})
    set(PLATFORM_SRCS "")

    if(WIN32)
        set(ICON_RC resources/icon.rc)
        list(APPEND PLATFORM_SRCS ${ICON_RC})
    elseif(APPLE)
        set(ICON_MACOS resources/icon.icns)
        list(APPEND PLATFORM_SRCS ${ICON_MACOS})
        set_source_files_properties(${ICON_MACOS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endif()

    add_executable(${EXE_NAME} MACOSX_BUNDLE ${SOURCES} ${PLATFORM_SRCS})
endif()

# --- COMPILER OPTIONS ---
if(MSVC)
    target_compile_options(${EXE_NAME} PRIVATE /W4)
    target_compile_definitions(${EXE_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${EXE_NAME} PRIVATE -Wall -Wextra -pedantic)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(${EXE_NAME} PRIVATE -Wno-newline-eof -Wno-deprecated-declarations)
    endif()
endif()

if(WIN32)
    set_target_properties(${EXE_NAME} PROPERTIES WIN32_EXECUTABLE "$<NOT:$<CONFIG:Debug>>")
endif()

if(APPLE)
    set_target_properties(${EXE_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.${EXE_NAME}"
    )
endif()

# Sanitizers (Linux/Mac only, Disable for MinGW)
if(NOT MSVC AND NOT MINGW AND NOT ANDROID)
    target_compile_options(${EXE_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    target_link_options(${EXE_NAME} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
endif()

# --- LINKING ---
if(TARGET SDL3_mixer AND NOT TARGET SDL3_mixer::SDL3_mixer)
    add_library(SDL3_mixer::SDL3_mixer ALIAS SDL3_mixer)
endif()

target_link_libraries(${EXE_NAME} PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image 
    SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
    ${PLATFORM_LIBS}
)

if(NOT ANDROID)
    target_link_libraries(${EXE_NAME} PRIVATE discord-rpc::discord-rpc)
    target_compile_definitions(${EXE_NAME} PRIVATE USE_DISCORD)
endif()

if(JNI_FOUND)
    target_include_directories(${EXE_NAME} PRIVATE ${JNI_INCLUDE_DIRS})
endif()

target_include_directories(${EXE_NAME} PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generated
)

target_compile_definitions(${EXE_NAME} PRIVATE _USE_MATH_DEFINES)

# --- ASSET PACKER TOOL ---
set(PACKER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/tools")
set(MASTER_DAT_NAME ${CMAKE_PROJECT_NAME})
set(MASTER_DAT_PATH "${CMAKE_BINARY_DIR}/${MASTER_DAT_NAME}.dat")
set(LIST_FILE_PATH "${CMAKE_BINARY_DIR}/assets_list.txt")

file(GLOB_RECURSE RAW_ASSETS "${CMAKE_SOURCE_DIR}/assets/*")
set(ASSET_LIST_TEXT "")
foreach(entry ${RAW_ASSETS})
    if(NOT IS_DIRECTORY ${entry})
        file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${entry}")
        string(APPEND ASSET_LIST_TEXT "${rel_path}\n")
    endif()
endforeach()
file(WRITE "${LIST_FILE_PATH}" "${ASSET_LIST_TEXT}")

set(GENERATED_HEADER_PATH "${CMAKE_BINARY_DIR}/generated/${CMAKE_PROJECT_NAME}/dat.h")
file(WRITE "${GENERATED_HEADER_PATH}" "// Auto-generated.\n#pragma once\n\n#define MASTER_DAT_PATH \"${MASTER_DAT_NAME}.dat\"\n#define GAME_VERSION ${GAME_VERSION}")

if(CMAKE_CROSSCOMPILING)
    # Android Build -> Compile Packer for Host
    include(ExternalProject)
    set(HOST_BUILD_DIR "${CMAKE_BINARY_DIR}/host_packer_build")
    
    set(PACKER_TOOL "${HOST_BUILD_DIR}/packer")
    if(CMAKE_HOST_WIN32)
        set(PACKER_TOOL "${HOST_BUILD_DIR}/packer.exe")
    endif()

    ExternalProject_Add(host_packer_project
        SOURCE_DIR ${PACKER_SOURCE_DIR}
        BINARY_DIR ${HOST_BUILD_DIR}
        CMAKE_ARGS 
            "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${HOST_BUILD_DIR}"
            "-DCMAKE_BUILD_TYPE=Release"
            "-DCMAKE_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}"
        INSTALL_COMMAND ""
        BUILD_ALWAYS 1
    )
    set(PACKER_DEPENDS host_packer_project)
else()
    add_subdirectory(src/tools)
    if(TARGET packer)
        # Force packer to its own folder so it stays out of the game's bin folder
        set_target_properties(packer PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools"
        )
        if(MSVC)
            target_compile_definitions(packer PRIVATE _CRT_SECURE_NO_WARNINGS)
        endif()
    endif()
    set(PACKER_TOOL $<TARGET_FILE:packer>)
    set(PACKER_DEPENDS packer)
endif()

add_custom_command(
    OUTPUT "${MASTER_DAT_PATH}"
    COMMAND ${PACKER_TOOL} "${LIST_FILE_PATH}" "${MASTER_DAT_PATH}" "${GAME_VERSION}"
    DEPENDS ${PACKER_DEPENDS} "${LIST_FILE_PATH}"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Packing Assets..."
)

add_custom_target(PackAssets ALL DEPENDS "${MASTER_DAT_PATH}")
add_dependencies(${EXE_NAME} PackAssets)

if(ANDROID)
    set(ANDROID_ASSETS_DIR "${CMAKE_SOURCE_DIR}/android/app/src/main/assets")
    file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}")
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MASTER_DAT_PATH}"
        "${ANDROID_ASSETS_DIR}/${MASTER_DAT_NAME}.dat"
    )
else()
    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MASTER_DAT_PATH}"
        "$<TARGET_FILE_DIR:${EXE_NAME}>/${MASTER_DAT_NAME}.dat"
    )
endif()

# --- CPACK CONFIGURATION ---
install(TARGETS ${EXE_NAME} 
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION .
)
install(FILES "${MASTER_DAT_PATH}" DESTINATION .)

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Frogzalcoatl")
set(CPACK_PACKAGE_VERSION "${GAME_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-v${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}-${CPACK_PACKAGE_NAME}")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
elseif(UNIX AND NOT APPLE)
    # Generate both DEB, RPM and a generic Tarball for Arch/others
    set(CPACK_GENERATOR "DEB;RPM;TGZ")

    # DEB Specifics (Ubuntu/Debian)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    # Tell Linux it needs SDL3 installed to run
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl3, libsdl3-image, libsdl3-mixer") 

    # RPM Specifics (Fedora)
    set(CPACK_RPM_PACKAGE_LICENSE "MIT") # Or your license
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
    set(CPACK_RPM_PACKAGE_REQUIRES "sdl3, sdl3_image, sdl3_mixer")
elseif(WIN32)
    # Using Inno Setup for the actual EXE installer. This just creates a zip folder. See .vscode/tasks.json
    install(CODE "
        file(GLOB DLLS \"${CMAKE_BINARY_DIR}/bin/\${CMAKE_INSTALL_CONFIG_NAME}/*.dll\")
        if(DLLS)
            file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}\" TYPE SHARED_LIBRARY FILES \${DLLS})
        endif()
    ")
    set(CPACK_GENERATOR "ZIP")
endif()

include(CPack)