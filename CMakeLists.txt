cmake_minimum_required(VERSION 3.28)

project(DCompiler)

set(CMAKE_CXX_STANDARD 23)

set (
    SOURCE
    "src/main.cpp"
)

add_executable(DCompiler ${SOURCE})