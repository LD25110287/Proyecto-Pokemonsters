#ifndef POKEMONSTER_H
#define POKEMONSTER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Move.h"

// ── Sistema de atributos ──────────────────────────────────────────────────────
// Vacuna > Virus > Data > Vacuna
enum class Attribute { Vacuna, Virus, Data };

// Retorna true si 'attacker' tiene ventaja sobre 'defender'
inline bool hasAdvantage(Attribute attacker, Attribute defender)
{
    return (attacker == Attribute::Vacuna && defender == Attribute::Virus)  ||
           (attacker == Attribute::Virus  && defender == Attribute::Data)   ||
           (attacker == Attribute::Data   && defender == Attribute::Vacuna);
}
// ─────────────────────────────────────────────────────────────────────────────

class Pokemonster : public sf::Drawable
{
public:
    Pokemonster();
    Pokemonster(const std::string& name, int hpMax, int attack, int defense,
                const std::vector<Move>& moves);

    // ── Constructor y operador de copia personalizados ──────────────────────
    // Sin esto, copiar/asignar un Pokemonster (p.ej. al cambiar de personaje
    // en combate con `player = buildCharacter(...)`) deja `sprite` apuntando
    // a la textura del objeto temporal de origen, que se destruye al terminar
    // la expresión → el personaje se dibuja como un cuadro blanco.
    // Estos re-vinculan `sprite` a la textura PROPIA de este objeto.
    Pokemonster(const Pokemonster& other);
    Pokemonster& operator=(const Pokemonster& other);
    // ──────────────────────────────────────────────────────────────────────

    bool loadSpriteSheet(const std::string& path, int fWidth, int fHeight);
    void setFrame(int row, int col);
    void playAnimation(int row, int framesAmount);
    void updateAnimation();
    void attack(Pokemonster& target, int selectedMoveIndex);
    void setPosition(float x, float y);
    void setScale(float x, float y);

    void takeDamage(int damage);
    bool isFainted()   const;
    bool isAnimating() const { return isAttacking; }

    // ── Atributo ──────────────────────────────────────────────────────────
    void      setAttribute(Attribute a) { attribute = a; }
    Attribute getAttribute()      const { return attribute; }
    // ──────────────────────────────────────────────────────────────────────

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
    Attribute         attribute;   // atributo de este personaje

    sf::Texture texture;
    sf::Sprite  sprite;
    int frameWidth, frameHeight;
    int currentFrame, currentRow, maxFrames;
    float frameDuration;
    bool  isAttacking;
    sf::Clock animClock;

    int          pendingDamage;
    bool         hasPendingDamage;
    Pokemonster* damageTarget;

    int energy;
};

#endif // POKEMONSTER_H
