cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project main.cpp
        LexicalAnalyzer/Utility/State.cpp
        LexicalAnalyzer/Parser/Parser.cpp
        LexicalAnalyzer/NFA/NFA.cpp
        LexicalAnalyzer/NFA/process_tokens.cpp

)
