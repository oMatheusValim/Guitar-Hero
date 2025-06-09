#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <vector>
#include <string>

// Agora o FileHandler só lida com operações genéricas de arquivo.
class FileHandler {
public:
    // Lista todos os arquivos em um diretório (implementação simplificada)
    static std::vector<std::string> listFiles(const std::string& directoryPath);
    static bool saveScore(const std::string& filename, int score);
};

#endif