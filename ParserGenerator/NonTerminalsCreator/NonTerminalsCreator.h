#include <map>
#include <string>
#include "../Utility/NonTerminal.h"
#ifndef PROJECT_NONTERMINALSCREATOR_H
#define PROJECT_NONTERMINALSCREATOR_H

class NonTerminalsCreator
{
public:
    void createFirst(std::map<std::string, std::shared_ptr<NonTerminal >> &nonTerminals);
    void createFollow(std::map<std::string, std::shared_ptr<NonTerminal >> &nonTerminals);
};

#endif // PROJECT_NONTERMINALSCREATOR_H
