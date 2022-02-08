cmake_minimum_required (VERSION 3.15 FATAL_ERROR)
project (Nonogram
  VERSION 0.0.1
  LANGUAGES CXX
)

set (CMAKE_AUTOMOC ON)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# compile warning flags
set (CXX_FLAGS -W
               -Wall
               -Werror
               -Wextra
               -Wimplicit-fallthrough
               -Wgnu-anonymous-struct
               -Wnon-virtual-dtor
               -Wextra-semi
               -Wextra-semi-stmt
               -Wshadow-field-in-constructor
               -Wshadow-uncaptured-local
               -Wunreachable-code
               -Wunused
               -Wunused-member-function
               -Wno-switch-bool
)

set (CMAKE_CXX_FLAGS "${FLAGS}")

find_package (Qt5 5.9 REQUIRED
  COMPONENTS Core
             Gui
             Help
             LinguistTools
             OpenGL
             Sql
             Svg
             Widgets
)

include_directories ("${PROJECT_SOURCE_DIR}/src")

add_subdirectory (src/nonogram)

install (DIRECTORY "puzzles/" DESTINATION "puzzles")
install (DIRECTORY DESTINATION "answers")
