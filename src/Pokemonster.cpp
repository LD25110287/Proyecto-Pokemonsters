#include "../include/Pokemonster.h"
#include <iostream>

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attack(1), defense(0), moves()
{
}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack, int defense, const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attack(attack), defense(defense), moves(moves)
{
}

bool Pokemonster::loadTexture(const std::string& path)
{
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }
    sprite.setTexture(texture);
    return true;
}

void Pokemonster::setPosition(float x, float y)
{
    sprite.setPosition(x, y);
}

void Pokemonster::takeDamage(int damage)
{
    hpActual -= damage;
    if (hpActual < 0) hpActual = 0;
}

bool Pokemonster::isFainted() const
{
    return hpActual <= 0;
}

const std::vector<Move>& Pokemonster::getMoves() const { return moves; }
const std::string& Pokemonster::getName() const { return name; }
int Pokemonster::getHP() const { return hpActual; }
int Pokemonster::getHPMax() const { return hpMax; }
int Pokemonster::getAttack() const { return attack; }
int Pokemonster::getDefense() const { return defense; }

void Pokemonster::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (texture.getSize().x > 0)
        target.draw(sprite, states);
}
