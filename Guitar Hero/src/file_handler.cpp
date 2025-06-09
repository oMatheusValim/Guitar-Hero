#include "file_handler.h"
#include <fstream>
// Para listar arquivos, precisaríamos de uma biblioteca ou código específico do SO.
// No Linux, poderíamos usar <dirent.h>. Vamos deixar um placeholder por enquanto.
// #include <dirent.h>

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
    
    // Adicionar músicas músicas 
    files.push_back("assets/songs/Paint_It_Black.txt");
    files.push_back("assets/songs/Legendary.txt");
    files.push_back("Jojo.txt");
    // files.push_back("assets/songs/another_song.txt");
    
    return files;
}