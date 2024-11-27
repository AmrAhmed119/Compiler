
#ifndef PROJECT_PARSER_H
#define PROJECT_PARSER_H

#include <string>

class IParser {
public:
    virtual ~IParser() = default;
    // Parse the file and create the NFA
    virtual void parse(const std::string& filePath) = 0;
};


class Parser : public IParser {
public:
    ~Parser() override = default;
    void parse(const std::string& filePath) override;
};


#endif //PROJECT_PARSER_H
