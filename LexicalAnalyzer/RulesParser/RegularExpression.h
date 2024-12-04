//
// Created by hussein on 11/29/24.
//

#ifndef COMPILER_REGULAREXPRESSION_H
#define COMPILER_REGULAREXPRESSION_H


#include <string>
#include <vector>
#include "RegularDefinition.h"

class RegularExpression : public RegularDefinition {
    private:
        int priority{};
    public:
        RegularExpression(std::string name, std::string regex, int priority);
        RegularExpression();
        int getPriority();
};

#endif //COMPILER_REGULAREXPRESSION_H
