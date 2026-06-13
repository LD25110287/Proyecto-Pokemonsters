#include "../include/Pokemonster.h"
#include <algorithm>
#include <iostream>

const int Pokemonster::MAX_ENERGY;

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attackStat(1), defense(0),
      frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr),
      energy(3)
{}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack,
                         int defense, const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attackStat(attack),
      defense(defense), moves(moves),
      frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr),
      energy(3)
{}

// ── Sprite ────────────────────────────────────────────────────────────────────
bool Pokemonster::loadSpriteSheet(const std::string& path, int fWidth, int fHeight)
{
    if (!texture.loadFromFile(path))
    { std::cerr << "Failed to load: " << path << "\n"; return false; }
    sprite.setTexture(texture);
    frameWidth  = fWidth;
    frameHeight = fHeight;
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    return true;
}

void Pokemonster::setFrame(int row, int col)
{
    sprite.setTextureRect(sf::IntRect(col * frameWidth, row * frameHeight,
                                     frameWidth, frameHeight));
}

// Aplica escala visual al sprite (no afecta lógica ni hitbox)
void Pokemonster::setScale(float x, float y)
{
    sprite.setScale(x, y);
}

void Pokemonster::playAnimation(int row, int framesAmount)
{
    currentRow   = row;
    maxFrames    = framesAmount;
    currentFrame = 0;
    isAttacking  = true;
    animClock.restart();
    setFrame(currentRow, currentFrame);
}

void Pokemonster::updateAnimation()
{
    if (!isAttacking || frameDuration <= 0.f || maxFrames <= 0) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration)
    {
        currentFrame++;
        if (currentFrame >= maxFrames)
        {
            // Aplicar daño diferido al final de la animación
            if (hasPendingDamage && damageTarget)
            {
                damageTarget->takeDamage(pendingDamage);
                hasPendingDamage = false;
                damageTarget     = nullptr;
                pendingDamage    = 0;
            }
            currentRow   = 0;
            isAttacking  = false;
            currentFrame = 0;
        }
        setFrame(currentRow, currentFrame);
        animClock.restart();
    }
}

// ── Energía ───────────────────────────────────────────────────────────────────
void Pokemonster::addEnergy(int amount)
{
    energy = std::min(energy + amount, MAX_ENERGY);
}

bool Pokemonster::canUseMove(int moveIndex) const
{
    if (moveIndex < 0 || moveIndex >= (int)moves.size()) return false;
    return energy >= moves[moveIndex].energyCost;
}

// ── Ataque ────────────────────────────────────────────────────────────────────
void Pokemonster::attack(Pokemonster& target, int selectedMoveIndex)
{
    if (selectedMoveIndex < 0 || selectedMoveIndex >= (int)moves.size()) return;
    if (!canUseMove(selectedMoveIndex)) return;

    const Move& m = moves[selectedMoveIndex];

    energy -= m.energyCost;
    if (energy < 0) energy = 0;

    playAnimation(m.animationRow, m.frameCount);

    int damage    = std::max(1, m.power + attackStat - target.getDefense());
    pendingDamage = damage;
    damageTarget  = &target;
    hasPendingDamage = true;
}

// ── Otros ─────────────────────────────────────────────────────────────────────
void Pokemonster::setPosition(float x, float y) { sprite.setPosition({x, y}); }

void Pokemonster::takeDamage(int damage)
{
    hpActual -= damage;
    if (hpActual < 0) hpActual = 0;
}

bool Pokemonster::isFainted() const { return hpActual <= 0; }

const std::vector<Move>& Pokemonster::getMoves()  const { return moves; }
const std::string&        Pokemonster::getName()   const { return name; }
int Pokemonster::getHP()      const { return hpActual; }
int Pokemonster::getHPMax()   const { return hpMax; }
int Pokemonster::getAttack()  const { return attackStat; }
int Pokemonster::getDefense() const { return defense; }

void Pokemonster::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (texture.getSize().x > 0)
        target.draw(sprite, states);
}
