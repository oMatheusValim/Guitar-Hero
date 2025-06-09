#include "game.h"
#include <iostream>

int main() {
    Game game;
    
    if (!game.initialize()) {
        std::cerr << "Falha ao inicializar o jogo." << std::endl;
        return -1;
    }
    
    // O 'run' agora cuida de tudo: menu, jogo, etc.
    game.run();

    return 0;
}