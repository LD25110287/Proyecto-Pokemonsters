#ifndef POKEMONSTER_H
#define POKEMONSTER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Move.h"

class Pokemonster : public sf::Drawable
{
public:
    Pokemonster();
    Pokemonster(const std::string& name, int hpMax, int attack, int defense, const std::vector<Move>& moves);

    bool loadSpriteSheet(const std::string& path, int fWidth, int fHeight);
    void setFrame(int row, int col);
    void playAnimation(int row, int framesAmount);
    void updateAnimation();
    void attack(Pokemonster& target, int selectedMoveIndex);
    void setPosition(float x, float y);

    void takeDamage(int damage);
    bool isFainted() const;
    bool isAnimating() const { return isAttacking; }

    const std::vector<Move>& getMoves() const;
    const std::string& getName() const;
    int getHP() const;
    int getHPMax() const;
    int getAttack() const;
    int getDefense() const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::string name;
    int hpMax;
    int hpActual;
    int attackStat;
    int defense;
    std::vector<Move> moves; // tamaño esperado: 4

    sf::Texture texture;
    sf::Sprite sprite;
    int frameWidth;
    int frameHeight;

    int currentFrame;
    int currentRow;
    int maxFrames;
    float frameDuration;
    bool isAttacking;
    sf::Clock animClock;
};

#endif // POKEMONSTER_H
