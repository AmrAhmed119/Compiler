//
// Created by hussein on 11/29/24.
//

#ifndef COMPILER_REGULARDEFINITION_H
#define COMPILER_REGULARDEFINITION_H

#include <string>
#include <vector>

class RegularDefinition {

    private:
        std::string name;
        std::string regex;
        int enumerateRanges(std::string &regex);
        void parenthesizeClosures(std::string &regex);
    public:
        RegularDefinition(std::string name, std::string regex);
        RegularDefinition();

        const std::string getName() const;
        const std::string getRegex() const;

    void replaceDefinitions(std::string &str, std::vector<RegularDefinition> regularDefinitions);

    int standardizeRegex(std::vector<RegularDefinition> regularDefinitions);

    std::string parenthesizeDisjunctions(std::string &str);
};


#endif //COMPILER_REGULARDEFINITION_H
