#include <fstream>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <regex>
#include <iostream>
#include "NonTerminalsCreator.h"
#include "../../LexicalAnalyzer/Utility/Util.h"
#include "../Utility/Util.h"

const std::string EPSILON = "\\L";

std::pair<std::string, std::string> splitLine(std::string& str, const char* delim);
std::vector<std::string> splitProduction(const std::string& str, char delimiter);
std::map<std::string, std::shared_ptr<NonTerminal>> getNonTerminalsMap(const std::set<std::shared_ptr<NonTerminal>>& nonTerminals);
std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> &nt);
std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> &production);
std::set<std::shared_ptr<Terminal>> setsUnion(const std::vector<std::set<std::shared_ptr<Terminal>>> &unexpandedSet);
bool hasEps(const std::set<std::shared_ptr<Terminal>> &expandedSet);
void addTerminalsAndFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals);
void addDollarForStart(std::shared_ptr<NonTerminal> &nonTerminal);
void terminalAndFirstToProduction(std::shared_ptr<Production> &production);
bool terminalAndFirstToNt(std::shared_ptr<NonTerminal> &cur, std::shared_ptr<Symbol> &next);
void removeEpsFromSet(std::set<std::shared_ptr<Terminal>> &curSet);
void addFollows(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals);
void addFollowForProduction(std::shared_ptr<Production> &production, std::shared_ptr<NonTerminal> &head);
void followToNt(std::shared_ptr<NonTerminal> &nonTerminal, std::shared_ptr<NonTerminal> &head);

std::map<std::string, std::shared_ptr<NonTerminal>> getNonTerminalsMap(const std::set<std::shared_ptr<NonTerminal>>& nonTerminals) {
    std::map<std::string, std::shared_ptr<NonTerminal>> nonTerminalsMap;
    for (const auto& nonTerminal : nonTerminals) {
        nonTerminalsMap[nonTerminal->getName()] = nonTerminal;
    }
    return nonTerminalsMap;
}

std::pair<std::string, std::string> splitLine(std::string& str, const char* delim) {
    int pos = (int)str.find(delim);
    trimBlanksFromEnds(str);
    if (pos == std::string::npos) {
        return {str, ""};
    }
    std::string left = str.substr(0, pos);
    std::string right = str.substr(pos + 1);
    trimBlanksFromEnds(left);
    trimBlanksFromEnds(right);
    return {left, right};
}

std::vector<std::string> splitProduction(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        trimBlanksFromEnds(token);
        result.push_back(token);
    }

    return result;
}

NonTerminalsCreator::NonTerminalsCreator(std::string CFGFilePath) : CFGFilePath(std::move(CFGFilePath)) {}

void NonTerminalsCreator::readCFGFile() {
    std::ifstream file(CFGFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("File not found");
    }
    std::string line;
    while (std::getline(file, line)) {
        auto [left, right] = splitLine(line, "#");
        if (!right.empty()) {
            if (!left.empty()) {
                _grammarLines.back() += " " + left;
            }
            _grammarLines.push_back(right);
        } else {
            _grammarLines.back() += " " + left;
        }
    }
    file.close();
}

std::set<std::shared_ptr<NonTerminal>> NonTerminalsCreator::getNonTerminals() {
    std::set<std::shared_ptr<NonTerminal>> nonTerminals;
    for (auto &[_, symbol] : symbols) {
        if (auto nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbol)) {
            nonTerminals.insert(nonTerminal);
        }
    }
    return nonTerminals;
}

std::vector<std::string> NonTerminalsCreator::getGrammarLines() {
    return _grammarLines;
}

std::shared_ptr<NonTerminal> NonTerminalsCreator::createNonTerminalOrGetIfExists(const std::string &name) {
    if (symbols.find(name) == symbols.end()) {
        symbols[name] = std::make_shared<NonTerminal>(name);
    }
    return std::dynamic_pointer_cast<NonTerminal>(symbols[name]);
}

std::shared_ptr<Terminal> NonTerminalsCreator::createTerminalOrGetIfExists(const std::string &name) {
    if (symbols.find(name) == symbols.end()) {
        symbols[name] = std::make_shared<Terminal>(name, name == EPSILON);
    }
    return std::dynamic_pointer_cast<Terminal>(symbols[name]);
}

void NonTerminalsCreator::processNonTerminal(std::string& rule, bool isStarting) {
    auto [left, right] = splitLine(rule, "=");
    if (left.empty() || right.empty()) {
        throw std::runtime_error("Invalid rule");
    }

    auto nonTerminal = createNonTerminalOrGetIfExists(left);
    if (isStarting) {
        nonTerminal->setIsStarting(true);
    }

    std::vector<std::string> productions = splitProduction(right, '|');
    for (const auto& production : productions) {
        nonTerminal->addProduction(processProduction(production));
    }
}

std::shared_ptr<Production> NonTerminalsCreator::processProduction(const std::string &rule) {
    std::shared_ptr<Production> production = std::make_shared<Production>();
    if (rule == EPSILON) {
        production->addSymbol(createTerminalOrGetIfExists(EPSILON));
        return production;
    }
    std::regex productionRegex("'([^']*)'|([^']+)");
    std::sregex_iterator iter(rule.begin(), rule.end(), productionRegex);
    std::sregex_iterator end;

    while (iter != end) {
        if ((*iter)[1].matched) {
            production->addSymbol(createTerminalOrGetIfExists((*iter)[1].str()));
        } else if ((*iter)[2].matched) {
            std::string nonTerminal = (*iter)[2].str();
            std::istringstream stream(nonTerminal);
            std::string token;
            while (stream >> token) {
                if (token.empty()) {
                    continue;
                }
                production->addSymbol(createNonTerminalOrGetIfExists(token));
            }
        }
        ++iter;
    }

    return production;
}

std::set<std::shared_ptr<NonTerminal>> NonTerminalsCreator::createNonTerminals() {
    for (int i = 0; i < _grammarLines.size(); i++) {
        processNonTerminal(_grammarLines[i], i == 0);
    }
    auto nonTerminalsMap = getNonTerminalsMap(getNonTerminals());
    //createFirst(nonTerminalsMap);
//    createFollow(nonTerminalsMap);
    return getNonTerminals();
}

std::shared_ptr<NonTerminal> NonTerminalsCreator::getStartingNonTerminal() {
    auto nonTerminals = getNonTerminals();
    for (const auto& nonTerminal : nonTerminals) {
        if (nonTerminal->getIsStarting()) {
            return nonTerminal;
        }
    }
    return nullptr;
}

void NonTerminalsCreator::printNonTerminals(const std::set<std::shared_ptr<NonTerminal>>& nonTerminals) {
    for (const auto& nonTerminal : nonTerminals) {
        if (nonTerminal->getIsStarting()) {
            std::cout << "Starting State: " << nonTerminal->getName() << std::endl;
            break;
        }
    }
    for (const auto& nonTerminal : nonTerminals) {
        std::string x = nonTerminal->getIsStarting() ? " (Starting State) " : "";
        std::cout << nonTerminal->getName() << x << " -> ";
        for (const auto& production : nonTerminal->getProductions()) {
            for (const auto& symbol : production->getSymbols()) {
                if (auto term = std::dynamic_pointer_cast<Terminal>(symbol)) {
                    std::string y = term->getIsEpsilon() ? " (EPS) " : "";
                    std::cout << "'" << term->getName() << "' " << y << " ";
                } else if (auto nonTerm = std::dynamic_pointer_cast<NonTerminal>(symbol)) {
                    std::cout << symbol->getName() << " ";
                }
            }
            if (nonTerminal->getProductions().back() != production) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
}

void NonTerminalsCreator::createFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nt : nonTerminals)
    {
        std::shared_ptr<NonTerminal> cur = nt.second;
        std::vector<std::set<std::shared_ptr<Terminal>>> firsts = calcFirstOfNT(cur);
        cur->setFirst(firsts);
    }
}

std::vector<std::set<std::shared_ptr<Terminal>>> calcFirstOfNT(std::shared_ptr<NonTerminal> &nt)
{
    std::vector<std::set<std::shared_ptr<Terminal>>> firstsOfNt;
    if (!nt->getFirst().empty())
        firstsOfNt = nt->getFirst();
    else
        for (std::shared_ptr<Production> production : nt->getProductions())
        {
            std::set<std::shared_ptr<Terminal>> firstOfProduction = calcFirstOfProduction(production);
            firstsOfNt.push_back(firstOfProduction);
        }
    return firstsOfNt;
}

std::set<std::shared_ptr<Terminal>> calcFirstOfProduction(std::shared_ptr<Production> &production)
{
    std::set<std::shared_ptr<Terminal>> firstOfProduction;

    for (std::shared_ptr<Symbol> symbol : production->getSymbols())
    {
        // Use std::dynamic_pointer_cast for casting shared_ptr
        if (std::shared_ptr<Terminal> terminal = std::dynamic_pointer_cast<Terminal>(symbol))
        {
            firstOfProduction.insert(terminal);
            break;
        }
        else if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbol))
        {
            std::vector<std::set<std::shared_ptr<Terminal>>> unexpandedSet = calcFirstOfNT(nonTerminal);
            std::set<std::shared_ptr<Terminal>> expandedSet = setsUnion(unexpandedSet);
            firstOfProduction.insert(expandedSet.begin(), expandedSet.end());

            // If expandedSet does not contain epsilon, break the loop
            if (!hasEps(expandedSet))
                break;
        }
        else
        {
            // Handle invalid symbol types
            print("Wrong symbol type");
        }
    }

    return firstOfProduction;
}

std::set<std::shared_ptr<Terminal>> setsUnion(const std::vector<std::set<std::shared_ptr<Terminal>>> &unexpandedSet)
{
    std::set<std::shared_ptr<Terminal>> expandedSet;
    for (std::set<std::shared_ptr<Terminal>> terminlsSet : unexpandedSet)
        expandedSet.insert(terminlsSet.begin(), terminlsSet.end());
    return expandedSet;
}

bool hasEps(const std::set<std::shared_ptr<Terminal>> &expandedSet)
{
    for (std::shared_ptr<Terminal> terminal : expandedSet)
        if (terminal->getIsEpsilon())
            return true;
    return false;
}

void NonTerminalsCreator::createFollow(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    addTerminalsAndFirst(nonTerminals);
    addFollows(nonTerminals);
}

void addTerminalsAndFirst(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nonTerminal : nonTerminals)
    {
        addDollarForStart(nonTerminal.second);
        for (std::shared_ptr<Production> production : nonTerminal.second->getProductions())
            terminalAndFirstToProduction(production);
    }
}

void addDollarForStart(std::shared_ptr<NonTerminal> &nonTerminal)
{
    if (nonTerminal->getIsStarting())
    {
        std::set<std::shared_ptr<Terminal>> curSet;
        std::shared_ptr<Terminal> endText = std::make_shared<Terminal>("$", false);
        curSet.insert(endText);
        nonTerminal->setFollow(curSet);
    }
}

void terminalAndFirstToProduction(std::shared_ptr<Production> &production)
{
    std::vector<std::shared_ptr<Symbol>> symbols = production->getSymbols();
    int numSymbols = symbols.size();
    for (int symbIndx = 0; symbIndx < numSymbols; symbIndx++)
    {
        if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbols[symbIndx]))
        {
            int next = symbIndx + 1;
            bool hasNext = true;
            while (next != numSymbols && hasNext)
                hasNext = terminalAndFirstToNt(nonTerminal, symbols[next++]);
        }
    }
}

bool terminalAndFirstToNt(std::shared_ptr<NonTerminal> &cur, std::shared_ptr<Symbol> &next)
{
    std::set<std::shared_ptr<Terminal>> curSet = cur->getFollow();
    bool ans = false;
    removeEpsFromSet(curSet);
    if (std::shared_ptr<Terminal> terminal = std::dynamic_pointer_cast<Terminal>(next))
    {
        curSet.insert(terminal);
        ans = false;
    }
    else
    {
        std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(next);
        std::set<std::shared_ptr<Terminal>> nextFirst = setsUnion(nonTerminal->getFirst());
        curSet.insert(nextFirst.begin(), nextFirst.end());
        if (hasEps(nextFirst))
            ans = true;
        else
            ans = false;
    }
    cur->setFollow(curSet);
    return ans;
}

void removeEpsFromSet(std::set<std::shared_ptr<Terminal>> &curSet)
{
    for (std::shared_ptr<Terminal> terminal : curSet)
        if (terminal->getIsEpsilon())
        {
            curSet.erase(terminal);
            break;
        }
}

void addFollows(std::map<std::string, std::shared_ptr<NonTerminal>> &nonTerminals)
{
    for (auto nonTerminal : nonTerminals)
    {
        addDollarForStart(nonTerminal.second);
        for (std::shared_ptr<Production> production : nonTerminal.second->getProductions())
            addFollowForProduction(production, nonTerminal.second);
    }
}

void addFollowForProduction(std::shared_ptr<Production> &production, std::shared_ptr<NonTerminal> &head)
{
    std::vector<std::shared_ptr<Symbol>> symbols = production->getSymbols();
    int numSymbols = symbols.size();
    for (int symbIndx = 0; symbIndx < numSymbols; symbIndx++)
    {
        if (std::shared_ptr<NonTerminal> nonTerminal = std::dynamic_pointer_cast<NonTerminal>(symbols[symbIndx]))
            if (symbIndx == numSymbols - 1 || hasEps(nonTerminal->getFollow()))
                followToNt(nonTerminal, head);
    }
}

void followToNt(std::shared_ptr<NonTerminal> &nonTerminal, std::shared_ptr<NonTerminal> &head)
{
    std::set<std::shared_ptr<Terminal>> curSet = nonTerminal->getFollow();
    removeEpsFromSet(curSet);
    curSet.insert(head->getFollow().begin(), head->getFollow().end());
    nonTerminal->setFollow(curSet);
}
