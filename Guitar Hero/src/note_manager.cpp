#include "note_manager.h"
#include <fstream>
#include <iostream>
#include <string> // Necessário para std::string e std::getline
#include <sstream> // Necessário para std::stringstream
#include <allegro5/allegro_primitives.h>

// --- Constantes para a Velocidade ---
// Diminuímos a velocidade para dar mais tempo de reação.
const float INITIAL_NOTE_SPEED = 220.0f; // Era 300.0f
const float MAX_NOTE_SPEED = 500.0f;     // Era 700.0f
const float SPEED_INCREASE_RATE = 4.0f;  // Era 5.0f

// --- CORREÇÃO 1: Mapeamento de Teclas ---
// Agora esta função entende os códigos do arquivo .txt
// E também as teclas pressionadas pelo jogador (ALLEGRO_KEY_*)
int map_key_to_track(int keycode) {
    switch (keycode) {
        // Mapeamento para as teclas do jogador
        case ALLEGRO_KEY_A: return 0;
        case ALLEGRO_KEY_S: return 1;
        case ALLEGRO_KEY_J: return 2;
        case ALLEGRO_KEY_K: return 3;
        case ALLEGRO_KEY_L: return 4;

        // Mapeamento para os códigos do arquivo .txt
        case 97:  return 0; // 'a' 
        case 115: return 1; // 's'
        case 106: return 2; // 'd -> j'
        case 107: return 3; // 'f -> k'
        case 108: return 4; // 'g -> l'
            
        default: return -1;
    }
}

NoteManager::NoteManager() {
    reset();
}

void NoteManager::reset() {
    notes.clear();
    note_speed = INITIAL_NOTE_SPEED;
}

// --- CORREÇÃO 2: Leitura Inteligente do Arquivo ---
void NoteManager::loadSong(const std::string& filename) {
    reset();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo da música: " << filename << std::endl;
        return;
    }

    std::string line;
    // Lê o arquivo linha por linha
    while (std::getline(file, line)) {
        // Ignora linhas de comentário ou vazias
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        float time;
        int key_code;

        // Tenta extrair os dois números da linha
        if (ss >> time >> key_code) {
            Note note;
            note.time = time;
            note.track = map_key_to_track(key_code);
            
            // Só adiciona a nota se a trilha for válida
            if (note.track != -1) {
                note.y_position = 0;
                note.active = false;
                note.hit = false;
                note.missed = false;
                notes.push_back(note);
            }
        }
    }
    std::cout << "Musica carregada com " << notes.size() << " notas." << std::endl;
}

void NoteManager::update(float song_position, float delta_time) {
    if (note_speed < MAX_NOTE_SPEED) {
        note_speed += SPEED_INCREASE_RATE * delta_time;
    }

    const float HIT_ZONE_Y = 525.0f;
    const float seconds_on_screen = HIT_ZONE_Y / note_speed;

    for (auto& note : notes) {
        // Ativa a nota quando for a hora certa de aparecer na tela
        if (!note.active && !note.hit && !note.missed && song_position >= note.time - seconds_on_screen) {
            note.active = true;
            note.y_position = 0;
        }

        // Move a nota para baixo se ela estiver ativa
        if (note.active && !note.hit) {
            note.y_position += note_speed * delta_time;
        }

        // Verifica se o jogador perdeu a nota
        if (note.active && !note.hit && !note.missed && note.y_position > HIT_ZONE_Y + 30) {
            note.missed = true;
            note.active = false;
        }
    }
}

int NoteManager::checkHit(int key_code) {
    int track = map_key_to_track(key_code);
    if (track == -1) return 0;

    const float HIT_ZONE_Y_START = 480.0f;
    const float HIT_ZONE_Y_END = 550.0f;

    for (auto& note : notes) {
        if (note.active && !note.hit && !note.missed && note.track == track) {
            if (note.y_position >= HIT_ZONE_Y_START && note.y_position <= HIT_ZONE_Y_END) {
                note.hit = true;
                note.active = false;
                return 100;
            }
        }
    }
    return 0;
}

ALLEGRO_COLOR NoteManager::keyToColor(int track) {
    switch (track) {
        case 0: return al_map_rgb(0, 255, 0);   // Verde
        case 1: return al_map_rgb(255, 0, 0);   // Vermelho
        case 2: return al_map_rgb(255, 255, 0); // Amarelo
        case 3: return al_map_rgb(0, 0, 255);   // Azul
        case 4: return al_map_rgb(255, 165, 0); // Laranja
        default: return al_map_rgb(255, 255, 255);
    }
}

void NoteManager::render() {
    const float TRACK_START_X = 200.0f;
    const float TRACK_WIDTH = 80.0f;

    for (const auto& note : notes) {
       if (note.active && !note.hit) {
            float center_x = TRACK_START_X + (note.track * TRACK_WIDTH) + (TRACK_WIDTH / 2);
            float center_y = note.y_position;
            al_draw_filled_circle(center_x, center_y, 25, keyToColor(note.track));
        }
    }
}

bool NoteManager::isSongFinished() const {
    for (const auto& note : notes) {
        if (!note.hit && !note.missed) {
            return false;
        }
    }
    return !notes.empty(); 
}

// Implementação da função de contagem (não utilizada no momento, mas mantida)
int NoteManager::getActiveNotesCount() const {
    int count = 0;
    for (const auto& note : notes) {
        if (note.active) {
            count++;
        }
    }
    return count;
}
