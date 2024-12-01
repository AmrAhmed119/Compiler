
#include <sstream>
#include "FileReader.h"


// Constructor that accepts a file path
FileReader::FileReader(const std::string& filePath) {
    file.open(filePath); // Open the file
    if (!file.is_open()) {
        throw std::ios_base::failure("Failed to open file: " + filePath);
    }
}

// Destructor to close the file
FileReader::~FileReader() {
    if (file.is_open()) {
        file.close();
    }
}

std::queue<std::string> split(std::string& str) {
    std::istringstream ss(str);
    std::string word;
    std::queue<std::string> words;
    while(ss >> word) {
        words.push(word);
    }
    return words;
}

// Function to read the next line from the file
std::queue<std::string> FileReader::readLine() {
    if (!file.is_open()) {
        throw std::ios_base::failure("File is not open.");
    }

    std::string line;
    if (std::getline(file, line)) {
        return split(line);
    } else if (file.eof()) {
        throw std::ios_base::failure("End of file reached.");
    } else {
        throw std::ios_base::failure("Error reading from file.");
    }
}

// Check if the file has more lines to read
bool FileReader::hasMoreLines() {
    return file.peek() != EOF; // Peek to check if we're at EOF
}

// Function to close the file
void FileReader::closeFile() {
    if (file.is_open()) {
        file.close();
    }
}
