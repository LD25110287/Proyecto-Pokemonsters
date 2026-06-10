#ifndef MOVE_H
#define MOVE_H

#include <string>

enum class MoveType { Normal, Fire, Water, Grass, Electric };

struct Move {
    std::string name;
    int power;
    MoveType type;
    std::string soundPath; // ruta relativa en assets/sounds/
    int animationRow;      // fila en el sprite sheet para esta animación
    int frameCount;        // cantidad de cuadros en esta animación

    Move(const std::string& n = "", int p = 0, MoveType t = MoveType::Normal, const std::string& s = "",
         int row = 0, int frames = 1)
        : name(n), power(p), type(t), soundPath(s), animationRow(row), frameCount(frames) {}
};

#endif // MOVE_H
