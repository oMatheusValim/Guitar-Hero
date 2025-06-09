#include "note_manager.h"
#include <fstream>
#include <iostream>
#include <allegro5/allegro_primitives.h>

// --- Constantes para a Velocidade ---
const float INITIAL_NOTE_SPEED = 300.0f; // Velocidade inicial em pixels/segundo
const float MAX_NOTE_SPEED = 700.0f;     // Velocidade máxima
const float SPEED_INCREASE_RATE = 5.0f;  // Quantos pixels/segundo a velocidade aumenta por segundo

// Mapeamento de teclas para trilhas (0 a 4)
int map_key_to_track(int keycode) {
    switch (keycode) {
        case ALLEGRO_KEY_A: return 0;
        case ALLEGRO_KEY_S: return 1;
        case ALLEGRO_KEY_D: return 2;
        case ALLEGRO_KEY_F: return 3;
        case ALLEGRO_KEY_G: return 4;
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

void NoteManager::loadSong(const std::string& filename) {
    reset();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo da música: " << filename << std::endl;
        return;
    }

    float time;
    int key_code; 
    while (file >> time >> key_code) {
        Note note;
        note.time = time;
        note.track = map_key_to_track(key_code);
        if (note.track == -1) continue; 

        note.y_position = 0; // Começa um pouco acima da tela
        note.active = false;
        note.hit = false;
        note.missed = false;
        notes.push_back(note);
    }
    std::cout << "Música carregada com " << notes.size() << " notas." << std::endl;
}

void NoteManager::update(float song_position, float delta_time) {
    // Aumenta a velocidade gradualmente
    if (note_speed < MAX_NOTE_SPEED) {
        note_speed += SPEED_INCREASE_RATE * delta_time;
    }

    const float HIT_ZONE_Y = 525.0f; // Posição Y da zona de acerto
    // O tempo que a nota fica na tela agora depende da velocidade atual
    const float seconds_on_screen = HIT_ZONE_Y / note_speed;

    for (auto& note : notes) {
        // Ativa a nota quando for a hora certa
        if (!note.active && !note.hit && !note.missed && song_position >= note.time - seconds_on_screen) {
            note.active = true;
            note.y_position = 0; // Garante que ela comece do topo ao ser ativada
        }

        // Move a nota se ela estiver ativa
        if (note.active && !note.hit) {
            note.y_position += note_speed * delta_time;
        }

        // Verifica se a nota foi perdida
        if (note.active && !note.hit && !note.missed && note.y_position > HIT_ZONE_Y + 30) { // Uma pequena margem
            note.missed = true;
            note.active = false;
        }
    }
}


// Implementação da função de contagem
int NoteManager::getActiveNotesCount() const {
    int count = 0;
    for (const auto& note : notes) {
        if (note.active) {
            count++;
        }
    }
    return count;
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
    // RETORNA 0 AO ERRAR (SEM PENALIDADE)
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
        /*if (note.active && !note.hit) {
            float x1 = TRACK_START_X + note.track * TRACK_WIDTH + 5; // Adiciona margem
            float y1 = note.y_position - 10;
            float x2 = x1 + TRACK_WIDTH - 10; // Adiciona margem
            float y2 = note.y_position + 10;
            al_draw_filled_circle(x1 + (TRACK_WIDTH - 10)/2, y1 + 10, 25, keyToColor(note.track));
        }*/
       if (note.active && !note.hit) {
            float center_x = TRACK_START_X + (note.track * TRACK_WIDTH) + (TRACK_WIDTH / 2);
            float center_y = note.y_position;
            
            // Desenha uma elipse vermelha
            al_draw_filled_ellipse(center_x, center_y, 35, 15, al_map_rgb(255, 0, 0));
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