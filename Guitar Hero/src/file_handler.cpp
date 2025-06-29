#include "file_handler.h"
#include <fstream>

bool FileHandler::saveScore(const std::string& filename, int score) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    file << "Pontuação Final: " << score << std::endl;
    return true;
}

std::vector<std::string> FileHandler::listFiles(const std::string& directoryPath) {
    std::vector<std::string> files;
    
    // Adiciona músicas 
    files.push_back("assets/songs/Paint_It_Black.txt");
    files.push_back("assets/songs/Legendary.txt");
    files.push_back("assets/songs/Jojo.txt");
    
    return files;
}