cmake_minimum_required(VERSION 3.28)

project(XComp)

set(CMAKE_CXX_STANDARD 23)

set (
    SOURCE
    "src/main.cpp"
    "src/filesys.cpp"
)

add_executable(XComp ${SOURCE})