cmake_minimum_required(VERSION 3.27)
project(Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project main.cpp
        LexicalAnalyzer/Utility/State.cpp
        LexicalAnalyzer/NFA/NFA.cpp
        LexicalAnalyzer/DFA/DFA.cpp
        LexicalAnalyzer/RulesParser/RulesParser.cpp
        LexicalAnalyzer/RulesParser/RegularExpression.cpp
        LexicalAnalyzer/RulesParser/RegularDefinition.cpp
        LexicalAnalyzer/Utility/Util.cpp
        LexicalAnalyzer/Tokenizer/Tokenizer.cpp
        LexicalAnalyzer/Tokenizer/Token.cpp
        LexicalAnalyzer/Tokenizer/FileReader.cpp
        LexicalAnalyzer/NFA/process_tokens.cpp
        )

