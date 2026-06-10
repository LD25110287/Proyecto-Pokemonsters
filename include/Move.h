#ifndef MOVE_H
#define MOVE_H

#include <string>

enum class MoveType { Normal, Fire, Water, Grass, Electric };

struct Move {
    std::string name;
    int power;
    MoveType type;
    std::string soundPath; // ruta relativa en assets/sounds/

    Move(const std::string& n = "", int p = 0, MoveType t = MoveType::Normal, const std::string& s = "")
        : name(n), power(p), type(t), soundPath(s) {}
};

#endif // MOVE_H
