cmake_minimum_required(VERSION 3.20)

# --- pick the board: cmake -B build -DBOARD=dq8rly ---
set(BOARD "" CACHE STRING "Board to build (folder name under boards/)")

# Discover available boards for a friendly error / listing.
file(GLOB _board_dirs RELATIVE ${CMAKE_SOURCE_DIR}/boards ${CMAKE_SOURCE_DIR}/boards/*)
set(AVAILABLE_BOARDS "")
foreach(_d ${_board_dirs})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/boards/${_d})
        list(APPEND AVAILABLE_BOARDS ${_d})
    endif()
endforeach()

if(NOT BOARD)
    message(FATAL_ERROR
        "No BOARD selected. Configure with -DBOARD=<name>.\n"
        "Available boards: ${AVAILABLE_BOARDS}")
endif()
if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/boards/${BOARD})
    message(FATAL_ERROR
        "Unknown BOARD '${BOARD}'.\nAvailable boards: ${AVAILABLE_BOARDS}")
endif()

project(i2c-hat-${BOARD} C CXX ASM)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/i2c_hat_board.cmake)

message(STATUS "Building I2C-HAT firmware for board: ${BOARD}")
add_subdirectory(boards/${BOARD})
