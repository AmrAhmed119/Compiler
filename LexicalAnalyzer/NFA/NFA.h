
#ifndef PROJECT_NFA_H
#define PROJECT_NFA_H

#include "../Utility/State.h"
#include <string>

class INFA {
public:
    virtual ~INFA() = default;
    // Create the NFA from the file
    virtual State& createNFA(const std::string& filePath) = 0;

};

class NFA : public INFA {
public:
    ~NFA() override = default;
    State& createNFA(const std::string& filePath) override;
};


#endif //PROJECT_NFA_H
