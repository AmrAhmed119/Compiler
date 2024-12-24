#ifndef PROJECT_PARSERTABLE_H
#define PROJECT_PARSERTABLE_H


#include <string>
#include "../Utility/NonTerminal.h"

class ParserTable {
public:

    std::map<std::string,std::shared_ptr<NonTerminal>> nonTerminals;

    // Constructor
    ParserTable(std::map<std::string,std::shared_ptr<NonTerminal>>& nonTerminals);

    void parserTableCreator();

    void printTable(std::string filePath);

};


#endif //PROJECT_PARSERTABLE_H
