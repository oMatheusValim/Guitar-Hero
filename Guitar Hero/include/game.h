#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include "note_manager.h"
#include <vector>
#include <string>

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
    bool running;
    GameState currentState;
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* font; 
    ALLEGRO_SAMPLE* hit_sound;
    ALLEGRO_SAMPLE* miss_sound;
    ALLEGRO_AUDIO_STREAM* music_stream; 

    NoteManager noteManager;

    int score;
    int final_score;
    float song_position;
    std::string selectedSongPath;
    bool music_started;

    std::vector<std::string> songList;
    int selectedSongIndex;
    int menu_option;
    int score_screen_option;

    void processEvent(const ALLEGRO_EVENT& event);
    void update(float delta_time);
    void render();
    void updateMenu(const ALLEGRO_EVENT& event);
    void renderMenu();
    void updateSongSelect(const ALLEGRO_EVENT& event);
    void renderSongSelect();
    void updatePlaying(const ALLEGRO_EVENT& event, float delta_time);
    void renderPlaying();
    void updateScoreScreen(const ALLEGRO_EVENT& event);
    void renderScoreScreen();
    void startPlaying();
    void endPlaying();
    void loadSongList();
};

#endif