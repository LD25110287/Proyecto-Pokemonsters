#include "../include/BattleUI.h"
#include "../include/Pokemonster.h"
#include <iostream>

// ── Colores ───────────────────────────────────────────────────────────────────
const sf::Color BattleUI::P1_BTN_ACTIVE   = sf::Color(50,  80,  160);
const sf::Color BattleUI::P1_BTN_DISABLED = sf::Color(30,  45,  80);
const sf::Color BattleUI::P2_BTN_ACTIVE   = sf::Color(140, 40,  40);
const sf::Color BattleUI::P2_BTN_DISABLED = sf::Color(70,  25,  25);
const sf::Color BattleUI::BTN_NO_ENERGY   = sf::Color(45,  45,  45);

// ── Portraits por índice de personaje ────────────────────────────────────────
const char* BattleUI::PORTRAIT_PATHS[6] = {
    "assets/images/champ_select_1.png",
    "assets/images/champ_select_2.png",
    "assets/images/champ_select_3.png",
    "assets/images/champ_select_4.png",
    "assets/images/champ_select_5.png",
    "assets/images/champ_select_6.png",
};

// ── Nombres de personajes ─────────────────────────────────────────────────────
const char* BattleUI::getCharName(int index)
{
    static const char* names[6] = {
        "Exdarktyranomon", "BeelStarmon", "Bioquetzalmon",
        "Jesmon", "Sleipmon", "Magnamon"
    };
    if (index < 0 || index > 5) return "???";
    return names[index];
}

// ── Constructor ───────────────────────────────────────────────────────────────
BattleUI::BattleUI(const sf::Vector2u& windowSize)
    : winSize(windowSize), player(nullptr), enemy(nullptr), activeIsP1(true),
      p1Attribute(Attribute::Vacuna), p2Attribute(Attribute::Vacuna),
      p1Team_({0,1,2}), p2Team_({0,1,2}),
      p1ActiveSlot_(0), p2ActiveSlot_(0),
      switchIconLoaded_(false), switchPanelOpen_(false)
{
    portraitLoaded_.fill(false);

    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::vector<std::string> fallbacks = {
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };
        for (auto& p : fallbacks)
            if (font.loadFromFile(p)) break;
    }

    // ── Barra HP J1 (abajo-izquierda) ────────────────────────────────────────
    playerHpBack.setSize({220.f, 20.f});
    playerHpBack.setFillColor(sf::Color(30, 30, 30, 200));
    playerHpBack.setOutlineThickness(2.f);
    playerHpBack.setOutlineColor(sf::Color(100, 100, 100));
    playerHpBack.setPosition(60.f, 340.f);

    playerHpFront.setSize({216.f, 16.f});
    playerHpFront.setFillColor(sf::Color(50, 200, 50));
    playerHpFront.setPosition(62.f, 342.f);

    // ── Barra HP J2 (arriba-derecha) ──────────────────────────────────────────
    enemyHpBack.setSize({220.f, 20.f});
    enemyHpBack.setFillColor(sf::Color(30, 30, 30, 200));
    enemyHpBack.setOutlineThickness(2.f);
    enemyHpBack.setOutlineColor(sf::Color(100, 100, 100));
    enemyHpBack.setPosition(winSize.x - 240.f, 20.f);

    enemyHpFront.setSize({216.f, 16.f});
    enemyHpFront.setFillColor(sf::Color(50, 200, 50));
    enemyHpFront.setPosition(winSize.x - 238.f, 22.f);

    // ── Barras de energía ─────────────────────────────────────────────────────
    buildEnergyBar(playerEnergyCells, 60.f,              368.f, sf::Color(0, 200, 255));
    buildEnergyBar(enemyEnergyCells,  winSize.x - 240.f, 48.f,  sf::Color(255, 180, 0));

    // ── Panel de botones (abajo-derecha) ──────────────────────────────────────
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

    // ── Etiqueta de turno ─────────────────────────────────────────────────────
    float labelX = winSize.x - btnW - 20.f;
    float labelY = winSize.y - 4 * (btnH + 8.f) - 44.f;
    turnLabel.setFont(font);
    turnLabel.setCharacterSize(15);
    turnLabel.setStyle(sf::Text::Bold);
    turnLabel.setString("TURNO: Jugador 1");
    turnLabel.setFillColor(sf::Color(100, 160, 255));
    turnLabel.setPosition(labelX, labelY);

    // ── Panel de cambio (construido vacío, se rellena en buildSwitchPanel) ────
    switchPanelBg_.setFillColor(sf::Color(10, 10, 20, 230));
    switchPanelBg_.setOutlineColor(sf::Color(100, 100, 180));
    switchPanelBg_.setOutlineThickness(2.f);

    switchPanelTitle_.setFont(font);
    switchPanelTitle_.setCharacterSize(13);
    switchPanelTitle_.setStyle(sf::Text::Bold);
    switchPanelTitle_.setFillColor(sf::Color(200, 200, 255));

    for (int i = 0; i < 3; ++i)
    {
        switchSlotBg_[i].setOutlineThickness(1.f);

        switchSlotName_[i].setFont(font);
        switchSlotName_[i].setCharacterSize(12);
        switchSlotName_[i].setFillColor(sf::Color::White);

        switchSlotTag_[i].setFont(font);
        switchSlotTag_[i].setCharacterSize(10);
        switchSlotTag_[i].setStyle(sf::Text::Bold);
        switchSlotTag_[i].setFillColor(sf::Color(255, 215, 0));
    }
}

// ── buildEnergyBar ────────────────────────────────────────────────────────────
void BattleUI::buildEnergyBar(std::vector<sf::RectangleShape>& cells,
                               float x, float y, sf::Color)
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

// ── Setters de equipo ─────────────────────────────────────────────────────────
void BattleUI::setP1Team(const std::array<int,3>& team, int activeSlot)
{
    p1Team_       = team;
    p1ActiveSlot_ = activeSlot;
}

void BattleUI::setP2Team(const std::array<int,3>& team, int activeSlot)
{
    p2Team_       = team;
    p2ActiveSlot_ = activeSlot;
}

// ── updateActiveSlot ──────────────────────────────────────────────────────────
void BattleUI::updateActiveSlot(bool isP1, int newSlot)
{
    if (isP1) p1ActiveSlot_ = newSlot;
    else      p2ActiveSlot_ = newSlot;
}

// ── loadSwitchIcons ───────────────────────────────────────────────────────────
void BattleUI::loadSwitchIcons()
{
    bool ok1 = switchTexP1_.loadFromFile("assets/images/cambio_P1.png");
    bool ok2 = switchTexP2_.loadFromFile("assets/images/cambio_P2.png");
    switchIconLoaded_ = ok1 && ok2;

    if (!switchIconLoaded_)
        std::cerr << "[BattleUI] No se cargaron los iconos de cambio\n";

    // Posición inicial: a la izquierda del turnLabel
    // turnLabel está en (winSize.x - 240, labelY)
    // Ponemos el ícono justo antes del texto de turno
    const float iconSize = 28.f;
    const float btnW = 220.f;
    float labelX = winSize.x - btnW - 20.f;
    float labelY = winSize.y - 4 * (40.f + 8.f) - 44.f;

    // Ícono a la derecha del texto "TURNO: Jugador X", alineado verticalmente
    // Lo ponemos al extremo derecho del panel, junto al label
    float iconX = winSize.x - 20.f - iconSize;   // borde derecho del panel
    float iconY = labelY - 2.f;

    switchIcon_.setPosition(iconX, iconY);

    // Escalar ícono a iconSize x iconSize
    if (ok1)
    {
        sf::Vector2u sz = switchTexP1_.getSize();
        if (sz.x > 0 && sz.y > 0)
            switchIcon_.setScale(iconSize / sz.x, iconSize / sz.y);
        switchIcon_.setTexture(switchTexP1_);
    }

    switchIconBounds_ = sf::FloatRect(iconX, iconY, iconSize, iconSize);
}

// ── loadAttributeIcons ────────────────────────────────────────────────────────
void BattleUI::loadAttributeIcons()
{
    if (!attrTexVa.loadFromFile("assets/images/atributo_Va.png"))
        std::cerr << "[BattleUI] No se cargo atributo_Va.png\n";
    if (!attrTexVi.loadFromFile("assets/images/atributo_Vi.png"))
        std::cerr << "[BattleUI] No se cargo atributo_Vi.png\n";
    if (!attrTexDa.loadFromFile("assets/images/atributo_Da.png"))
        std::cerr << "[BattleUI] No se cargo atributo_Da.png\n";

    p1AttrSprite.setScale(0.7f, 0.7f);
    p1AttrSprite.setPosition(60.f - 38.f, 338.f);

    p2AttrSprite.setScale(0.7f, 0.7f);
    p2AttrSprite.setPosition(winSize.x - 240.f - 38.f, 18.f);
}

// ── setPlayerPokemon / setEnemyPokemon ────────────────────────────────────────
void BattleUI::setPlayerPokemon(Pokemonster* p) { player = p; }
void BattleUI::setEnemyPokemon(Pokemonster* e)  { enemy  = e; }

// ── setActivePlayer ───────────────────────────────────────────────────────────
void BattleUI::setActivePlayer(bool isPlayer1Turn)
{
    activeIsP1 = isPlayer1Turn;

    // Actualizar textura del ícono de cambio según jugador activo
    if (switchIconLoaded_)
    {
        if (activeIsP1)
            switchIcon_.setTexture(switchTexP1_);
        else
            switchIcon_.setTexture(switchTexP2_);
    }

    // Cerrar panel de cambio al cambiar de turno
    switchPanelOpen_ = false;

    refreshButtons();
}

// ── refreshButtons ────────────────────────────────────────────────────────────
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

// ── buildSwitchPanel ──────────────────────────────────────────────────────────
// Construye el panel con los 3 personajes del jugador activo
void BattleUI::buildSwitchPanel()
{
    const std::array<int,3>& team      = activeIsP1 ? p1Team_ : p2Team_;
    const int                activeSlot = activeIsP1 ? p1ActiveSlot_ : p2ActiveSlot_;

    // Colores de acento del jugador activo
    sf::Color accentColor = activeIsP1 ? sf::Color(80, 120, 220) : sf::Color(200, 60, 60);
    sf::Color accentOut   = activeIsP1 ? sf::Color(120, 160, 255): sf::Color(255, 100, 100);

    // Dimensiones del panel
    const float panelW  = 220.f;
    const float slotH   = 52.f;
    const float slotGap = 6.f;
    const float padY    = 30.f;   // espacio para el título
    const float padX    = 8.f;
    const float panelH  = padY + 3 * slotH + 2 * slotGap + 8.f;

    // Anclar el panel justo encima del turnLabel
    float panelX = winSize.x - panelW - 20.f;
    float panelY = winSize.y - 4 * (40.f + 8.f) - 44.f - panelH - 6.f;

    switchPanelBg_.setSize({panelW, panelH});
    switchPanelBg_.setPosition(panelX, panelY);
    switchPanelBg_.setOutlineColor(accentOut);

    switchPanelTitle_.setString("Cambiar personaje:");
    switchPanelTitle_.setPosition(panelX + padX, panelY + 6.f);

    // Pre-cargar portraits necesarios.
    // IMPORTANTE: cargamos TODOS antes de asignar al sprite para evitar que
    // loadFromFile() mueva la textura en memoria e invalide punteros de sprites
    // previos (bug del cuadro blanco en SFML 2.x).
    for (int i = 0; i < 3; ++i)
    {
        int charIdx = team[i];
        if (charIdx >= 0 && charIdx < 6)
        {
            // Siempre recargamos: garantiza que la textura esté en memoria fresca
            if (portraitTextures_[charIdx].loadFromFile(PORTRAIT_PATHS[charIdx]))
            {
                portraitTextures_[charIdx].setSmooth(true);
                portraitLoaded_[charIdx] = true;
            }
            else
            {
                portraitLoaded_[charIdx] = false;
            }
        }
    }

    // Ahora que todas las texturas están estables en memoria, asignamos a sprites
    const float portSize = slotH - 6.f;
    for (int i = 0; i < 3; ++i)
    {
        int charIdx = team[i];
        if (charIdx >= 0 && charIdx < 6 && portraitLoaded_[charIdx])
        {
            // resetRect=true fuerza recalculo del textureRect, evita cuadro blanco
            switchSlotPortrait_[i].setTexture(portraitTextures_[charIdx], true);
            sf::Vector2u sz = portraitTextures_[charIdx].getSize();
            if (sz.x > 0 && sz.y > 0)
                switchSlotPortrait_[i].setScale(portSize / sz.x, portSize / sz.y);
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        float slotX = panelX + padX;
        float slotY = panelY + padY + i * (slotH + slotGap);

        bool isActive = (i == activeSlot);

        // Fondo del slot
        switchSlotBg_[i].setSize({panelW - 2 * padX, slotH});
        switchSlotBg_[i].setPosition(slotX, slotY);

        if (isActive)
        {
            // Slot en campo: más oscuro / apagado
            switchSlotBg_[i].setFillColor(sf::Color(20, 20, 20, 180));
            switchSlotBg_[i].setOutlineColor(sf::Color(80, 80, 80));
        }
        else
        {
            switchSlotBg_[i].setFillColor(accentColor);
            switchSlotBg_[i].setOutlineColor(accentOut);
        }

        // Posicionar portrait dentro del slot
        int charIdx = team[i];
        if (charIdx >= 0 && charIdx < 6 && portraitLoaded_[charIdx])
        {
            switchSlotPortrait_[i].setPosition(slotX + 3.f, slotY + 3.f);
        }

        // Nombre del personaje
        switchSlotName_[i].setString(getCharName(charIdx));
        switchSlotName_[i].setPosition(slotX + portSize + 8.f, slotY + 6.f);
        switchSlotName_[i].setFillColor(isActive ? sf::Color(120, 120, 120) : sf::Color::White);

        // Tag "EN CAMPO" si está activo
        switchSlotTag_[i].setString(isActive ? "EN CAMPO" : "");
        switchSlotTag_[i].setPosition(slotX + portSize + 8.f, slotY + 26.f);
    }
}

// ── updateMoveButtons ─────────────────────────────────────────────────────────
void BattleUI::updateMoveButtons(Pokemonster* pokemon, bool isPlayer1)
{
    if (!pokemon) return;
    if (isPlayer1 == activeIsP1)
        refreshButtons();
}

// ── update ────────────────────────────────────────────────────────────────────
void BattleUI::update()
{
    // HP J1
    if (player)
    {
        float ratio = (player->getHPMax() > 0)
            ? static_cast<float>(player->getHP()) / player->getHPMax() : 0.f;
        playerHpFront.setSize({216.f * ratio, 16.f});
        if      (ratio > 0.5f) playerHpFront.setFillColor(sf::Color(50, 200, 50));
        else if (ratio > 0.2f) playerHpFront.setFillColor(sf::Color(220, 200, 50));
        else                   playerHpFront.setFillColor(sf::Color(220, 50, 50));

        int e = player->getEnergy();
        sf::Color c1(0, 200, 255);
        for (int i = 0; i < 10; ++i)
            playerEnergyCells[i].setFillColor(i < e ? c1 : sf::Color(40, 40, 40));
    }

    // HP J2
    if (enemy)
    {
        float ratio = (enemy->getHPMax() > 0)
            ? static_cast<float>(enemy->getHP()) / enemy->getHPMax() : 0.f;
        enemyHpFront.setSize({216.f * ratio, 16.f});
        if      (ratio > 0.5f) enemyHpFront.setFillColor(sf::Color(50, 200, 50));
        else if (ratio > 0.2f) enemyHpFront.setFillColor(sf::Color(220, 200, 50));
        else                   enemyHpFront.setFillColor(sf::Color(220, 50, 50));

        int e = enemy->getEnergy();
        sf::Color c2(255, 180, 0);
        for (int i = 0; i < 10; ++i)
            enemyEnergyCells[i].setFillColor(i < e ? c2 : sf::Color(40, 40, 40));
    }

    // Íconos de atributo
    auto applyAttrTex = [&](sf::Sprite& spr, Attribute a) {
        switch (a) {
            case Attribute::Vacuna: if (attrTexVa.getSize().x>0) spr.setTexture(attrTexVa); break;
            case Attribute::Virus:  if (attrTexVi.getSize().x>0) spr.setTexture(attrTexVi); break;
            case Attribute::Data:   if (attrTexDa.getSize().x>0) spr.setTexture(attrTexDa); break;
        }
    };
    applyAttrTex(p1AttrSprite, p1Attribute);
    applyAttrTex(p2AttrSprite, p2Attribute);

    refreshButtons();
}

// ── draw ──────────────────────────────────────────────────────────────────────
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

    // Íconos de atributo
    if (attrTexVa.getSize().x > 0 || attrTexVi.getSize().x > 0 || attrTexDa.getSize().x > 0)
    {
        target.draw(p1AttrSprite);
        target.draw(p2AttrSprite);
    }

    // Etiqueta de turno
    if (font.getInfo().family.size() > 0)
        target.draw(turnLabel);

    // Ícono de cambio (si está cargado)
    if (switchIconLoaded_)
        target.draw(switchIcon_);

    // Panel de botones de movimiento (solo si el panel de cambio está cerrado)
    if (!switchPanelOpen_)
    {
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
    else
    {
        // Panel de selección de cambio
        target.draw(switchPanelBg_);
        if (font.getInfo().family.size() > 0)
            target.draw(switchPanelTitle_);

        for (int i = 0; i < 3; ++i)
        {
            target.draw(switchSlotBg_[i]);

            // Portrait (si cargó)
            int charIdx = (activeIsP1 ? p1Team_ : p2Team_)[i];
            if (charIdx >= 0 && charIdx < 6 && portraitLoaded_[charIdx])
                target.draw(switchSlotPortrait_[i]);

            if (font.getInfo().family.size() > 0)
            {
                target.draw(switchSlotName_[i]);
                target.draw(switchSlotTag_[i]);
            }
        }
    }
}

// ── handleMouseClick ──────────────────────────────────────────────────────────
int BattleUI::handleMouseClick(sf::Vector2i mousePos, bool isPlayer1Turn)
{
    if (isPlayer1Turn != activeIsP1) return -1;

    sf::Vector2f m(static_cast<float>(mousePos.x),
                   static_cast<float>(mousePos.y));

    // Si el panel de cambio está abierto, los clicks de movimiento se ignoran
    if (switchPanelOpen_) return -1;

    // ── Click en el ícono de cambio ───────────────────────────────────────────
    if (switchIconLoaded_ && switchIconBounds_.contains(m))
    {
        buildSwitchPanel();
        switchPanelOpen_ = true;
        return -1;   // no es un movimiento, signal especial manejado por el caller
    }

    // ── Click en botones de movimiento ────────────────────────────────────────
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

// ── handleSwitchClick ─────────────────────────────────────────────────────────
// Llamado desde Game cuando el panel de cambio está abierto.
// Retorna el slot seleccionado (0-2) o -1 (click fuera / slot activo).
int BattleUI::handleSwitchClick(sf::Vector2i mousePos)
{
    if (!switchPanelOpen_) return -1;

    sf::Vector2f m(static_cast<float>(mousePos.x),
                   static_cast<float>(mousePos.y));

    int activeSlot = activeIsP1 ? p1ActiveSlot_ : p2ActiveSlot_;

    for (int i = 0; i < 3; ++i)
    {
        if (switchSlotBg_[i].getGlobalBounds().contains(m))
        {
            if (i == activeSlot) return -1;   // ya está en campo

            // Cierra el panel al seleccionar
            switchPanelOpen_ = false;
            return i;
        }
    }

    // Click fuera del panel → cerrar sin cambiar
    if (!switchPanelBg_.getGlobalBounds().contains(m))
        switchPanelOpen_ = false;

    return -1;
}
