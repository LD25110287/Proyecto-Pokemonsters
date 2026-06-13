#include "../include/BattleUI.h"
#include "../include/Pokemonster.h"
#include <iostream>

// ── Colores ───────────────────────────────────────────────────────────────────
const sf::Color BattleUI::P1_BTN_ACTIVE   = sf::Color(50,  80,  160);
const sf::Color BattleUI::P1_BTN_DISABLED = sf::Color(30,  45,  80);
const sf::Color BattleUI::P2_BTN_ACTIVE   = sf::Color(140, 40,  40);
const sf::Color BattleUI::P2_BTN_DISABLED = sf::Color(70,  25,  25);
const sf::Color BattleUI::BTN_NO_ENERGY   = sf::Color(45,  45,  45);

// ── Constantes de layout del HUD ─────────────────────────────────────────────
static constexpr float HUD_SCALE      = 0.55f;
static constexpr float IMG_W          = 691.f;
static constexpr float IMG_H          = 185.f;

// Offsets CORREGIDOS dentro del sprite HUD (para que la barra verde NO se salga)
// Valores más precisos basados en la imagen original:
// - El rectángulo negro del HP comienza en x≈163, y≈82, ancho≈517, alto≈21
static constexpr float HP_OFF_X       = 163.f * HUD_SCALE;   // 89.7
static constexpr float HP_OFF_Y       =  82.f * HUD_SCALE;   // 45.1
static constexpr float HP_MAX_W       = 517.f * HUD_SCALE;   // 284.4
static constexpr float HP_HEIGHT      =  21.f * HUD_SCALE;   // 11.6

// Offsets para las celdas de energía
static constexpr float EN_OFF_X       = 226.f * HUD_SCALE;   // 124.3
static constexpr float EN_OFF_Y       = 136.f * HUD_SCALE;   // 74.8
static constexpr float EN_CELL_W      =  24.f * HUD_SCALE;   // 13.2
static constexpr float EN_GAP         =   5.f * HUD_SCALE;   // 2.8
static constexpr float EN_CELL_H      =  14.f * HUD_SCALE;   // 7.7

// ── Constructor ───────────────────────────────────────────────────────────────
BattleUI::BattleUI(const sf::Vector2u& windowSize)
    : winSize(windowSize), player(nullptr), enemy(nullptr), activeIsP1(true)
{
    // Cargar fuente
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::vector<std::string> fallbacks = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };
        for (auto& p : fallbacks)
            if (font.loadFromFile(p)) break;
    }

    // ── HUD Jugador 1 (ARRIBA-IZQUIERDA) ──────────────────────────────────────
    if (!playerHudTexture.loadFromFile("assets/images/barra_vida_jugador_1.png"))
        std::cerr << "[Warning] No se encontro: assets/images/barra_vida_jugador_1.png\n";

    playerHudSprite.setTexture(playerHudTexture);
    playerHudSprite.setScale(HUD_SCALE, HUD_SCALE);

    // Posición: ARRIBA-IZQUIERDA
    const float p1HudX = 10.f;
    const float p1HudY = 8.f;
    playerHudSprite.setPosition(p1HudX, p1HudY);

    // Barra HP de J1 - AHORA CON LÍMITES (clip) para que no se salga
    playerHpFront.setSize({HP_MAX_W, HP_HEIGHT});
    playerHpFront.setFillColor(sf::Color(50, 200, 50));
    playerHpFront.setPosition(p1HudX + HP_OFF_X, p1HudY + HP_OFF_Y);

    // Celdas de energía de J1
    buildEnergyBar(playerEnergyCells,
                   p1HudX + EN_OFF_X,
                   p1HudY + EN_OFF_Y,
                   sf::Color(0, 200, 255));

    // ── HUD Jugador 2 (ARRIBA-DERECHA) ────────────────────────────────────────
    if (!enemyHudTexture.loadFromFile("assets/images/barra_vida_jugador_2.png"))
        std::cerr << "[Warning] No se encontro: assets/images/barra_vida_jugador_2.png\n";

    enemyHudSprite.setTexture(enemyHudTexture);
    enemyHudSprite.setScale(HUD_SCALE, HUD_SCALE);

    // Posición: ARRIBA-DERECHA
    const float p2HudX = static_cast<float>(winSize.x) - IMG_W * HUD_SCALE - 10.f;
    const float p2HudY = 8.f;
    enemyHudSprite.setPosition(p2HudX, p2HudY);

    // Barra HP de J2
    enemyHpFront.setSize({HP_MAX_W, HP_HEIGHT});
    enemyHpFront.setFillColor(sf::Color(50, 200, 50));
    enemyHpFront.setPosition(p2HudX + HP_OFF_X, p2HudY + HP_OFF_Y);

    // Celdas de energía de J2
    buildEnergyBar(enemyEnergyCells,
                   p2HudX + EN_OFF_X,
                   p2HudY + EN_OFF_Y,
                   sf::Color(255, 180, 0));

    // ── Panel de botones (abajo-derecha) ──────────────────────────────────────
    const float btnW = 220.f, btnH = 40.f;
    moveButtons.resize(4);
    moveTexts.resize(4);
    moveCostTexts.resize(4);

    for (int i = 0; i < 4; ++i)
    {
        float x = static_cast<float>(winSize.x) - btnW - 20.f;
        float y = static_cast<float>(winSize.y) - (4 - i) * (btnH + 8.f) - 20.f;

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
    float labelX = static_cast<float>(winSize.x) - btnW - 20.f;
    float labelY = static_cast<float>(winSize.y) - 4.f * (btnH + 8.f) - 44.f;
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
    for (int i = 0; i < 10; ++i)
    {
        cells[i].setSize({EN_CELL_W, EN_CELL_H});
        cells[i].setPosition(x + i * (EN_CELL_W + EN_GAP), y);
        cells[i].setFillColor(sf::Color(40, 40, 40));
        cells[i].setOutlineColor(sf::Color(80, 80, 80));
        cells[i].setOutlineThickness(0.f);
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

    sf::Color btnActive   = activeIsP1 ? P1_BTN_ACTIVE   : P2_BTN_ACTIVE;
    sf::Color btnDisabled = activeIsP1 ? P1_BTN_DISABLED  : P2_BTN_DISABLED;
    sf::Color outActive   = activeIsP1 ? sf::Color(120, 140, 220) : sf::Color(220, 100, 100);
    sf::Color outDisabled = activeIsP1 ? sf::Color(50, 60, 100)   : sf::Color(100, 50, 50);
    sf::Color costColor   = activeIsP1 ? sf::Color(0, 220, 255)   : sf::Color(255, 180, 60);

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

        // LIMITAR el ancho máximo para que NO sobrepase el rectángulo negro
        float currentWidth = HP_MAX_W * ratio;
        if (currentWidth > HP_MAX_W) currentWidth = HP_MAX_W;
        playerHpFront.setSize({currentWidth, HP_HEIGHT});

        // Cambiar color según la vida restante
        if      (ratio > 0.5f) playerHpFront.setFillColor(sf::Color(50, 200, 50));
        else if (ratio > 0.2f) playerHpFront.setFillColor(sf::Color(220, 200, 50));
        else                   playerHpFront.setFillColor(sf::Color(220, 50, 50));

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

        float currentWidth = HP_MAX_W * ratio;
        if (currentWidth > HP_MAX_W) currentWidth = HP_MAX_W;
        enemyHpFront.setSize({currentWidth, HP_HEIGHT});

        if      (ratio > 0.5f) enemyHpFront.setFillColor(sf::Color(50, 200, 50));
        else if (ratio > 0.2f) enemyHpFront.setFillColor(sf::Color(220, 200, 50));
        else                   enemyHpFront.setFillColor(sf::Color(220, 50, 50));

        int e = enemy->getEnergy();
        sf::Color c2(255, 180, 0);
        for (int i = 0; i < 10; ++i)
            enemyEnergyCells[i].setFillColor(i < e ? c2 : sf::Color(40, 40, 40));
    }

    refreshButtons();
}

// ── Draw ──────────────────────────────────────────────────────────────────────
void BattleUI::draw(sf::RenderTarget& target)
{
    // IMPORTANTE: Cambiar el orden de dibujo para que la barra HP quede DENTRO del marco
    
    // 1. Dibujar el SPRITE HUD primero (el marco)
    target.draw(playerHudSprite);
    target.draw(enemyHudSprite);
    
    // 2. Luego dibujar las barras HP ENCIMA (pero dentro del marco)
    //    Esto asegura que no se "salgan" visualmente
    target.draw(playerHpFront);
    target.draw(enemyHpFront);
    
    // 3. Celdas de energía
    for (auto& c : playerEnergyCells) target.draw(c);
    for (auto& c : enemyEnergyCells)  target.draw(c);
    
    // 4. Etiqueta de turno y panel de botones
    if (font.getInfo().family.size() > 0)
        target.draw(turnLabel);

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
int BattleUI::handleMouseClick(sf::Vector2i mousePos, bool isPlayer1Turn)
{
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