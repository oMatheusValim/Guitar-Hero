#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "note_manager.h" // Inclui nosso novo manager
#include <vector>
#include <string>

// Os estados do nosso jogo
enum class GameState {
    MENU,
    SONG_SELECT,
    PLAYING,
    SCORE_SCREEN
};

class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();

private:
    // Variáveis de estado e Allegro
    bool running;
    GameState currentState;
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* font; 
    ALLEGRO_SAMPLE* hit_sound;
    ALLEGRO_SAMPLE* miss_sound;
    ALLEGRO_AUDIO_STREAM* music_stream; 

    // Gerenciador de Notas
    NoteManager noteManager;

    // Variáveis de Gameplay
    int score;
    int final_score; // Para guardar a pontuação ao final da música
    float song_position;
    std::string selectedSongPath;
    bool music_started;

    // Variáveis para a UI (Interface)
    std::vector<std::string> songList;
    int selectedSongIndex;
    int menu_option;
    int score_screen_option; // << NOVO: Para navegar na tela de score


    // Funções de loop principal, divididas por estado
    void processEvent(const ALLEGRO_EVENT& event);
    void update(float delta_time);
    void render();

    // Funções específicas de cada estado
    void updateMenu(const ALLEGRO_EVENT& event);
    void renderMenu();

    void updateSongSelect(const ALLEGRO_EVENT& event);
    void renderSongSelect();
    
    void updatePlaying(const ALLEGRO_EVENT& event, float delta_time);
    void renderPlaying();

    void updateScoreScreen(const ALLEGRO_EVENT& event);
    void renderScoreScreen();
    
    // Funções auxiliares
    void startPlaying();
    void endPlaying();
    void loadSongList();
};

#endif