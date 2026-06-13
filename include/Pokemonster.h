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
    Pokemonster(const std::string& name, int hpMax, int attack, int defense,
                const std::vector<Move>& moves);

    bool loadSpriteSheet(const std::string& path, int fWidth, int fHeight);
    void setFrame(int row, int col);
    void playAnimation(int row, int framesAmount);
    void updateAnimation();
    void attack(Pokemonster& target, int selectedMoveIndex);
    void setPosition(float x, float y);

    // Escala visual del sprite (para efecto estilo Pokémon clásico)
    void setScale(float x, float y);

    void takeDamage(int damage);
    bool isFainted()   const;
    bool isAnimating() const { return isAttacking; }

    // ── Energía ───────────────────────────────────────────────────────────
    static const int MAX_ENERGY = 10;
    int  getEnergy()    const { return energy; }
    int  getMaxEnergy() const { return MAX_ENERGY; }
    void addEnergy(int amount);
    bool canUseMove(int moveIndex) const;
    // ──────────────────────────────────────────────────────────────────────

    const std::vector<Move>& getMoves()  const;
    const std::string&        getName()  const;
    int getHP()      const;
    int getHPMax()   const;
    int getAttack()  const;
    int getDefense() const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::string       name;
    int               hpMax;
    int               hpActual;
    int               attackStat;
    int               defense;
    std::vector<Move> moves;

    sf::Texture texture;
    sf::Sprite  sprite;
    int frameWidth, frameHeight;
    int currentFrame, currentRow, maxFrames;
    float frameDuration;
    bool  isAttacking;
    sf::Clock animClock;

    // Daño diferido (Bug 1 fix)
    int          pendingDamage;
    bool         hasPendingDamage;
    Pokemonster* damageTarget;

    // Energía
    int energy;
};

#endif // POKEMONSTER_H
