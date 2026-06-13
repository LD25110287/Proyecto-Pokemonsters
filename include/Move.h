#ifndef MOVE_H
#define MOVE_H

#include <string>

enum class MoveType { Normal, Fire, Water, Grass, Electric };

struct Move {
    std::string name;
    int         power;
    MoveType    type;
    std::string soundPath;
    int         animationRow;
    int         frameCount;
    int         energyCost;   // ← NUEVO: barritas de energía que cuesta

    Move(const std::string& n = "", int p = 0, MoveType t = MoveType::Normal,
         const std::string& s = "", int row = 0, int frames = 1, int cost = 1)
        : name(n), power(p), type(t), soundPath(s),
          animationRow(row), frameCount(frames), energyCost(cost) {}
};

#endif // MOVE_H
