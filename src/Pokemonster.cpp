#include "../include/Pokemonster.h"
#include <algorithm>
#include <iostream>

Pokemonster::Pokemonster()
    : name(""), hpMax(1), hpActual(1), attackStat(1), defense(0), moves(),
      frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr)
{
}

Pokemonster::Pokemonster(const std::string& name, int hpMax, int attack, int defense,
                         const std::vector<Move>& moves)
    : name(name), hpMax(hpMax), hpActual(hpMax), attackStat(attack), defense(defense),
      moves(moves), frameWidth(0), frameHeight(0), currentFrame(0), currentRow(0),
      maxFrames(0), frameDuration(0.1f), isAttacking(false),
      pendingDamage(0), hasPendingDamage(false), damageTarget(nullptr)
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
    frameWidth  = fWidth;
    frameHeight = fHeight;
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
    return true;
}

void Pokemonster::setFrame(int row, int col)
{
    int x = col * frameWidth;
    int y = row * frameHeight;
    sprite.setTextureRect(sf::IntRect(x, y, frameWidth, frameHeight));
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
    if (!isAttacking) return;
    if (frameDuration <= 0.f || maxFrames <= 0) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration)
    {
        currentFrame++;

        if (currentFrame >= maxFrames)
        {
            // ── BUG 1 FIX ──────────────────────────────────────────────────
            // El daño AHORA se aplica cuando la animación de ataque termina,
            // NO al inicio. Así el HP del enemigo no baja al mismo tiempo
            // que el del jugador.
            if (hasPendingDamage && damageTarget != nullptr)
            {
                damageTarget->takeDamage(pendingDamage);
                hasPendingDamage = false;
                damageTarget     = nullptr;
                pendingDamage    = 0;
            }
            // ───────────────────────────────────────────────────────────────

            currentRow  = 0;   // volver a fila idle
            isAttacking = false;
            currentFrame = 0;
        }

        setFrame(currentRow, currentFrame);
        animClock.restart();
    }
}

void Pokemonster::attack(Pokemonster& target, int selectedMoveIndex)
{
    if (selectedMoveIndex < 0 ||
        selectedMoveIndex >= static_cast<int>(moves.size()))
    {
        std::cerr << "Invalid move index: " << selectedMoveIndex << std::endl;
        return;
    }

    const Move& selectedMove = moves[selectedMoveIndex];

    // ── BUG 2 FIX ──────────────────────────────────────────────────────────
    // Cada movimiento tiene su propia fila (animationRow) y cantidad de
    // frames (frameCount) definidos en Move.h. Aquí se usa esa info para
    // reproducir la animación correcta según la habilidad seleccionada.
    playAnimation(selectedMove.animationRow, selectedMove.frameCount);
    // ───────────────────────────────────────────────────────────────────────

    // ── BUG 1 FIX: guardar daño como pendiente ─────────────────────────────
    // El daño se calcula ahora pero se guarda; se aplica al final de la
    // animación en updateAnimation(), no de forma inmediata.
    int damage       = std::max(1, selectedMove.power + attackStat - target.getDefense());
    pendingDamage    = damage;
    damageTarget     = &target;
    hasPendingDamage = true;
    // ───────────────────────────────────────────────────────────────────────
}

void Pokemonster::setPosition(float x, float y)
{
    sprite.setPosition({x, y});
}

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
