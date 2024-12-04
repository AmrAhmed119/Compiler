//
// Created by hussein on 11/29/24.
//

#include "Util.h"
#include <sstream>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::pair<std::string, std::string> splitIntoTwo(const std::string& str, char delimiter){
    size_t pos = str.find(delimiter);
    if (pos != std::string::npos){
        std::string before = str.substr(0, pos);
        std::string after = str.substr(pos + 1);
        return make_pair(before, after);
    }
    return make_pair(str, std::string {});
}

void trimBlanksFromEnds(std::string& str){
    const std::string& blanks = " \t\n\r";
    str.erase(str.find_last_not_of(blanks)+1);
    str.erase(0, str.find_first_not_of(blanks));
}

void removeConsecutiveSpaces(std::string& str) {
    std::string result;
    bool seenSpace = false;

    for (char ch : str) {
        if (std::isspace(ch)) {
            if (!seenSpace) {
                result += ' ';
                seenSpace = true;
            }
        } else {
            result += ch;
            seenSpace = false;
        }
    }
    str = result;
}

void removeFirstAndLastChars(std::string& str){
    if (str.size()>2) {
        str.erase(0, 1);
        str.erase(str.length()-1);
    }
}


