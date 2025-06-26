#include "game.h"
#include "file_handler.h"
#include <iostream>
#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

Game::Game() : 
    running(false), currentState(GameState::MENU), display(nullptr), 
    event_queue(nullptr), timer(nullptr), font(nullptr), 
    hit_sound(nullptr), miss_sound(nullptr), music_stream(nullptr),
    score(0), final_score(0), song_position(0.0f), 
    selectedSongIndex(0), menu_option(0), score_screen_option(0), music_started(false) {}

Game::~Game() {
    if (music_stream) al_destroy_audio_stream(music_stream);
    if (hit_sound) al_destroy_sample(hit_sound);
    if (miss_sound) al_destroy_sample(miss_sound);
    if (font) al_destroy_font(font);
    if (timer) al_destroy_timer(timer);
    if (event_queue) al_destroy_event_queue(event_queue);
    if (display) al_destroy_display(display);
}

bool Game::initialize() {
    if (!al_init()) return false;
    if (!al_install_keyboard()) return false;
    if (!al_install_mouse()) return false;
    if (!al_init_primitives_addon()) return false;
    al_init_font_addon();
    if (!al_init_ttf_addon()) return false;
    if (!al_install_audio() || !al_init_acodec_addon()) return false;
    
    al_reserve_samples(10);

    display = al_create_display(800, 600);
    timer = al_create_timer(1.0 / 60.0);
    event_queue = al_create_event_queue();

    if (!display || !timer || !event_queue) return false;
    
    font = al_load_ttf_font("assets/fonts/font.ttf", 32, 0); 
    if (!font) {
        font = al_create_builtin_font();
    }
    
    hit_sound = al_load_sample("assets/sounds/hit.wav");
    miss_sound = al_load_sample("assets/sounds/miss.wav");

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    return true;
}

// Run (Loop principal)
void Game::run() {
    running = true;
    al_start_timer(timer);
    bool redraw = true;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            update(1.0 / 60.0);
            redraw = true;
        } else {
             processEvent(event);
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            render();
            al_flip_display();
        }
    }
}

// ProcessEvent (Delega eventos)
void Game::processEvent(const ALLEGRO_EVENT& event) {
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        running = false;
        return;
    }
    if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
         // ESC volta para o menu principal, ou sai do jogo se já estiver no menu
        if (currentState != GameState::MENU) {
            endPlaying(); // Encerra a música se estiver tocando
            currentState = GameState::MENU;
        } else {
            running = false;
        }
        return;
    }


    switch (currentState) {
        case GameState::MENU:          updateMenu(event); break;
        case GameState::SONG_SELECT:   updateSongSelect(event); break;
        case GameState::PLAYING:       updatePlaying(event, 0); break;
        case GameState::SCORE_SCREEN:  updateScoreScreen(event); break;
    }
}

void Game::update(float delta_time) {
    if (currentState == GameState::PLAYING) {
        updatePlaying({}, delta_time);
    }
}

void Game::render() {
    al_clear_to_color(al_map_rgb(20, 20, 40)); 
    switch (currentState) {
        case GameState::MENU:          renderMenu(); break;
        case GameState::SONG_SELECT:   renderSongSelect(); break;
        case GameState::PLAYING:       renderPlaying(); break;
        case GameState::SCORE_SCREEN:  renderScoreScreen(); break;
    }
}

// --- LÓGICA DO MENU 
void Game::updateMenu(const ALLEGRO_EVENT& event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:   menu_option = (menu_option + 1) % 2; break;
            case ALLEGRO_KEY_DOWN: menu_option = (menu_option + 1) % 2; break;
            case ALLEGRO_KEY_ENTER:
                if (menu_option == 0) {
                    loadSongList();
                    currentState = GameState::SONG_SELECT;
                } else { 
                    running = false;
                }
                break;
        }
    }
}
void Game::renderMenu() {
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 100, ALLEGRO_ALIGN_CENTER, "Guitar Hero FOseus");
    ALLEGRO_COLOR play_color = (menu_option == 0) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR exit_color = (menu_option == 1) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    al_draw_text(font, play_color, 400, 300, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, exit_color, 400, 500, ALLEGRO_ALIGN_CENTER, "Sair");
}

// --- LÓGICA DA SELEÇÃO DE MÚSICA ---
void Game::loadSongList() {
    songList = FileHandler::listFiles("assets/songs");
    selectedSongIndex = 0;
}

void Game::updateSongSelect(const ALLEGRO_EVENT& event) {
    if (songList.empty()) {
        if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ENTER)
            currentState = GameState::MENU;
        return;
    }

    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch(event.keyboard.keycode) {
            case ALLEGRO_KEY_DOWN:
                selectedSongIndex = (selectedSongIndex + 1) % songList.size();
                break;
            case ALLEGRO_KEY_UP:
                selectedSongIndex = (selectedSongIndex == 0) ? songList.size() - 1 : selectedSongIndex - 1;
                break;
            case ALLEGRO_KEY_ENTER:
                selectedSongPath = songList[selectedSongIndex];
                startPlaying();
                break;
        }
    }
}

void Game::renderSongSelect() {
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTER, "Selecione uma Musica");

    if (songList.empty()) {
        al_draw_text(font, al_map_rgb(255, 0, 0), 400, 250, ALLEGRO_ALIGN_CENTER, "Nenhuma musica encontrada!");
        al_draw_text(font, al_map_rgb(200,200,200), 400, 550, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para voltar");
        return;
    }
    
    for (int i = 0; i < songList.size(); ++i) {
        ALLEGRO_COLOR color = (i == selectedSongIndex) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        
        std::string fullPath = songList[i];
        size_t last_slash = fullPath.find_last_of("/\\");
        std::string filename = (last_slash == std::string::npos) ? fullPath : fullPath.substr(last_slash + 1);
        size_t last_dot = filename.find_last_of(".");
        std::string songName = (last_dot == std::string::npos) ? filename : filename.substr(0, last_dot);

        // Substitui underscores por espaços
        std::replace(songName.begin(), songName.end(), '_', ' ');

        al_draw_text(font, color, 400, 200 + i * 40, ALLEGRO_ALIGN_CENTER, songName.c_str());
    }
    
    al_draw_text(font, al_map_rgb(200,200,200), 400, 550, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para jogar ou ESC para voltar");
}

// --- LÓGICA DO JOGO
void Game::startPlaying() {
    score = 0;
    song_position = 0;
    music_started = false;
    noteManager.reset();
    noteManager.loadSong(selectedSongPath);
    
    float startTime = noteManager.getStartTime();
    song_position = startTime; // Inicia a variável de tempo na posição 

    std::string audioPath = selectedSongPath;
    size_t dotPos = audioPath.rfind('.');
    if (dotPos != std::string::npos) audioPath.replace(dotPos, std::string::npos, ".ogg");

    music_stream = al_load_audio_stream(audioPath.c_str(), 4, 2048);
    if (music_stream) {
        al_attach_audio_stream_to_mixer(music_stream, al_get_default_mixer());
        al_set_audio_stream_playing(music_stream, true);
        // Se houver um tempo de início, pula para ele
        if (startTime > 0) {
            al_seek_audio_stream_secs(music_stream, startTime);
        }
        music_started = true;
    } 

    currentState = GameState::PLAYING;
}

// Lógica de fim de jogo
void Game::updatePlaying(const ALLEGRO_EVENT& event, float delta_time) {
    float startTime = noteManager.getStartTime();
    // Tempo de jogo excedeu 90 segundos
    if (song_position - startTime >= 90.0f) {
        endPlaying();
        return;
    }

    // A música com áudio terminou de tocar naturalmente
    if (music_started && music_stream && !al_get_audio_stream_playing(music_stream)) {
        endPlaying();
        return;
    }

    // lógica de avanço de tempo
    if (delta_time > 0) {
        if (music_started && music_stream) {
            song_position = al_get_audio_stream_position_secs(music_stream);
        } else {
            song_position += delta_time;
        }
        noteManager.update(song_position, delta_time);
    }
    
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        int points = noteManager.checkHit(event.keyboard.keycode);
        if (points > 0) { 
            score += points;
            if (hit_sound) {
                al_play_sample(hit_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
            }
        }
    }
}

// Renderização das pistas visuais
void Game::renderPlaying() {
    al_draw_filled_rectangle(190, 0, 610, 600, al_map_rgb(25, 25, 25));
    for (int i = 0; i < 5; ++i) {
        al_draw_line(200 + i * 80, 0, 200 + i * 80, 600, al_map_rgb(50, 50, 50), 2);
    }
    al_draw_line(598, 0, 598, 600, al_map_rgb(50, 50, 50), 2);
    al_draw_line(190, 550, 610, 550, al_map_rgb(255, 255, 0), 3);
    const char* keys[] = {"A", "S", "J", "K", "L"};
    for (int i = 0; i < 5; ++i) {
        al_draw_filled_circle(240 + i * 80, 525, 30, al_map_rgba(255, 255, 255, 50));
        al_draw_text(font, al_map_rgb(0,0,0), 240 + i * 80, 510, ALLEGRO_ALIGN_CENTER, keys[i]);
    }
    
    noteManager.render();
    al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Score: %d", score);
    // Adiciona o tempo na tela
    al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, ALLEGRO_ALIGN_CENTER, "Tempo: %.0f", song_position - noteManager.getStartTime());
}

void Game::endPlaying() {
    if (music_stream) {
        al_detach_audio_stream(music_stream); // Para de tocar
        al_destroy_audio_stream(music_stream);
        music_stream = nullptr;
    }
    final_score = score; // Salva a pontuação final
    FileHandler::saveScore("scores.txt", final_score);
    currentState = GameState::SCORE_SCREEN;
    score_screen_option = 0; // Reseta a opção do menu de score
}

void Game::updateScoreScreen(const ALLEGRO_EVENT& event) {
     if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch(event.keyboard.keycode) {
            case ALLEGRO_KEY_DOWN:
                score_screen_option = (score_screen_option + 1) % 3;
                break;
            case ALLEGRO_KEY_UP:
                score_screen_option = (score_screen_option == 0) ? 2 : score_screen_option - 1;
                break;
            case ALLEGRO_KEY_ENTER:
                if (score_screen_option == 0) { // Jogar Novamente
                    startPlaying();
                } else if (score_screen_option == 1) { // Selecionar Outra Música
                    currentState = GameState::SONG_SELECT;
                } else { // Sair para o Menu Principal
                    currentState = GameState::MENU;
                }
                break;
        }
     }
}

void Game::renderScoreScreen() {
    ALLEGRO_COLOR color1 = (score_screen_option == 0) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR color2 = (score_screen_option == 1) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR color3 = (score_screen_option == 2) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);

    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 100, ALLEGRO_ALIGN_CENTER, "MUSICA FINALIZADA");
    al_draw_textf(font, al_map_rgb(255, 255, 0), 400, 150, ALLEGRO_ALIGN_CENTER, "PONTUAÇÃO FINAL: %d", final_score);

    al_draw_text(font, color1, 400, 300, ALLEGRO_ALIGN_CENTER, "JOGAR NOVAMENTE");
    al_draw_text(font, color2, 400, 350, ALLEGRO_ALIGN_CENTER, "SELECIONAR OUTRA MÚSICA");
    al_draw_text(font, color3, 400, 500, ALLEGRO_ALIGN_CENTER, "VOLTAR AO MENU");
}   