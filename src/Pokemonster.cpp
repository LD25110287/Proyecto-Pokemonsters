#include "../include/Pokemonster.h"
#include <iostream>

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attack(1), defense(0), moves(), frameWidth(0), frameHeight(0)
{
}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack, int defense, const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attack(attack), defense(defense), moves(moves), frameWidth(0), frameHeight(0)
{
}


bool Pokemonster::loadSpriteSheet(const std::string& path, int fWidth, int fHeight)
{
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Failed to load sprite sheet: " << path << std::endl;
        return false;
    }

    sprite.setTexture(texture);
    frameWidth = fWidth;
    frameHeight = fHeight;
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    return true;
}

void Pokemonster::setFrame(int row, int col)
{
    // Cada cuadro del sprite sheet está ordenado en una cuadrícula.
    // La posición X se calcula con la columna multiplicada por el ancho del cuadro,
    // y la posición Y con la fila multiplicada por la altura del cuadro.
    int x = col * frameWidth;
    int y = row * frameHeight;

    sf::IntRect frameRect(x, y, frameWidth, frameHeight);
    sprite.setTextureRect(frameRect);
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
