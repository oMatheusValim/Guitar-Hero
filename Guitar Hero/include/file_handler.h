#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <vector>
#include <string>

class FileHandler {
public:
    static std::vector<std::string> listFiles(const std::string& directoryPath);
    static bool saveScore(const std::string& filename, int score);
};

#endif