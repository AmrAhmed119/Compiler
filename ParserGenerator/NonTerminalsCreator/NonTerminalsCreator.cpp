#include <fstream>
#include <stdexcept>
#include <utility>
#include "NonTerminalsCreator.h"
#include "../../LexicalAnalyzer/Utility/Util.h"

std::pair<std::string, std::string> split(std::string& str, const char* delim);

std::pair<std::string, std::string> split(std::string& str, const char* delim) {
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

NonTerminalsCreator::NonTerminalsCreator(std::string CFGFilePath) : CFGFilePath(std::move(CFGFilePath)) {}

void NonTerminalsCreator::readCFGFile() {
    std::ifstream file(CFGFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("File not found");
    }
    std::string line;
    while (std::getline(file, line)) {
        auto [left, right] = split(line, "#");
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

void NonTerminalsCreator::processNonTerminal(std::string& rule) {
    auto [left, right] = split(rule, "=");
    if (left.empty() || right.empty()) {
        throw std::runtime_error("Invalid rule");
    }

    if (symbols.find(left) == symbols.end()) {
        symbols[left] = new NonTerminal(left);
    }

    auto* nonTerminal = dynamic_cast<NonTerminal*>(symbols[left]);


}

std::set<NonTerminal> NonTerminalsCreator::getNonTerminals() {
    std::set<NonTerminal> nonTerminals;
    for (auto &[_, symbol] : symbols) {
        if (auto* nonTerminal = dynamic_cast<NonTerminal*>(symbol)) {
            nonTerminals.insert(*nonTerminal);
        }
    }
    return nonTerminals;
}

std::vector<std::string> NonTerminalsCreator::getGrammarLines() {
    return _grammarLines;
}
