#include "../include/BattleUI.h"
#include "../include/Pokemonster.h"
#include <iostream>

const sf::Color BattleUI::BTN_ACTIVE   = sf::Color(70,  70,  120);
const sf::Color BattleUI::BTN_DISABLED = sf::Color(45,  45,  45);

// ── Constructor ───────────────────────────────────────────────────────────────
BattleUI::BattleUI(const sf::Vector2u& windowSize)
    : winSize(windowSize), player(nullptr), enemy(nullptr)
{
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::vector<std::string> fallbacks = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };
        for (auto& p : fallbacks)
            if (font.loadFromFile(p)) break;
    }

    // ── Barra HP jugador (abajo-izquierda) ───────────────────────────────
    playerHpBack.setSize({220.f, 16.f});
    playerHpBack.setFillColor(sf::Color(80, 80, 80));
    playerHpBack.setPosition(20.f, winSize.y - 130.f);

    playerHpFront.setSize({220.f, 16.f});
    playerHpFront.setFillColor(sf::Color::Green);
    playerHpFront.setPosition(playerHpBack.getPosition());

    // ── Barra HP enemigo (arriba-derecha) ────────────────────────────────
    enemyHpBack.setSize({220.f, 16.f});
    enemyHpBack.setFillColor(sf::Color(80, 80, 80));
    enemyHpBack.setPosition(winSize.x - 240.f, 20.f);

    enemyHpFront.setSize({220.f, 16.f});
    enemyHpFront.setFillColor(sf::Color::Green);
    enemyHpFront.setPosition(enemyHpBack.getPosition());

    // ── Barras de energía (10 celdas) ────────────────────────────────────
    buildEnergyBar(playerEnergyCells, 20.f,              winSize.y - 108.f,
                   sf::Color(0, 200, 255));
    buildEnergyBar(enemyEnergyCells,  winSize.x - 240.f, 42.f,
                   sf::Color(255, 180, 0));

    // ── Botones de movimiento (4) abajo-derecha ───────────────────────────
    const float btnW = 220.f, btnH = 40.f;
    moveButtons.resize(4);
    moveTexts.resize(4);
    moveCostTexts.resize(4);

    for (int i = 0; i < 4; ++i)
    {
        float x = winSize.x - btnW - 20.f;
        float y = winSize.y - (4 - i) * (btnH + 8.f) - 20.f;

        moveButtons[i].setSize({btnW, btnH});
        moveButtons[i].setFillColor(BTN_ACTIVE);
        moveButtons[i].setOutlineColor(sf::Color(120, 120, 180));
        moveButtons[i].setOutlineThickness(1.f);
        moveButtons[i].setPosition(x, y);

        moveTexts[i].setFont(font);
        moveTexts[i].setCharacterSize(14);
        moveTexts[i].setFillColor(sf::Color::White);
        moveTexts[i].setPosition(x + 6.f, y + 5.f);

        // Costo (esquina superior-derecha del botón)
        moveCostTexts[i].setFont(font);
        moveCostTexts[i].setCharacterSize(12);
        moveCostTexts[i].setFillColor(sf::Color(0, 220, 255));
        moveCostTexts[i].setPosition(x + btnW - 28.f, y + 4.f);
    }
}

// ── Construir barra de energía ────────────────────────────────────────────────
void BattleUI::buildEnergyBar(std::vector<sf::RectangleShape>& cells,
                               float x, float y, sf::Color /*activeColor*/)
{
    cells.resize(10);
    const float cellW = 18.f, cellH = 10.f, gap = 3.f;
    for (int i = 0; i < 10; ++i)
    {
        cells[i].setSize({cellW, cellH});
        cells[i].setPosition(x + i * (cellW + gap), y);
        cells[i].setFillColor(sf::Color(60, 60, 60));
        cells[i].setOutlineColor(sf::Color(100, 100, 100));
        cells[i].setOutlineThickness(1.f);
    }
}

// ── Setters ───────────────────────────────────────────────────────────────────
void BattleUI::setPlayerPokemon(Pokemonster* p) { player = p; }
void BattleUI::setEnemyPokemon(Pokemonster* e)  { enemy  = e; }

// ── Update ────────────────────────────────────────────────────────────────────
void BattleUI::update()
{
    // HP jugador
    if (player)
    {
        float ratio = (player->getHPMax() > 0)
            ? static_cast<float>(player->getHP()) / player->getHPMax() : 0.f;
        playerHpFront.setSize({220.f * ratio, 16.f});
        if      (ratio > 0.6f) playerHpFront.setFillColor(sf::Color::Green);
        else if (ratio > 0.3f) playerHpFront.setFillColor(sf::Color::Yellow);
        else                    playerHpFront.setFillColor(sf::Color::Red);

        // Energía jugador
        int e = player->getEnergy();
        sf::Color activeColor(0, 200, 255);
        for (int i = 0; i < 10; ++i)
            playerEnergyCells[i].setFillColor(i < e
                ? activeColor : sf::Color(40, 40, 40));

        // Botones: activo/desactivado según energía disponible
        const auto& moves = player->getMoves();
        for (size_t i = 0; i < moveButtons.size() && i < moves.size(); ++i)
        {
            bool canUse = player->canUseMove(static_cast<int>(i));
            moveButtons[i].setFillColor(canUse ? BTN_ACTIVE : BTN_DISABLED);
            moveButtons[i].setOutlineColor(canUse
                ? sf::Color(120, 120, 200) : sf::Color(60, 60, 60));
            moveTexts[i].setFillColor(canUse
                ? sf::Color::White : sf::Color(100, 100, 100));

            // Mostrar nombre y costo
            moveTexts[i].setString(moves[i].name);
            moveCostTexts[i].setString(std::to_string(moves[i].energyCost) + "E");
            moveCostTexts[i].setFillColor(canUse
                ? sf::Color(0, 220, 255) : sf::Color(80, 80, 80));
        }
    }

    // HP enemigo
    if (enemy)
    {
        float ratio = (enemy->getHPMax() > 0)
            ? static_cast<float>(enemy->getHP()) / enemy->getHPMax() : 0.f;
        enemyHpFront.setSize({220.f * ratio, 16.f});
        if      (ratio > 0.6f) enemyHpFront.setFillColor(sf::Color::Green);
        else if (ratio > 0.3f) enemyHpFront.setFillColor(sf::Color::Yellow);
        else                    enemyHpFront.setFillColor(sf::Color::Red);

        // Energía enemigo
        int e = enemy->getEnergy();
        sf::Color activeColor(255, 180, 0);
        for (int i = 0; i < 10; ++i)
            enemyEnergyCells[i].setFillColor(i < e
                ? activeColor : sf::Color(40, 40, 40));
    }
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void BattleUI::draw(sf::RenderTarget& target)
{
    // HP barras
    target.draw(playerHpBack);
    target.draw(playerHpFront);
    target.draw(enemyHpBack);
    target.draw(enemyHpFront);

    // Energía
    for (auto& c : playerEnergyCells) target.draw(c);
    for (auto& c : enemyEnergyCells)  target.draw(c);

    // Botones de movimiento
    for (size_t i = 0; i < moveButtons.size(); ++i)
    {
        target.draw(moveButtons[i]);
        if (font.getInfo().family.size() > 0)
        {
            target.draw(moveTexts[i]);
            target.draw(moveCostTexts[i]);
        }
    }
}

// ── Click ─────────────────────────────────────────────────────────────────────
int BattleUI::handleMouseClick(sf::Vector2i mousePos)
{
    sf::Vector2f m(static_cast<float>(mousePos.x),
                   static_cast<float>(mousePos.y));
    for (size_t i = 0; i < moveButtons.size(); ++i)
    {
        if (moveButtons[i].getGlobalBounds().contains(m))
        {
            // Si el botón está desactivado (no hay energía), ignorar click
            if (player && !player->canUseMove(static_cast<int>(i)))
                return -1;
            return static_cast<int>(i);
        }
    }
    return -1;
}

void BattleUI::updateMoveButtons(Pokemonster* pokemon)
{
    if (!pokemon) return;
    const auto& moves = pokemon->getMoves();
    for (size_t i = 0; i < moveTexts.size() && i < moves.size(); ++i)
        moveTexts[i].setString(moves[i].name);
}
