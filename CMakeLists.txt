# require version 3.10 or higher
cmake_minimum_required(VERSION 3.10)

#
# Project
#
#   - dictates the output executable filename (or library name on Android)
#
project(TestApp)

# Options you can set via command-line
option(HAS_TERMINAL "Show a terminal window for STDOUT/STDERR" ON)

#
# C_CXX_SOURCES_DIR
#
#   - the place where your C/C++ source files are located
#
set(C_CXX_SOURCES_DIR "src")

#
# C_CXX_HEADERS_DIR
#
#   - the place where your C/C++ header files are located
#
set(C_CXX_HEADERS_DIR "include")

#
# ASSETS_DIR
#
#   - the place where your pictures, sound files, etc.. live
#
set(ASSETS_DIR "assets")

##########################################################################
# DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING!!
##########################################################################

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# output executable basename (or library name on Android)
set(OutputExecutable "${CMAKE_PROJECT_NAME}")

######################################################################
# Directories

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# We need to specify the output for each configuration to make it work
# on Visual Studio solutions.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_PROFILE "${CMAKE_BINARY_DIR}/bin")

set(SOURCE_DATA_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_DIR})
set(SOURCE_CXX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${C_CXX_HEADERS_DIR})
set(SOURCE_CXX_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/${C_CXX_SOURCES_DIR})

# Source Files are Curated Here
file(
    GLOB_RECURSE SOURCE_CXX_FILES
    "${SOURCE_CXX_SRC_DIR}/*.cpp"
)

# Search in the "cmake" directory for additional CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

######################################################################
# Target creation
#
# On desktop (and most platforms) we build an executable.
# On Android, we must build a shared library (so it becomes lib<name>.so)
######################################################################
if(ANDROID)
    message(STATUS "Configuring for Android: building shared library")
    # set(NATIVE_APP_GLUE "/usr/lib/android-sdk/ndk-bundle/sources/android/native_app_glue/android_native_app_glue.c")
    add_library(${OutputExecutable} SHARED ${SOURCE_CXX_FILES})
    target_link_libraries(${OutputExecutable} log EGL GLESv2 android)
    # Define a macro for Android if needed
    target_compile_definitions(${OutputExecutable} PRIVATE __ANDROID__)
else()
        add_executable(${OutputExecutable} ${SOURCE_CXX_FILES})
endif()

######################################################################
# Platform-Specific Linking
######################################################################

# MacOS
if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    message(STATUS "Configuring for iOS")
    target_compile_definitions(${OutputExecutable} PRIVATE __APPLE__)
    target_compile_definitions(${OutputExecutable} PRIVATE GLES_SILENCE_DEPRECATION=1)
    enable_language(OBJCXX)
    # iOS frameworks (adjust based on your needs; this is a common starting set)
    target_link_libraries(${OutputExecutable}
        "-framework UIKit"
        "-framework Foundation"
        "-framework CoreGraphics"
        "-framework CoreAudio"
        "-framework CoreAudioKit"
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework GLKit"
        "-framework OpenGLES"
        "-framework QuartzCore")

        set(ios_resources
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/Main.storyboard"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/Contents.json"
        )
        # Add these files as sources (so they show up in the Xcode project)
        target_sources(${OutputExecutable} PRIVATE ${ios_resources})
        set_source_files_properties(${ios_resources}
            PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        
        # === Compiled iOS Source Files ===
        set(ios_sources
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/AppDelegate.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/GameViewController.mm"
            "${CMAKE_CURRENT_SOURCE_DIR}/ios/main.mm"
        )
        target_sources(${OutputExecutable} PRIVATE ${ios_sources} ${SOURCE_CXX_SRC_DIR})
        # Ensure .mm files compile as Objective-C++
        set_source_files_properties(${ios_sources} ${SOURCE_CXX_FILES} PROPERTIES LANGUAGE OBJCXX)
        
        # Instead of adding Info.plist as a source, you can specify it as the bundle’s plist:
        set_target_properties(${OutputExecutable} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist"
        )
elseif(APPLE)
    # macOS-specific desktop configuration
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    target_link_libraries(${OutputExecutable} ${CARBON_LIBRARY})
    find_package(GLUT REQUIRED)
    target_link_libraries(${OutputExecutable} ${GLUT_LIBRARIES})
    find_package(Threads REQUIRED)
    target_link_libraries(${OutputExecutable} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})
    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})
endif()

# Windows: MinGW
if(WIN32 AND MINGW)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    if (NOT HAS_TERMINAL)
        target_link_libraries(${OutputExecutable} -mwindows -municode)
    endif()

    # GDI+
    set(GDIPLUS_LIBRARY gdiplus)
    target_link_libraries(${OutputExecutable} ${GDIPLUS_LIBRARY})
    
    # Shlwapi
    set(SHLWAPI_LIBRARY shlwapi)
    target_link_libraries(${OutputExecutable} ${SHLWAPI_LIBRARY})
    
    # Dwmapi
    set(DWMAPI_LIBRARY dwmapi)
    target_link_libraries(${OutputExecutable} ${DWMAPI_LIBRARY})

    # stdc++fs
    target_link_libraries(${OutputExecutable} stdc++fs)

endif()

# Windows: Visual Studio / MSVC
if(WIN32 AND MSVC)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # Set startup project and debugger working directory
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${OutputExecutable})
    set_target_properties(${OutputExecutable} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    
    # Set subsystem: CONSOLE if HAS_TERMINAL is true; WINDOWS otherwise.
    if (HAS_TERMINAL)
        target_link_options(${OutputExecutable} PRIVATE "/SUBSYSTEM:CONSOLE")
    else()
        target_link_options(${OutputExecutable} PRIVATE "/SUBSYSTEM:WINDOWS")
    endif()

    # GDI+
    set(GDIPLUS_LIBRARY gdiplus)
    target_link_libraries(${OutputExecutable} ${GDIPLUS_LIBRARY})
    
    # Shlwapi
    set(SHLWAPI_LIBRARY shlwapi)
    target_link_libraries(${OutputExecutable} ${SHLWAPI_LIBRARY})
    
    # Dwmapi
    set(DWMAPI_LIBRARY dwmapi)
    target_link_libraries(${OutputExecutable} ${DWMAPI_LIBRARY})

endif() # Visual Studio / MSVC

# Linux (excluding Apple, Emscripten, and Android)
if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # X11
    find_package(X11 REQUIRED)
    target_link_libraries(${OutputExecutable} X11::X11)
    include_directories(${X11_INCLUDE_DIRS})

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${OutputExecutable} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})

    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})

    # stdc++fs
    target_link_libraries(${OutputExecutable} stdc++fs)

    # dl, for miniaudio
    target_link_libraries(${OutputExecutable} dl)

endif() # Linux

# Emscripten
if(EMSCRIPTEN)
    
    # Generate an HTML file
    set(CMAKE_EXECUTABLE_SUFFIX .html)

    # Build Cache: SDL2_mixer, libpng, zlib
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build sdl2_mixer libpng zlib)
    
    if(EXISTS "${SOURCE_DATA_DIR}" AND IS_DIRECTORY "${SOURCE_DATA_DIR}")
        target_link_options(${OutputExecutable} PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            -sUSE_LIBPNG=1
            -sLLD_REPORT_UNDEFINED
            --preload-file ${SOURCE_DATA_DIR}@assets)
    else()
        target_link_options(${OutputExecutable} PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            -sUSE_LIBPNG=1
            -sLLD_REPORT_UNDEFINED)
    endif()

endif() # Emscripten

# Android
if(ANDROID)
    message(STATUS "Adding Android system libraries")
    # On Android, the native code is built as a shared library (libTestApp.so)
    # Link with system libraries (log, EGL, GLESv2) that are needed by native apps.
    target_link_libraries(${OutputExecutable} log EGL GLESv2)
endif()

######################################################################
# Set include directory
######################################################################
if(IS_DIRECTORY ${SOURCE_CXX_INCLUDE_DIR})
    include_directories(${SOURCE_CXX_INCLUDE_DIR})
endif()

######################################################################
# Copy assets/ directory target
######################################################################

set(DATA_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${ASSETS_DIR})

file(GLOB_RECURSE src_data_files
    RELATIVE ${SOURCE_DATA_DIR}/ "${SOURCE_DATA_DIR}/*.*" "${SOURCE_DATA_DIR}/*")
foreach(fn ${src_data_files})
    add_custom_command(
        OUTPUT ${DATA_OUTPUT_DIR}/${fn}
        COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DATA_DIR}/${fn} ${DATA_OUTPUT_DIR}/${fn}
        MAIN_DEPENDENCY ${SOURCE_DATA_DIR}/${fn})
    list(APPEND out_data_files ${DATA_OUTPUT_DIR}/${fn})
endforeach()

add_custom_target(copy_data DEPENDS ${out_data_files})

# Copy Asset Files, if not Emscripten and (optionally) not Android
if (NOT EMSCRIPTEN AND NOT ANDROID AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    add_dependencies(${OutputExecutable} copy_data)
endif()
