#include "game.h"
#include "file_handler.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Construtor
Game::Game() : 
    running(false), currentState(GameState::MENU), display(nullptr), 
    event_queue(nullptr), timer(nullptr), font(nullptr), 
    hit_sound(nullptr), miss_sound(nullptr), music_stream(nullptr),
    score(0), final_score(0), song_position(0.0f), 
    selectedSongIndex(0), menu_option(0), score_screen_option(0), music_started(false) {}

// Destrutor
Game::~Game() {
    if (music_stream) al_destroy_audio_stream(music_stream);
    if (hit_sound) al_destroy_sample(hit_sound);
    if (miss_sound) al_destroy_sample(miss_sound);
    if (font) al_destroy_font(font);
    if (timer) al_destroy_timer(timer);
    if (event_queue) al_destroy_event_queue(event_queue);
    if (display) al_destroy_display(display);
}

// Initialize
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

// Update (Chamado a cada frame)
void Game::update(float delta_time) {
    if (currentState == GameState::PLAYING) {
        updatePlaying({}, delta_time);
    }
}

// Render (Chama a renderização do estado atual)
void Game::render() {
    al_clear_to_color(al_map_rgb(20, 20, 40)); 
    switch (currentState) {
        case GameState::MENU:          renderMenu(); break;
        case GameState::SONG_SELECT:   renderSongSelect(); break;
        case GameState::PLAYING:       renderPlaying(); break;
        case GameState::SCORE_SCREEN:  renderScoreScreen(); break;
    }
}

// --- LÓGICA DO MENU ---
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
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 100, ALLEGRO_ALIGN_CENTER, "GUITAR HERO CLONE");
    ALLEGRO_COLOR play_color = (menu_option == 0) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR exit_color = (menu_option == 1) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
    al_draw_text(font, play_color, 400, 250, ALLEGRO_ALIGN_CENTER, "Selecionar Musica");
    al_draw_text(font, exit_color, 400, 300, ALLEGRO_ALIGN_CENTER, "Sair");
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

// CORREÇÃO 1: Limpeza dos nomes das músicas
void Game::renderSongSelect() {
    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 50, ALLEGRO_ALIGN_CENTER, "Selecione uma Musica");

    if (songList.empty()) {
        al_draw_text(font, al_map_rgb(255, 0, 0), 400, 250, ALLEGRO_ALIGN_CENTER, "Nenhuma musica encontrada!");
        al_draw_text(font, al_map_rgb(200,200,200), 400, 550, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para voltar");
        return;
    }
    
    for (int i = 0; i < songList.size(); ++i) {
        ALLEGRO_COLOR color = (i == selectedSongIndex) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        
        // Lógica para extrair o nome do arquivo
        std::string fullPath = songList[i];
        size_t last_slash = fullPath.find_last_of("/\\");
        std::string filename = (last_slash == std::string::npos) ? fullPath : fullPath.substr(last_slash + 1);
        size_t last_dot = filename.find_last_of(".");
        std::string songName = (last_dot == std::string::npos) ? filename : filename.substr(0, last_dot);

        al_draw_text(font, color, 400, 200 + i * 40, ALLEGRO_ALIGN_CENTER, songName.c_str());
    }
    
    al_draw_text(font, al_map_rgb(200,200,200), 400, 550, ALLEGRO_ALIGN_CENTER, "Pressione ENTER para jogar ou ESC para voltar");
}

// --- LÓGICA DO JOGO ---
void Game::startPlaying() {
    score = 0;
    song_position = 0;
    music_started = false;
    noteManager.reset();
    noteManager.loadSong(selectedSongPath);
    
    std::string audioPath = selectedSongPath;
    size_t dotPos = audioPath.rfind('.');
    if (dotPos != std::string::npos) audioPath.replace(dotPos, std::string::npos, ".ogg");

    music_stream = al_load_audio_stream(audioPath.c_str(), 4, 2048);
    if (music_stream) {
        al_attach_audio_stream_to_mixer(music_stream, al_get_default_mixer());
        al_set_audio_stream_playing(music_stream, true);
        music_started = true;
    } 

    currentState = GameState::PLAYING;
}

// Lógica de fim de jogo
void Game::updatePlaying(const ALLEGRO_EVENT& event, float delta_time) {
    // --- Lógica de Fim de Jogo ---
    bool song_has_ended = false;
    if (music_started && music_stream && !al_get_audio_stream_playing(music_stream)) {
        // Se a música tinha começado e agora parou, ela terminou.
        song_has_ended = true;
    } else if (!music_started && noteManager.isSongFinished()) {
        // Se nunca teve música e todas as notas acabaram, o jogo termina.
        song_has_ended = true;
    }

    if(song_has_ended) {
        endPlaying();
        return;
    }

    // --- Lógica de Avanço de Tempo ---
    if (delta_time > 0) {
        // Se a música está tocando, use o tempo dela para a sincronia perfeita.
        if (music_started && music_stream) {
            song_position = al_get_audio_stream_position_secs(music_stream);
        } else {
            // Se não, avance o tempo manualmente (RESERVA DE SEGURANÇA)
            song_position += delta_time;
        }
        
        // Atualiza o gerenciador de notas com o tempo correto
        noteManager.update(song_position, delta_time);
    }
    
    // --- Lógica de Input ---
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

// CORREÇÃO 2: Renderização das pistas visuais
void Game::renderPlaying() {
    // Desenha a "estrada" do jogo
    al_draw_filled_rectangle(190, 0, 610, 600, al_map_rgb(25, 25, 25));

    // Desenha as 5 colunas
    for (int i = 0; i < 5; ++i) {
        al_draw_line(200 + i * 80, 0, 200 + i * 80, 600, al_map_rgb(50, 50, 50), 2);
    }
    al_draw_line(598, 0, 598, 600, al_map_rgb(50, 50, 50), 2);


    // Desenha a zona de acerto
    al_draw_line(190, 550, 610, 550, al_map_rgb(255, 255, 0), 3);

    // Desenha os alvos fixos na zona de acerto
    const char* keys[] = {"A", "S", "D", "F", "G"};
    for (int i = 0; i < 5; ++i) {
        al_draw_filled_circle(240 + i * 80, 525, 30, al_map_rgba(255, 255, 255, 50));
        al_draw_text(font, al_map_rgb(0,0,0), 240 + i * 80, 510, ALLEGRO_ALIGN_CENTER, keys[i]);
    }

    noteManager.render();
    al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Score: %d", score);
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

// --- LÓGICA DA TELA DE PONTUAÇÃO ---
// CORREÇÃO 5: Nova tela de score com 3 opções
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

    al_draw_text(font, al_map_rgb(255, 255, 255), 400, 100, ALLEGRO_ALIGN_CENTER, "Musica Finalizada!");
    al_draw_textf(font, al_map_rgb(255, 255, 0), 400, 150, ALLEGRO_ALIGN_CENTER, "Pontuacao Final: %d", final_score);

    al_draw_text(font, color1, 400, 300, ALLEGRO_ALIGN_CENTER, "Jogar Novamente");
    al_draw_text(font, color2, 400, 350, ALLEGRO_ALIGN_CENTER, "Selecionar Outra Musica");
    al_draw_text(font, color3, 400, 400, ALLEGRO_ALIGN_CENTER, "Voltar ao Menu Principal");
}