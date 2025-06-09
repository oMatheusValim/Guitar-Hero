#ifndef NOTE_MANAGER_H
#define NOTE_MANAGER_H

#include <vector>
#include <string>
#include <allegro5/allegro5.h>

struct Note {
    float time;
    float y_position;
    int track;
    bool active;
    bool hit;
    bool missed;
};

class NoteManager {
public:
    NoteManager();

    void loadSong(const std::string& filename);
    void update(float song_position, float delta_time);
    void render();
    int checkHit(int key_code);
    void reset();

    bool isSongFinished() const;
    int getActiveNotesCount() const;

private:
    std::vector<Note> notes;
    float note_speed; // << NOVO: Velocidade agora é uma variável

    ALLEGRO_COLOR keyToColor(int track);
};

#endif // NOTE_MANAGER_H