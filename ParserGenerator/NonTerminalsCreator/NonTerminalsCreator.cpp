#include <fstream>
#include <stdexcept>
#include <utility>
#include <sstream>
#include <regex>
#include <iostream>
#include "NonTerminalsCreator.h"
#include "../../LexicalAnalyzer/Utility/Util.h"

const std::string EPSILON = "\\L";

std::pair<std::string, std::string> splitLine(std::string& str, const char* delim);
std::vector<std::string> splitProduction(const std::string& str, char delimiter);

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


