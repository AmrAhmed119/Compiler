cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project main.cpp
        LexicalAnalyzer/Utility/State.cpp
        LexicalAnalyzer/Parser/Parser.cpp
        LexicalAnalyzer/NFA/NFA.cpp
        LexicalAnalyzer/NFA/NFA.h
        LexicalAnalyzer/DFA/DFA.cpp
        LexicalAnalyzer/DFA/DFA.h
        LexicalAnalyzer/Tokenizer/Tokenizer.cpp
        LexicalAnalyzer/Tokenizer/Tokenizer.h
        LexicalAnalyzer/Tokenizer/FileReader.cpp
        LexicalAnalyzer/Tokenizer/FileReader.h
        LexicalAnalyzer/Tokenizer/Token.cpp
        LexicalAnalyzer/Tokenizer/Token.h)
