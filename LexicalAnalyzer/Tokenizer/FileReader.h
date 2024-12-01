
#ifndef PROJECT_FILEREADER_H
#define PROJECT_FILEREADER_H


#include <iostream>
#include <fstream>
#include <string>
#include <queue>

class FileReader {
private:
    std::ifstream file;

public:
    // Constructor that accepts a file path
    explicit FileReader(const std::string& filePath);

    // Destructor to close the file
    ~FileReader();

    // Function to read the next line from the file
    std::queue<std::string> readLine();

    // Check if the file has more lines to read
    bool hasMoreLines();

    // Function to close the file
    void closeFile();
};



#endif //PROJECT_FILEREADER_H
