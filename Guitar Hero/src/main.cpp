#include "game.h"
#include <iostream>

int main() {
    Game game;
    
    if (!game.initialize()) {
        std::cerr << "Falha ao inicializar o jogo." << std::endl;
        return -1;
    }
    game.run();

    return 0;
}