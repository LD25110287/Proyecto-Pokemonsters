#include "../include/Pokemonster.h"
#include <algorithm>
#include <iostream>

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attackStat(1), defense(0), moves(), frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0), maxFrames(0), frameDuration(0.1f), isAttacking(false)
{
}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack, int defense, const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attackStat(attack), defense(defense), moves(moves), frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0), maxFrames(0), frameDuration(0.1f), isAttacking(false)
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

void Pokemonster::playAnimation(int row, int framesAmount)
{
    currentRow = row;
    maxFrames = framesAmount;
    currentFrame = 0;
    isAttacking = true;
    animClock.restart();
    setFrame(currentRow, currentFrame);
}

void Pokemonster::updateAnimation()
{
    if (!isAttacking) return;
    if (frameDuration <= 0.f || maxFrames <= 0)
        return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration)
    {
        currentFrame++;

        if (currentFrame >= maxFrames)
        {
            if (isAttacking)
            {
                currentRow = 0; // Volver a Idle cuando la animación de ataque termina.
                isAttacking = false;
            }
            currentFrame = 0;
        }

        setFrame(currentRow, currentFrame);
        animClock.restart();
    }
}

void Pokemonster::attack(Pokemonster& target, int selectedMoveIndex)
{
    if (selectedMoveIndex < 0 || selectedMoveIndex >= static_cast<int>(moves.size()))
    {
        std::cerr << "Invalid move index: " << selectedMoveIndex << std::endl;
        return;
    }

    const Move& selectedMove = moves[selectedMoveIndex];

    // Reproducir animación específica del ataque basado en la fila y cantidad de cuadros del Move.
    playAnimation(selectedMove.animationRow, selectedMove.frameCount);

    int damage = std::max(1, selectedMove.power + attackStat - target.getDefense());
    target.takeDamage(damage);
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
int Pokemonster::getAttack() const { return attackStat; }
int Pokemonster::getDefense() const { return defense; }

void Pokemonster::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (texture.getSize().x > 0)
        target.draw(sprite, states);
}
