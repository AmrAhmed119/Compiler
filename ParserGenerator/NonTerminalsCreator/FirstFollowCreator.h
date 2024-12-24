#ifndef PROJECT_FIRSTFOLLOWCREATOR_H
#define PROJECT_FIRSTFOLLOWCREATOR_H

#include <fstream>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <regex>
#include <iostream>
#include "../../LexicalAnalyzer/Utility/Util.h"
#include "../Utility/Util.h"
#include "../Utility/NonTerminal.h"
#include "../Utility/Terminal.h"
#include "../Utility/Production.h"
#include "../Utility/Symbol.h"

class FirstFollowCreator
{
private:
public:
    ~FirstFollowCreator() = default;
    explicit FirstFollowCreator(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals);
};

#endif // PROJECT_FIRSTFOLLOWCREATOR_H
