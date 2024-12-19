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
        ParserGenerator/Utility/Production.cpp
        ParserGenerator/Utility/Production.h
        ParserGenerator/Utility/Production.h
        ParserGenerator/Utility/Symbol.cpp
        ParserGenerator/Utility/Symbol.h
        ParserGenerator/Utility/Terminal.cpp
        ParserGenerator/Utility/Terminal.h
        ParserGenerator/Utility/NonTerminal.cpp
        ParserGenerator/Utility/NonTerminal.h
        ParserGenerator/Utility/Util.cpp
        ParserGenerator/Utility/Util.h
        ParserGenerator/DerivationGenerator/DerivationGenerator.cpp
        ParserGenerator/DerivationGenerator/DerivationGenerator.h
        ParserGenerator/NonTerminalsCreator/NonTerminalsCreator.cpp
        ParserGenerator/NonTerminalsCreator/NonTerminalsCreator.h
        ParserGenerator/ParserTable/ParserTable.cpp
        ParserGenerator/ParserTable/ParserTable.h
)

