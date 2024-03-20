cmake_minimum_required(VERSION 3.28)

project(XComp)

set(CMAKE_CXX_STANDARD 23)

set (
    SOURCE
    source/main.cpp
    source/filesys.cpp
    source/Compile/compile.cpp
    source/Lexer/lexer.cpp
    source/Token/token.cpp
)

add_executable(XComp ${SOURCE})