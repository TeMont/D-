cmake_minimum_required(VERSION 3.28)

project(XComp)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE 
    source/main.cpp 
    source/filesys.cpp 
    source/Compile/compile.cpp
    source/Lexer/lexer.cpp 
    source/Token/token.cpp
)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        option(XCOMP_TESTS "build tests (or no)" ON)

        if(XCOMP_TESTS)
            add_library(example_tests ${SOURCE})
            enable_testing()
            add_subdirectory(tests)
        endif()
endif()

add_executable(XComp ${SOURCE})
