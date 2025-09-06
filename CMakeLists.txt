#==============================================================================#
cmake_minimum_required(VERSION 3.24)
cmake_policy(SET CMP0115 NEW)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#==============================================================================#
# Settings                                                                     #
#==============================================================================#
set(APP_NAME "SALite")
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_BUILD 1)

set(APP_DEBUG true CACHE BOOL "Whether to build for debugging purposes")
set(BUILD_TESTING ON CACHE BOOL "Whether to enable testing mode")
set(GIT_SUBMODULE ON CACHE BOOL "Check submodules during build")
#==============================================================================#
project(${APP_NAME}
	LANGUAGES CXX
	VERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_BUILD}
	DESCRIPTION "BrownTurbo Gaming's Lite Client"
)
#==============================================================================#
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt libraries/*
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network)

# Source files
set(SOURCES
    src/utils.cpp
    src/xmlsets.cpp
    src/Game/dialogs.cpp
    src/Game/menus.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/logswindow.cpp
    src/settingswindow.cpp
    src/connectwindow.cpp
    src/playbackwindow.cpp
    src/math_stuff.cpp
    src/Networking/netgame.cpp
    src/Networking/netrpc.cpp
    src/common.cpp
    src/Game/cmds.cpp
    src/Game/localplayer.cpp
    src/Game/misc_funcs.cpp
    src/Game/PlayerPos.cpp
    src/RakNetWorker.cpp
    src/scorebwindow.cpp
    src/Game/CAnimationInfo.cpp
    src/Game/playback.cpp
    src/Game/recorder.cpp
    src/Game/audiostream.cpp
    src/logs.cpp
)

# Header files
set(HEADERS
    include/utils.h
    include/xmlsets.h
    include/Game/dialogs.h
    include/Game/menus.h
    include/main.h
    include/mainwindow.h
    include/logswindow.h
    include/settingswindow.h
    include/connectwindow.h
    include/playbackwindow.h
    include/CVector.h
    include/math_stuff.h
    include/Networking/netgame.h
    include/Networking/netrpc.h
    include/common.h
    include/Game/cmds.h
    include/Game/localplayer.h
    include/Game/misc_funcs.h
    include/Game/PlayerPos.h
    include/globals.h
    include/RakNetWorker.h
    include/ThreadWorker.h
    include/WorkerClass.h
    include/scorebwindow.h
    include/Game/CAnimationInfo.h
    include/Game/playback.h
    include/Game/recorder.h
    include/Game/audiostream.h
    include/logs.h
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# ...
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Multimedia Qt6::Network)

# Enable Qt-specific processing
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

set_target_properties(Qt6::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE")

# X11 ??
find_package(X11 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB RAKNET_SRC "lib/raknet/*.cpp")
file(GLOB RAKNETSA_SRC "lib/raknet/SAMP/*.cpp")
file(GLOB RAKNET_HDR "lib/raknet/*.h")
file(GLOB RAKNETSA_HDR "lib/raknet/SAMP/*.h")
add_library(RakNet ${RAKNET_SRC} ${RAKNETSA_SRC} ${RAKNET_HDR} ${RAKNETSA_HDR})
target_link_libraries(RakNet PRIVATE Qt6::Core)
set_property(TARGET RakNet PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(RakNet PUBLIC -DRAKNET_LEGACY -DPACKETS_PER_TICK=3 -D_RAKNET_THREADSAFE)

file(GLOB XML_SRC "lib/tinyxml/*.cpp")
file(GLOB XML_HDR "lib/tinyxml/*.h")
add_library(TinyXML ${XML_SRC} ${XML_HDR})

target_link_libraries(${PROJECT_NAME} PRIVATE RakNet TinyXML)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -lstdc++ -fsanitize=address") ## -fsanitize=thread

add_definitions(-DROOT_PATH="${CMAKE_SOURCE_DIR}")
