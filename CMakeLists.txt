cmake_minimum_required(VERSION 3.15)
project(ChronoType VERSION 1.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/hook.cpp
    src/db.cpp
    src/server.cpp
    src/sqlite3.c
    src/tray.cpp
    src/resource.rc
)

# Executable with Windows subsystem (no console window by default, but we'll show console for debugging)
# To hide the console later, use WIN32 in add_executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

# Windows Specific Libraries (for SetWindowsHookEx and Ws2_32 for cpp-httplib)
target_link_libraries(${PROJECT_NAME} user32 Ws2_32)
