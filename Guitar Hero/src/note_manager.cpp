#include "note_manager.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <allegro5/allegro_primitives.h>

// --- Constantes de Velocidade
const float INITIAL_NOTE_SPEED = 220.0f;
const float MAX_NOTE_SPEED = 500.0f;
const float SPEED_INCREASE_RATE = 4.0f;

// --- Função de Mapeamento de Teclas 
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
        case 106: return 2; // 'j'
        case 107: return 3; // 'k'
        case 108: return 4; // 'l'
            
        default: return -1;
    }
}

NoteManager::NoteManager() {
    reset();
}

void NoteManager::reset() {
    notes.clear();
    note_speed = INITIAL_NOTE_SPEED;
    startTime = 0.0f;
}

// --- Função de Carregamento
void NoteManager::loadSong(const std::string& filename) {
    reset();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo da música: " << filename << std::endl;
        return;
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (firstLine) {
            std::string head;
            std::stringstream iss_head(line);
            iss_head >> head;
            if (head == "START_TIME:") {
                iss_head >> startTime;
                continue; 
            }
        }
        firstLine = false;

        std::stringstream ss(line);
        float time;
        int key_code;
        if (ss >> time >> key_code) {
            Note note;
            note.time = time;
            note.track = map_key_to_track(key_code);
            
            if (note.track != -1) {
                note.y_position = 0;
                note.active = false;
                note.hit = false;
                note.missed = false;
                notes.push_back(note);
            }
        }
    }
    std::cout << "Musica carregada com " << notes.size() << " notas. Start time: " << startTime << "s." << std::endl;
}

float NoteManager::getStartTime() const {
    return startTime;
}

void NoteManager::update(float song_position, float delta_time) {
    if (note_speed < MAX_NOTE_SPEED) {
        note_speed += SPEED_INCREASE_RATE * delta_time;
    }

    const float HIT_ZONE_Y = 525.0f;
    const float seconds_on_screen = HIT_ZONE_Y / note_speed;

    for (auto& note : notes) {
        if (!note.active && !note.hit && !note.missed && song_position >= note.time - seconds_on_screen) {
            note.active = true;
            note.y_position = 0;
        }

        if (note.active && !note.hit) {
            note.y_position += note_speed * delta_time;
        }

        if (note.active && !note.hit && !note.missed && note.y_position > HIT_ZONE_Y + 30) {
            note.missed = true;
            note.active = false;
        }
    }
}

// --- Função de Checagem de Acerto
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
                return 10;
            }
        }
    }
    return 0;
}

// --- Função de Cor da Nota 
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

// --- Função de Renderização
void NoteManager::render() {
    const float TRACK_START_X = 200.0f;
    const float TRACK_WIDTH = 80.0f;

    for (const auto& note : notes) {
       if (note.active && !note.hit) {
            float center_x = TRACK_START_X + (note.track * TRACK_WIDTH) + (TRACK_WIDTH / 2);
            float center_y = note.y_position;
            
            // Efeito "3D": desenha uma "base" escura e deslocada
            al_draw_filled_ellipse(center_x, center_y + 5, 35, 15, al_map_rgb(120, 0, 0));
            // Desenha o "topo" do disco em vermelho vivo
            al_draw_filled_ellipse(center_x, center_y, 35, 15, keyToColor(note.track));
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

int NoteManager::getActiveNotesCount() const {
    int count = 0;
    for (const auto& note : notes) {
        if (note.active) {
            count++;
        }
    }
    return count;
}