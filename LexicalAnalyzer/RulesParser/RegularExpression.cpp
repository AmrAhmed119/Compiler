
#include "RegularExpression.h"

#include <utility>



RegularExpression::RegularExpression(std::string name, std::string regex, int priority)
        : RegularDefinition(std::move(name), std::move(regex)), priority(priority) {}

RegularExpression::RegularExpression() = default;


int RegularExpression::getPriority() {
    return priority;
}

