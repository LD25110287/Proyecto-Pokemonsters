#include "../include/Pokemonster.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>   // rand()

const int Pokemonster::MAX_ENERGY;

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attackStat(1), defense(0),
      frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr),
      energy(3), attribute(Attribute::Vacuna)
{}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack,
                         int defense, const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attackStat(attack),
      defense(defense), moves(moves),
      frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr),
      energy(3), attribute(Attribute::Vacuna)
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

void Pokemonster::setScale(float x, float y) { sprite.setScale(x, y); }

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
// FÓRMULA DE DAÑO con varianza aleatoria para habilidades 2, 3 y 4:
//
//   Habilidad 1 (0E) → power=6  → daño FIJO:    6% del HP máximo enemigo
//   Habilidad 2 (2E) → power=15 → daño ALEATORIO: 9%  a 15% del HP máximo
//   Habilidad 3 (3E) → power=25 → daño ALEATORIO: 18% a 25% del HP máximo
//   Habilidad 4 (5E) → power=40 → daño ALEATORIO: 30% a 40% del HP máximo
//
// El mínimo de cada habilidad siempre supera el máximo de la anterior,
// así que las habilidades caras SIEMPRE valen la pena, solo hay incertidumbre
// en QUÉ TAN BIEN salen.
//
// Además se aplica un modificador de stats (±20%) basado en atk vs def.
void Pokemonster::attack(Pokemonster& target, int selectedMoveIndex)
{
    if (selectedMoveIndex < 0 || selectedMoveIndex >= (int)moves.size()) return;
    if (!canUseMove(selectedMoveIndex)) return;

    const Move& m = moves[selectedMoveIndex];

    // Descontar energía
    energy -= m.energyCost;
    if (energy < 0) energy = 0;

    // Activar animación
    playAnimation(m.animationRow, m.frameCount);

    // ── Rango de porcentaje según el índice de la habilidad ───────────────────
    // power es el techo (máximo). El piso es ~60% del techo.
    // Habilidad 1 (power=6)  → siempre 6%  (sin varianza, es el ataque básico)
    // Habilidad 2 (power=15) → entre 9%  y 15%
    // Habilidad 3 (power=25) → entre 18% y 25%
    // Habilidad 4 (power=40) → entre 30% y 40%
    float powerMin, powerMax;

    if (selectedMoveIndex == 0)
    {
        // Habilidad 1: daño fijo, sin sorpresas
        powerMin = static_cast<float>(m.power);
        powerMax = static_cast<float>(m.power);
    }
    else
    {
        // Habilidades 2-4: mínimo = 60% del techo, máximo = techo
        powerMax = static_cast<float>(m.power);
        powerMin = powerMax * 0.60f;
    }

    // Porcentaje aleatorio entre min y max
    float range      = powerMax - powerMin;
    float randomPct  = powerMin + (range > 0.f
                       ? (static_cast<float>(std::rand() % 1001) / 1000.f) * range
                       : 0.f);

    // Daño base = porcentaje del HP máximo del objetivo
    float baseDamage = (randomPct / 100.f) * static_cast<float>(target.getHPMax());

    // Modificador de stats: ±20% máximo según diferencia atk - def
    float statDiff = static_cast<float>(attackStat - target.getDefense());
    float statMod  = 1.0f + std::max(-0.20f, std::min(0.20f, statDiff / 50.f));

    // ── Multiplicador de atributo ─────────────────────────────────────────────
    // Vacuna > Virus > Data > Vacuna
    // Si este personaje tiene ventaja: multiplicador aleatorio x1.2 a x1.5
    float attrMod = 1.0f;
    if (hasAdvantage(attribute, target.getAttribute()))
    {
        // Aleatorio entre 1.2 y 1.5
        float t = static_cast<float>(std::rand() % 1001) / 1000.f; // 0.0 a 1.0
        attrMod = 1.2f + t * 0.3f;  // 1.2 a 1.5
    }
    // ─────────────────────────────────────────────────────────────────────────

    int damage = std::max(1, static_cast<int>(baseDamage * statMod * attrMod));

    pendingDamage    = damage;
    damageTarget     = &target;
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
