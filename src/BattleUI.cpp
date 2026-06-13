#include "../include/BattleUI.h"
#include "../include/Pokemonster.h"
#include <iostream>

// ── Colores ───────────────────────────────────────────────────────────────────
const sf::Color BattleUI::P1_BTN_ACTIVE   = sf::Color(50,  80,  160);  // azul J1 activo
const sf::Color BattleUI::P1_BTN_DISABLED = sf::Color(30,  45,  80);   // azul J1 sin energía
const sf::Color BattleUI::P2_BTN_ACTIVE   = sf::Color(140, 40,  40);   // rojo J2 activo
const sf::Color BattleUI::P2_BTN_DISABLED = sf::Color(70,  25,  25);   // rojo J2 sin energía
const sf::Color BattleUI::BTN_NO_ENERGY   = sf::Color(45,  45,  45);   // gris genérico

// ── Constructor ───────────────────────────────────────────────────────────────
BattleUI::BattleUI(const sf::Vector2u& windowSize)
    : winSize(windowSize), player(nullptr), enemy(nullptr), activeIsP1(true)
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

    // ── Barra HP Jugador 1 (abajo-izquierda) ─────────────────────────────────
    // Fondo semi-transparente con marco
    playerHpBack.setSize({220.f, 20.f});
    playerHpBack.setFillColor(sf::Color(30, 30, 30, 200));
    playerHpBack.setOutlineThickness(2.f);
    playerHpBack.setOutlineColor(sf::Color(100, 100, 100));
    playerHpBack.setPosition(20.f, 340.f);

    // Barra de color (Más chica e incrustada +2px)
    playerHpFront.setSize({216.f, 16.f});
    playerHpFront.setFillColor(sf::Color(50, 200, 50));
    playerHpFront.setPosition(22.f, 342.f);

    // ── Barra HP Jugador 2 (arriba-derecha) ───────────────────────────────────
    // Fondo semi-transparente con marco
    enemyHpBack.setSize({220.f, 20.f});
    enemyHpBack.setFillColor(sf::Color(30, 30, 30, 200));
    enemyHpBack.setOutlineThickness(2.f);
    enemyHpBack.setOutlineColor(sf::Color(100, 100, 100));
    enemyHpBack.setPosition(winSize.x - 240.f, 20.f);

    // Barra de color (Más chica e incrustada +2px)
    enemyHpFront.setSize({216.f, 16.f});
    enemyHpFront.setFillColor(sf::Color(50, 200, 50));
    enemyHpFront.setPosition(winSize.x - 238.f, 22.f);

    // ── Barras de energía ─────────────────────────────────────────────────────
    // Ajustadas un poco en Y para no encimarse con el nuevo marco
    buildEnergyBar(playerEnergyCells, 20.f,              368.f, sf::Color(0, 200, 255));
    buildEnergyBar(enemyEnergyCells,  winSize.x - 240.f, 48.f,  sf::Color(255, 180, 0));

    // ── Panel de botones (abajo-derecha, igual posición que antes) ────────────
    const float btnW = 220.f, btnH = 40.f;
    moveButtons.resize(4);
    moveTexts.resize(4);
    moveCostTexts.resize(4);

    for (int i = 0; i < 4; ++i)
    {
        float x = winSize.x - btnW - 20.f;
        float y = winSize.y - (4 - i) * (btnH + 8.f) - 20.f;

        moveButtons[i].setSize({btnW, btnH});
        moveButtons[i].setFillColor(P1_BTN_ACTIVE);
        moveButtons[i].setOutlineColor(sf::Color(120, 120, 200));
        moveButtons[i].setOutlineThickness(1.f);
        moveButtons[i].setPosition(x, y);

        moveTexts[i].setFont(font);
        moveTexts[i].setCharacterSize(14);
        moveTexts[i].setFillColor(sf::Color::White);
        moveTexts[i].setPosition(x + 6.f, y + 5.f);

        moveCostTexts[i].setFont(font);
        moveCostTexts[i].setCharacterSize(12);
        moveCostTexts[i].setFillColor(sf::Color(0, 220, 255));
        moveCostTexts[i].setPosition(x + btnW - 28.f, y + 4.f);
    }

    // ── Etiqueta de turno (encima del panel) ──────────────────────────────────
    float labelX = winSize.x - btnW - 20.f;
    float labelY = winSize.y - 4 * (btnH + 8.f) - 44.f;
    turnLabel.setFont(font);
    turnLabel.setCharacterSize(15);
    turnLabel.setStyle(sf::Text::Bold);
    turnLabel.setString("TURNO: Jugador 1");
    turnLabel.setFillColor(sf::Color(100, 160, 255));
    turnLabel.setPosition(labelX, labelY);
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

// ── Cambiar jugador activo → recarga el panel completo ────────────────────────
void BattleUI::setActivePlayer(bool isPlayer1Turn)
{
    activeIsP1 = isPlayer1Turn;
    refreshButtons();
}

// ── Recargar textos y colores del panel según quién tiene el turno ────────────
void BattleUI::refreshButtons()
{
    Pokemonster* current = activeIsP1 ? player : enemy;

    // Colores base del jugador activo
    sf::Color btnActive   = activeIsP1 ? P1_BTN_ACTIVE   : P2_BTN_ACTIVE;
    sf::Color btnDisabled = activeIsP1 ? P1_BTN_DISABLED  : P2_BTN_DISABLED;
    sf::Color outActive   = activeIsP1 ? sf::Color(120, 140, 220) : sf::Color(220, 100, 100);
    sf::Color outDisabled = activeIsP1 ? sf::Color(50, 60, 100)   : sf::Color(100, 50, 50);
    sf::Color costColor   = activeIsP1 ? sf::Color(0, 220, 255)   : sf::Color(255, 180, 60);

    // Etiqueta de turno
    if (activeIsP1)
    {
        turnLabel.setString("TURNO: Jugador 1");
        turnLabel.setFillColor(sf::Color(120, 160, 255));
    }
    else
    {
        turnLabel.setString("TURNO: Jugador 2");
        turnLabel.setFillColor(sf::Color(255, 120, 120));
    }

    if (!current) return;

    const auto& moves = current->getMoves();
    for (size_t i = 0; i < moveButtons.size() && i < moves.size(); ++i)
    {
        bool canUse = current->canUseMove(static_cast<int>(i));

        moveButtons[i].setFillColor(canUse ? btnActive : btnDisabled);
        moveButtons[i].setOutlineColor(canUse ? outActive : outDisabled);

        moveTexts[i].setString(moves[i].name);
        moveTexts[i].setFillColor(canUse ? sf::Color::White : sf::Color(110, 110, 110));

        moveCostTexts[i].setString(std::to_string(moves[i].energyCost) + "E");
        moveCostTexts[i].setFillColor(canUse ? costColor : sf::Color(80, 80, 80));
    }
}

// ── updateMoveButtons: llamado desde Game al inicializar ──────────────────────
void BattleUI::updateMoveButtons(Pokemonster* pokemon, bool isPlayer1)
{
    // Solo actualiza el panel si corresponde al jugador activo actualmente
    if (!pokemon) return;
    if (isPlayer1 == activeIsP1)
        refreshButtons();
}

// ── Update (cada frame) ───────────────────────────────────────────────────────
void BattleUI::update()
{
    // ── HP Jugador 1 ──────────────────────────────────────────────────────────
    if (player)
    {
        float ratio = (player->getHPMax() > 0)
            ? static_cast<float>(player->getHP()) / player->getHPMax() : 0.f;
        
        // Ajustamos al nuevo tamaño interior (216px)
        playerHpFront.setSize({216.f * ratio, 16.f});
        
        if      (ratio > 0.5f) playerHpFront.setFillColor(sf::Color(50, 200, 50));  // Verde brillante
        else if (ratio > 0.2f) playerHpFront.setFillColor(sf::Color(220, 200, 50)); // Amarillo
        else                   playerHpFront.setFillColor(sf::Color(220, 50, 50));  // Rojo

        // Energía J1
        int e = player->getEnergy();
        sf::Color c1(0, 200, 255);
        for (int i = 0; i < 10; ++i)
            playerEnergyCells[i].setFillColor(i < e ? c1 : sf::Color(40, 40, 40));
    }

    // ── HP Jugador 2 ──────────────────────────────────────────────────────────
    if (enemy)
    {
        float ratio = (enemy->getHPMax() > 0)
            ? static_cast<float>(enemy->getHP()) / enemy->getHPMax() : 0.f;
            
        // Ajustamos al nuevo tamaño interior (216px)
        enemyHpFront.setSize({216.f * ratio, 16.f});
        
        if      (ratio > 0.5f) enemyHpFront.setFillColor(sf::Color(50, 200, 50));  // Verde brillante
        else if (ratio > 0.2f) enemyHpFront.setFillColor(sf::Color(220, 200, 50)); // Amarillo
        else                   enemyHpFront.setFillColor(sf::Color(220, 50, 50));  // Rojo

        // Energía J2
        int e = enemy->getEnergy();
        sf::Color c2(255, 180, 0);
        for (int i = 0; i < 10; ++i)
            enemyEnergyCells[i].setFillColor(i < e ? c2 : sf::Color(40, 40, 40));
    }

    // ── Actualizar colores de botones si cambia la energía disponible ─────────
    refreshButtons();
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void BattleUI::draw(sf::RenderTarget& target)
{
    // HP
    target.draw(playerHpBack);
    target.draw(playerHpFront);
    target.draw(enemyHpBack);
    target.draw(enemyHpFront);

    // Energía
    for (auto& c : playerEnergyCells) target.draw(c);
    for (auto& c : enemyEnergyCells)  target.draw(c);

    // Etiqueta de turno
    if (font.getInfo().family.size() > 0)
        target.draw(turnLabel);

    // Panel de botones
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

// ── handleMouseClick ──────────────────────────────────────────────────────────
// Solo detecta clicks si es el turno del jugador correcto
int BattleUI::handleMouseClick(sf::Vector2i mousePos, bool isPlayer1Turn)
{
    // Si el panel no pertenece al jugador que clickeó, ignorar
    if (isPlayer1Turn != activeIsP1) return -1;

    sf::Vector2f m(static_cast<float>(mousePos.x),
                   static_cast<float>(mousePos.y));

    Pokemonster* current = activeIsP1 ? player : enemy;

    for (size_t i = 0; i < moveButtons.size(); ++i)
    {
        if (moveButtons[i].getGlobalBounds().contains(m))
        {
            if (current && !current->canUseMove(static_cast<int>(i)))
                return -1;
            return static_cast<int>(i);
        }
    }
    return -1;
}