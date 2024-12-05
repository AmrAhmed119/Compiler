
#include <algorithm>
#include "RegularDefinition.h"
#include "../Utility/Util.h"

RegularDefinition::RegularDefinition(std::string name, std::string regex) {
    this->name = name;
    this->regex = regex;
}

RegularDefinition::RegularDefinition() = default;

const std::string RegularDefinition::getName() const {
    return name;
}

const std::string RegularDefinition::getRegex() const{
    return regex;
}

int RegularDefinition::enumerateRanges(std::string &str) {
    std::string result;

    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];

        if (c == '-' && i > 0 && i < str.size() - 1 && str[i-1] != '\\') {
            // Check for valid range boundaries
            char start = str[i - 1];
            if(start == ' ' && i-2>=0){
                start = str[i - 2];
                result.pop_back();
            }
            char end = str[i + 1];
            if(end == ' '&& i+2<str.size()){
                end = str[i + 2];
                i++;
            }

            if (!isalnum(start) || !isalnum(end)) {
                return -1; // Invalid range
            }

            // Remove the previous character (start of the range)
            result.pop_back();

            // Expand the range
            result += '(';
            for (char ch = start; ch <= end; ++ch) {
                result += "(" + std::string(1, ch) + ")|";
            }
            result.back() = ')'; // Replace the last '|' with ')'

            ++i; // Skip the next character (end of the range)
        } else {
            result += c;
        }
    }

    str = result;
    return 0;
}

void RegularDefinition::parenthesizeClosures(std::string &str) {
    std::string result;
    char delimiters[] = {' ', '+', '*', '|',  '('};
    for (int i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '+' || c == '*'){
            if (str[i-1] != '\\'){
                if (str[i-1] == ')') // Ignore already parenthesized closure.
                    continue;
                int j = i-1;
                while (j >= 0){
                    // Find the beginning of closure
                    if (std::find(std::begin(delimiters), std::end(delimiters), str[j]) != std::end(delimiters))
                        break;
                    j--;
                }
                str.insert(j+1, 1, '(');
                str.insert(i+1, 1, ')');
                i+=2;
            }
        }
    }
}

std::string RegularDefinition::parenthesizeDisjunctions(std::string &str) {
    std::string result;
    std::string accumulator;
    bool disjunctionFound = false;
    for (int i = 0; i < str.length(); i++){
        char c = str[i];
        if (c != '|' && c != '(')
            accumulator += c;
        else if (c == '('){
            // Handle parentheses by identifying the matching closing bracket
            int j = i+1;
            int moreBrackets = 1;
            while (j < str.length()){
                if (str[j] == '(')
                    moreBrackets++;
                if (str[j] == ')'){
                    moreBrackets--;
                    if (moreBrackets == 0)
                        break;
                }
                j++;
            }
            int endingBracket = j;
            // Get the string inside the brackets
            std::string betweenBrackets = str.substr(i+1, endingBracket - (i+1));
            // Process the substring inside parentheses recursively
            std::string betweenBracketsAfterModification = parenthesizeDisjunctions(betweenBrackets);
            accumulator +=  "(" + betweenBracketsAfterModification + ")";
            i = endingBracket;
        }
        else {
            bool spaceBefore = false;
            bool spaceAfter = false;
            if (str[i-1] == ' '){
                accumulator.pop_back();
                spaceBefore = true;
            }

            if (str[i+1] == ' ')
                spaceAfter = true;

            result += "(" + accumulator + ")";
            spaceBefore ? result += " |" : result += "|";
            if (spaceAfter){
                result += " ";
                i++;
            }
            disjunctionFound = true;
            accumulator.clear();
        }
    }

    if(disjunctionFound)
        result += "(" + accumulator + ")";
    else
        result += accumulator;

    return result;
}
int RegularDefinition::standardizeRegex(std::vector<RegularDefinition> regularDefinitions) {
    removeConsecutiveSpaces(regex);
    parenthesizeClosures(regex);
    regex = parenthesizeDisjunctions(regex);
    int statusCode = enumerateRanges(regex);
    if (statusCode == -1)
        return -1;
    replaceDefinitions(regex, std::move(regularDefinitions));
    return 0;
}
void RegularDefinition::replaceDefinitions(std::string &str, std::vector<RegularDefinition> regularDefinitions) {
    std::string result;
    std::string accumulator;

    for (int i = 0; i < str.length(); ++i) {
        char c = str[i];
        char reservedChars[] = {'=', '+', '*', '|', '(', ')'};

        if (c == ' ') {
            auto it = std::find_if(regularDefinitions.begin(), regularDefinitions.end(),
                                   [&accumulator](const RegularDefinition& def) {
                                       return def.name == accumulator;
                                   });
            if (it != regularDefinitions.end()) {
                result += "(" + it->regex + ")";
            } else {
                result += accumulator;
            }
            result += c;
            accumulator.clear();
        } else if (std::find(std::begin(reservedChars), std::end(reservedChars), c) != std::end(reservedChars)) {
            if (i > 0 && str[i - 1] == '\\') {
                accumulator += c;
            } else {
                auto it = std::find_if(regularDefinitions.begin(), regularDefinitions.end(),
                                       [&accumulator](const RegularDefinition& def) {
                                           return def.name == accumulator;
                                       });
                if (it != regularDefinitions.end()) {
                    result += "(" + it->regex + ")";
                } else {
                    result += accumulator;
                }
                result += c;
                accumulator.clear();
            }
        } else {
            accumulator += c;
        }
    }
    result += accumulator;
    str = result;
}

