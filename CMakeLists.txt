cmake_minimum_required(VERSION 3.14)
project(TerminalFestivalFramework CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /EHsc /utf-8)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

# Core library (pure header-only interface)
add_library(festival_core INTERFACE)
target_include_directories(festival_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Standalone core verification target (strictly NO themes)
add_executable(core_test tests/core_standalone_test.cpp)
target_link_libraries(core_test PRIVATE festival_core)

# Main Festival Application (Festival Core + Mid-Autumn Theme)
add_executable(festival_app festival/main.cpp)
target_link_libraries(festival_app PRIVATE festival_core)
