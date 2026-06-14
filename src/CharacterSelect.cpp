#include "../include/CharacterSelect.h"
#include <iostream>
#include <algorithm>

// ── Helpers internos ──────────────────────────────────────────────────────────
// Devuelve el índice en attrTextures[] para un Attribute dado
static int attrIndex(Attribute a)
{
    switch (a) {
        case Attribute::Vacuna: return 0;
        case Attribute::Virus:  return 1;
        case Attribute::Data:   return 2;
    }
    return 0;
}

// ── Constructor ───────────────────────────────────────────────────────────────
CharacterSelect::CharacterSelect()
    : window(sf::VideoMode(800, 600), "Pokemonsters - Seleccion de Personaje", sf::Style::Close)
    , phase(Phase::PICKING)
    , hoveredCard(-1)
    , pickTurn(0)
    , orderSlot(0)
    , orderTurn(0)
    , selectedStage(-1)
    , stageAnimDone(false)
    , launchBattle(false)
{
    window.setFramerateLimit(60);
    p1Team.fill(-1);
    p2Team.fill(-1);
    attrLoaded[0] = attrLoaded[1] = attrLoaded[2] = false;

    characters = {
        { "Exdarktyranomon", "assets/images/1.png", "assets/images/champ_select_1.png",  50,  50, 500, 22, 8  },
        { "BeelStarmon",     "assets/images/2.png", "assets/images/champ_select_2.png", 100, 100, 475, 20, 6  },
        { "Bioquetzalmon",   "assets/images/3.png", "assets/images/champ_select_3.png", 100, 100, 450, 18, 10 },
        { "Jesmon",          "assets/images/4.png", "assets/images/champ_select_4.png", 100, 100, 525, 25, 7  },
        { "Sleipmon",        "assets/images/5.png", "assets/images/champ_select_5.png", 150,  80, 550, 19, 12 },
        { "Magnamon",        "assets/images/6.png", "assets/images/champ_select_6.png", 100, 100, 490, 23, 9  }
    };

    loadAssets();
}

// ── loadAssets ────────────────────────────────────────────────────────────────
void CharacterSelect::loadAssets()
{
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (const auto& p : fontPaths)
        if (font.loadFromFile(p)) break;

    // Fondo VS
    if (!vsBgTexture.loadFromFile("assets/images/imagen vs.png"))
        std::cerr << "No se pudo cargar imagen vs.png\n";
    else
    {
        vsBgSprite.setTexture(vsBgTexture);
        auto sz = vsBgTexture.getSize();
        vsBgSprite.setScale(800.f / sz.x, 600.f / sz.y);
    }

    // Fondos de escenario
    const std::string stagePaths[NUM_STAGES] = {
        "assets/images/fondo_batalla_campo.png",
        "assets/images/fondo_batalla_red_crystal.png",
        "assets/images/fondo_batalla_arena.png"
    };
    for (int i = 0; i < NUM_STAGES; ++i)
    {
        if (stageBgTextures[i].loadFromFile(stagePaths[i]))
        {
            stageBgSprites[i].setTexture(stageBgTextures[i]);
            auto sz = stageBgTextures[i].getSize();
            stageBgSprites[i].setScale(800.f / sz.x, 600.f / sz.y);
        }
    }

    // Portraits
    portraits.resize(characters.size());
    portraitSprites.resize(characters.size());
    for (size_t i = 0; i < characters.size(); ++i)
    {
        if (!portraits[i].loadFromFile(characters[i].portraitPath))
            std::cerr << "No se pudo cargar: " << characters[i].portraitPath << "\n";
        else
        {
            portraitSprites[i].setTexture(portraits[i]);
            auto sz = portraits[i].getSize();
            portraitSprites[i].setScale(100.f / sz.x, 100.f / sz.y);
        }
    }

    // ── Íconos de atributo ────────────────────────────────────────────────────
    // 0 = Vacuna → atributo_Va.png
    // 1 = Virus  → atributo_Vi.png
    // 2 = Data   → atributo_Da.png
    const std::string attrPaths[3] = {
        "assets/images/atributo_Va.png",
        "assets/images/atributo_Vi.png",
        "assets/images/atributo_Da.png",
    };
    for (int i = 0; i < 3; ++i)
    {
        if (attrTextures[i].loadFromFile(attrPaths[i]))
            attrLoaded[i] = true;
        else
            std::cerr << "[Warning] No se pudo cargar: " << attrPaths[i] << "\n";
    }
}

// ── drawAttributeIcon ─────────────────────────────────────────────────────────
// Dibuja el ícono del atributo escalado a iconSize×iconSize px, con top-left en (x, y)
void CharacterSelect::drawAttributeIcon(Attribute attr, float x, float y, float iconSize)
{
    int idx = attrIndex(attr);
    if (!attrLoaded[idx]) return;

    sf::Sprite icon(attrTextures[idx]);
    auto sz = attrTextures[idx].getSize();
    if (sz.x == 0 || sz.y == 0) return;

    icon.setScale(iconSize / sz.x, iconSize / sz.y);
    icon.setPosition(x, y);
    window.draw(icon);
}

// ── drawTextShadow ────────────────────────────────────────────────────────────
void CharacterSelect::drawTextShadow(const std::string& str, unsigned int size,
                                     sf::Color color, float x, float y,
                                     sf::Text::Style style)
{
    sf::Text shadow(str, font, size);
    shadow.setFillColor(sf::Color(0, 0, 0, 190));
    shadow.setStyle(style);
    shadow.setPosition(x + 2.f, y + 2.f);
    window.draw(shadow);

    sf::Text text(str, font, size);
    text.setFillColor(color);
    text.setStyle(style);
    text.setPosition(x, y);
    window.draw(text);
}

// ── drawPortrait ──────────────────────────────────────────────────────────────
void CharacterSelect::drawPortrait(int charIndex, float cx, float cy, float size,
                                   sf::Color borderColor, float borderThick)
{
    if (charIndex < 0 || charIndex >= (int)portraits.size()) return;
    if (portraits[charIndex].getSize().x == 0) return;

    auto sz = portraits[charIndex].getSize();
    float scaleX = size / static_cast<float>(sz.x);
    float scaleY = size / static_cast<float>(sz.y);

    sf::Sprite local(portraits[charIndex]);
    local.setScale(scaleX, scaleY);
    local.setPosition(cx - size / 2.f, cy - size / 2.f);

    if (borderThick > 0.f && borderColor != sf::Color::Transparent)
    {
        sf::RectangleShape border(sf::Vector2f(size + borderThick * 2.f,
                                               size + borderThick * 2.f));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(borderColor);
        border.setOutlineThickness(borderThick);
        border.setPosition(cx - size / 2.f - borderThick,
                           cy - size / 2.f - borderThick);
        window.draw(border);
    }

    window.draw(local);
}

// ── getCardAt ─────────────────────────────────────────────────────────────────
int CharacterSelect::getCardAt(sf::Vector2i mousePos) const
{
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;
            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);
            if (sf::FloatRect(x, y, CARD_W, CARD_H).contains(
                    static_cast<sf::Vector2f>(mousePos)))
                return idx;
        }
    return -1;
}

// ── handleEvents ─────────────────────────────────────────────────────────────
void CharacterSelect::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseMoved && phase == Phase::PICKING)
            hoveredCard = getCardAt(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);

            // ── PICKING ──────────────────────────────────────────────────────
            if (phase == Phase::PICKING)
            {
                int clicked = getCardAt(mp);
                if (clicked < 0) continue;

                bool isJ1Turn = (pickTurn % 2 == 0);

                if (isJ1Turn)
                {
                    bool alreadyPicked = std::find(p1Picks.begin(), p1Picks.end(), clicked) != p1Picks.end();
                    if (alreadyPicked) continue;
                    p1Picks.push_back(clicked);
                }
                else
                {
                    bool alreadyPicked = std::find(p2Picks.begin(), p2Picks.end(), clicked) != p2Picks.end();
                    if (alreadyPicked) continue;
                    p2Picks.push_back(clicked);
                }

                pickTurn++;
                if (pickTurn >= 6)
                {
                    phase      = Phase::ORDER;
                    orderTurn  = 0;
                    orderSlot  = 0;
                    orderSelected.clear();
                }
            }

            // ── ORDER ────────────────────────────────────────────────────────
            else if (phase == Phase::ORDER)
            {
                const bool draftIsJ1[6]  = { true,  false, false, true,  true,  false };
                const int  draftSlot[6]  = { 0,     0,     1,     1,     2,     2     };

                bool  curIsJ1 = draftIsJ1[orderTurn];
                int   curSlot = draftSlot[orderTurn];

                const std::vector<int>& picks = curIsJ1 ? p1Picks : p2Picks;

                for (int i = 0; i < 3; ++i)
                {
                    float cx   = 130.f + i * 160.f;
                    float cy   = 430.f;
                    float half = 55.f;
                    sf::FloatRect rect(cx - half, cy - half, half * 2, half * 2);

                    if (!rect.contains(static_cast<sf::Vector2f>(mp))) continue;

                    bool alreadyPlaced = false;
                    if (curIsJ1) {
                        for (int t = 0; t < 3; ++t) if (p1Team[t] == picks[i]) alreadyPlaced = true;
                    } else {
                        for (int t = 0; t < 3; ++t) if (p2Team[t] == picks[i]) alreadyPlaced = true;
                    }

                    if (alreadyPlaced) break;

                    if (curIsJ1) p1Team[curSlot] = picks[i];
                    else         p2Team[curSlot] = picks[i];

                    orderTurn++;
                    if (orderTurn >= 6) phase = Phase::CONFIRM;
                    break;
                }
            }

            // ── CONFIRM ──────────────────────────────────────────────────────
            else if (phase == Phase::CONFIRM)
            {
                sf::FloatRect btnJugar(220.f, 530.f, 160.f, 50.f);
                sf::FloatRect btnVolver(420.f, 530.f, 160.f, 50.f);

                if (btnJugar.contains(static_cast<sf::Vector2f>(mp)))
                {
                    phase = Phase::STAGE_SELECT;
                    selectedStage   = -1;
                    stageAnimDone   = false;
                    stageAnimClock.restart();
                }
                else if (btnVolver.contains(static_cast<sf::Vector2f>(mp)))
                {
                    pickTurn = 0;
                    p1Picks.clear();
                    p2Picks.clear();
                    p1Team.fill(-1);
                    p2Team.fill(-1);
                    orderSelected.clear();
                    orderSlot  = 0;
                    orderTurn  = 0;
                    phase = Phase::PICKING;
                    hoveredCard = -1;
                }
            }

            // ── STAGE_SELECT ─────────────────────────────────────────────────
            else if (phase == Phase::STAGE_SELECT && stageAnimDone)
            {
                launchBattle = true;
                window.close();
            }
        }
    }
}

// ── update ────────────────────────────────────────────────────────────────────
void CharacterSelect::update()
{
    // Lógica de animación dentro del render (drawStageSelect)
}

// ── drawPickingScreen ─────────────────────────────────────────────────────────
void CharacterSelect::drawPickingScreen()
{
    window.clear(sf::Color(18, 18, 32));

    bool isJ1Turn = (pickTurn % 2 == 0);
    sf::Color j1Color(100, 180, 255);
    sf::Color j2Color(255, 120, 120);

    drawTextShadow("Jugador 1 - Elige tu personaje", 18,
                   isJ1Turn ? j1Color : sf::Color(80, 80, 120),
                   10.f, 10.f,
                   isJ1Turn ? sf::Text::Bold : sf::Text::Regular);

    {
        sf::Text t("Jugador 2 - Elige tu personaje", font, 18);
        float tx = 800.f - t.getLocalBounds().width - 10.f;
        drawTextShadow("Jugador 2 - Elige tu personaje", 18,
                       !isJ1Turn ? j2Color : sf::Color(120, 80, 80),
                       tx, 10.f,
                       !isJ1Turn ? sf::Text::Bold : sf::Text::Regular);
    }

    for (int i = 0; i < (int)p1Picks.size(); ++i)
        drawPortrait(p1Picks[i], 30.f + i * 50.f, 80.f, 40.f, j1Color, 2.f);

    for (int i = 0; i < (int)p2Picks.size(); ++i)
        drawPortrait(p2Picks[i], 800.f - 30.f - i * 50.f, 80.f, 40.f, j2Color, 2.f);

    std::string turnMsg = isJ1Turn ? ">>> Turno de Jugador 1 <<<" : ">>> Turno de Jugador 2 <<<";
    sf::Color turnColor = isJ1Turn ? j1Color : j2Color;
    {
        sf::Text t(turnMsg, font, 16);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow(turnMsg, 16, turnColor, tx, 118.f, sf::Text::Bold);
    }

    // ── Grid de cartas ────────────────────────────────────────────────────────
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;

            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);

            sf::Color fillColor(40, 40, 60);
            sf::Color outColor(100, 100, 140);
            float outThick = 2.f;

            bool pickedByJ1 = std::find(p1Picks.begin(), p1Picks.end(), idx) != p1Picks.end();
            bool pickedByJ2 = std::find(p2Picks.begin(), p2Picks.end(), idx) != p2Picks.end();

            if (pickedByJ1 && pickedByJ2)
            { fillColor = sf::Color(60, 40, 80);  outColor = sf::Color(180, 100, 255); outThick = 3.f; }
            else if (pickedByJ1)
            { fillColor = sf::Color(20, 50, 100); outColor = j1Color; outThick = 3.f; }
            else if (pickedByJ2)
            { fillColor = sf::Color(80, 20, 20);  outColor = j2Color; outThick = 3.f; }
            else if (idx == hoveredCard)
            { fillColor = sf::Color(60, 60, 90);  outColor = sf::Color(255, 215, 0); outThick = 3.f; }

            sf::RectangleShape card(sf::Vector2f(CARD_W, CARD_H));
            card.setFillColor(fillColor);
            card.setOutlineColor(outColor);
            card.setOutlineThickness(outThick);
            card.setPosition(x, y);
            window.draw(card);

            // Portrait centrado en la carta
            drawPortrait(idx, x + CARD_W / 2.f, y + CARD_H / 2.f - 15.f, 120.f);

            // Nombre del personaje
            sf::Text nm(characters[idx].name, font, 13);
            nm.setFillColor(sf::Color::White);
            nm.setPosition(x + (CARD_W - nm.getLocalBounds().width) / 2.f, y + CARD_H - 28.f);
            window.draw(nm);

            // ── Ícono de atributo: esquina inferior-izquierda de la imagen ────
            // La imagen del portrait ocupa 120×120 px centrada en (x+CARD_W/2, y+CARD_H/2-15)
            // → top-left de la imagen = (x+CARD_W/2-60, y+CARD_H/2-75)
            // → bottom-left           = (x+CARD_W/2-60, y+CARD_H/2+45)
            // Colocamos el ícono (24×24) con su esquina top-left en ese bottom-left
            {
                const float ICON_SIZE   = 24.f;
                float imgLeft   = x + CARD_W / 2.f - 60.f;
                float imgBottom = y + CARD_H / 2.f - 15.f + 60.f;   // bottom de la imagen
                float iconX = imgLeft;
                float iconY = imgBottom - ICON_SIZE;   // arriba del borde inferior
                drawAttributeIcon(CHAR_ATTRIBUTES[idx], iconX, iconY, ICON_SIZE);
            }

            if (pickedByJ1)
                drawTextShadow("J1", 12, j1Color, x + 4.f, y + 4.f, sf::Text::Bold);
            if (pickedByJ2)
            {
                sf::Text t2("J2", font, 12);
                drawTextShadow("J2", 12, j2Color, x + CARD_W - t2.getLocalBounds().width - 8.f, y + 4.f, sf::Text::Bold);
            }
        }

    window.display();
}

// ── drawOrderScreen ───────────────────────────────────────────────────────────
void CharacterSelect::drawOrderScreen(bool isP1, int targetRound)
{
    window.clear(sf::Color(18, 18, 32));

    sf::Color playerColor = isP1 ? sf::Color(100, 180, 255) : sf::Color(255, 120, 120);
    std::string playerName = isP1 ? "Jugador 1" : "Jugador 2";
    const std::vector<int>& picks = isP1 ? p1Picks : p2Picks;

    std::string title = playerName + " - Elige quien pelea en Ronda " + std::to_string(targetRound + 1);
    {
        sf::Text t(title, font, 22);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow(title, 22, playerColor, tx, 20.f, sf::Text::Bold);
    }

    drawTextShadow("Haz clic en el personaje que quieres para esta ronda", 15, sf::Color(200, 200, 200), 70.f, 60.f);

    const std::array<int,3>& team = isP1 ? p1Team : p2Team;
    for (int i = 0; i < 3; ++i)
    {
        float cx = 200.f + i * 200.f;
        float cy = 200.f;

        std::string slotLabel = "Ronda " + std::to_string(i + 1);
        {
            sf::Text t(slotLabel, font, 16);
            drawTextShadow(slotLabel, 16, sf::Color(200, 200, 200), cx - t.getLocalBounds().width / 2.f, cy - 90.f);
        }

        if (team[i] >= 0)
        {
            sf::Color borderCol = (i == targetRound) ? sf::Color(255, 215, 0) : playerColor;
            drawPortrait(team[i], cx, cy, 120.f, borderCol, 3.f);
            drawTextShadow(characters[team[i]].name, 14, sf::Color::White, cx - 50.f, cy + 65.f);
        }
        else
        {
            sf::RectangleShape slot(sf::Vector2f(120.f, 120.f));
            slot.setFillColor(sf::Color(30, 30, 50));
            slot.setOutlineColor(i == targetRound ? sf::Color(255, 215, 0) : sf::Color(80, 80, 100));
            slot.setOutlineThickness(i == targetRound ? 3.f : 2.f);
            slot.setPosition(cx - 60.f, cy - 60.f);
            window.draw(slot);
            if (i == targetRound)
                drawTextShadow("?", 32, sf::Color(255, 215, 0), cx - 10.f, cy - 20.f);
        }
    }

    sf::RectangleShape sep(sf::Vector2f(700.f, 2.f));
    sep.setFillColor(sf::Color(80, 80, 120));
    sep.setPosition(50.f, 310.f);
    window.draw(sep);

    drawTextShadow("Tu equipo:", 16, sf::Color(180, 180, 255), 50.f, 320.f);

    for (int i = 0; i < 3; ++i)
    {
        float cx = 130.f + i * 160.f;
        float cy = 430.f;

        bool alreadyPlaced = false;
        if (isP1) {
            for (int t = 0; t < 3; ++t) if (p1Team[t] == picks[i]) alreadyPlaced = true;
        } else {
            for (int t = 0; t < 3; ++t) if (p2Team[t] == picks[i]) alreadyPlaced = true;
        }

        sf::Color borderCol = alreadyPlaced ? sf::Color(60, 60, 60) : playerColor;
        drawPortrait(picks[i], cx, cy, 110.f, borderCol, 3.f);

        if (alreadyPlaced)
        {
            sf::RectangleShape overlay(sf::Vector2f(110.f, 110.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            overlay.setPosition(cx - 55.f, cy - 55.f);
            window.draw(overlay);
            drawTextShadow("OK", 20, sf::Color(100, 255, 100), cx - 15.f, cy - 12.f, sf::Text::Bold);
        }

        // ── Ícono de atributo en la pantalla de orden ─────────────────────────
        // La imagen tiene 110×110 px centrada en (cx, cy)
        // Esquina inferior-izquierda → (cx-55, cy+55-ICON_SIZE)
        {
            const float ICON_SIZE = 22.f;
            drawAttributeIcon(CHAR_ATTRIBUTES[picks[i]],
                              cx - 55.f,
                              cy + 55.f - ICON_SIZE,
                              ICON_SIZE);
        }

        drawTextShadow(characters[picks[i]].name, 13,
                       alreadyPlaced ? sf::Color(100, 100, 100) : sf::Color::White,
                       cx - 50.f, cy + 60.f);
    }

    window.display();
}

// ── drawConfirmScreen ─────────────────────────────────────────────────────────
void CharacterSelect::drawConfirmScreen()
{
    if (vsBgTexture.getSize().x > 0)
        window.draw(vsBgSprite);
    else
        window.clear(sf::Color(18, 18, 32));

    {
        sf::Text t("CONFIRMAR EQUIPOS", font, 36);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow("CONFIRMAR EQUIPOS", 36, sf::Color(255, 215, 0), tx, 20.f, sf::Text::Bold);
    }

    drawTextShadow("JUGADOR 1", 24, sf::Color(100, 180, 255), 100.f, 80.f, sf::Text::Bold);
    drawTextShadow("JUGADOR 2", 24, sf::Color(255, 120, 120), 580.f, 80.f, sf::Text::Bold);

    for (int i = 0; i < 3; ++i)
    {
        // ── J1 ────────────────────────────────────────────────────────────────
        float cx1 = 150.f;
        float cy1 = 170.f + i * 130.f;

        drawPortrait(p1Team[i], cx1, cy1, 100.f, sf::Color::Transparent, 0.f);

        // Nombre con ícono de atributo a su izquierda
        sf::Text n1(characters[p1Team[i]].name, font, 16);
        float nameX1 = cx1 - n1.getLocalBounds().width / 2.f;
        float nameY1 = cy1 + 55.f;

        // Ícono a la izquierda del nombre
        const float ICON_SIZE = 18.f;
        drawAttributeIcon(CHAR_ATTRIBUTES[p1Team[i]],
                          nameX1 - ICON_SIZE - 4.f,
                          nameY1 + (n1.getLocalBounds().height - ICON_SIZE) / 2.f,
                          ICON_SIZE);

        drawTextShadow(characters[p1Team[i]].name, 16, sf::Color::White, nameX1, nameY1);

        // ── J2 ────────────────────────────────────────────────────────────────
        float cx2 = 640.f;
        float cy2 = 170.f + i * 130.f;

        drawPortrait(p2Team[i], cx2, cy2, 100.f, sf::Color::Transparent, 0.f);

        sf::Text n2(characters[p2Team[i]].name, font, 16);
        float nameX2 = cx2 - n2.getLocalBounds().width / 2.f;
        float nameY2 = cy2 + 55.f;

        drawAttributeIcon(CHAR_ATTRIBUTES[p2Team[i]],
                          nameX2 - ICON_SIZE - 4.f,
                          nameY2 + (n2.getLocalBounds().height - ICON_SIZE) / 2.f,
                          ICON_SIZE);

        drawTextShadow(characters[p2Team[i]].name, 16, sf::Color::White, nameX2, nameY2);
    }

    sf::RectangleShape btnJ(sf::Vector2f(160.f, 50.f));
    btnJ.setFillColor(sf::Color(0, 160, 0));
    btnJ.setOutlineColor(sf::Color::White);
    btnJ.setOutlineThickness(2.f);
    btnJ.setPosition(220.f, 530.f);
    window.draw(btnJ);
    drawTextShadow("JUGAR", 22, sf::Color::White, 255.f, 540.f, sf::Text::Bold);

    sf::RectangleShape btnV(sf::Vector2f(160.f, 50.f));
    btnV.setFillColor(sf::Color(160, 0, 0));
    btnV.setOutlineColor(sf::Color::White);
    btnV.setOutlineThickness(2.f);
    btnV.setPosition(420.f, 530.f);
    window.draw(btnV);
    drawTextShadow("VOLVER", 22, sf::Color::White, 450.f, 540.f, sf::Text::Bold);

    window.display();
}

// ── drawStageSelect ───────────────────────────────────────────────────────────
void CharacterSelect::drawStageSelect()
{
    if (vsBgTexture.getSize().x > 0)
        window.draw(vsBgSprite);
    else
        window.clear(sf::Color(10, 10, 20));

    sf::Text title("Sorteo de Escenario", font, 32);
    drawTextShadow("Sorteo de Escenario", 32, sf::Color(255, 215, 0),
                   (800.f - title.getLocalBounds().width) / 2.f, 18.f, sf::Text::Bold);

    float elapsed = stageAnimClock.getElapsedTime().asSeconds();
    int displayIndex;

    if (!stageAnimDone)
    {
        float interval = 0.06f + (elapsed / 2.5f) * 0.24f;
        displayIndex   = static_cast<int>(elapsed / interval) % NUM_STAGES;

        if (elapsed >= 3.0f)
        {
            selectedStage = std::rand() % NUM_STAGES;
            stageAnimDone = true;
            displayIndex  = selectedStage;
        }
    }
    else
    {
        displayIndex = selectedStage;
    }

    const float THUMB_W     = 200.f;
    const float THUMB_H     = 120.f;
    const float THUMB_W_SEL = 260.f;
    const float THUMB_H_SEL = 156.f;
    const float TOTAL_Y     = 200.f;

    float totalWidth = THUMB_W * 2.f + THUMB_W_SEL + 20.f * 2.f;
    float startX     = (800.f - totalWidth) / 2.f;
    float curX = startX;

    for (int i = 0; i < NUM_STAGES; ++i)
    {
        bool isSelected = (i == displayIndex);
        float w = isSelected ? THUMB_W_SEL : THUMB_W;
        float h = isSelected ? THUMB_H_SEL : THUMB_H;
        float y = TOTAL_Y + (isSelected ? 0.f : (THUMB_H_SEL - THUMB_H) / 2.f);

        if (stageBgTextures[i].getSize().x > 0)
        {
            sf::Vector2u sz = stageBgTextures[i].getSize();
            stageBgSprites[i].setScale(w / sz.x, h / sz.y);
            stageBgSprites[i].setPosition(curX, y);
            window.draw(stageBgSprites[i]);
        }

        sf::RectangleShape border(sf::Vector2f(w, h));
        border.setPosition(curX, y);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(isSelected ? sf::Color(255, 215, 0) : sf::Color(100, 100, 140));
        border.setOutlineThickness(isSelected ? 4.f : 2.f);
        window.draw(border);

        curX += w + 20.f;
    }

    const std::string STAGE_NAMES[NUM_STAGES] = { "Campo Abierto", "Cristales Rojos", "Arena Gladiadores" };
    curX = startX;

    for (int i = 0; i < NUM_STAGES; ++i)
    {
        bool  isSelected = (i == displayIndex);
        float w          = isSelected ? THUMB_W_SEL : THUMB_W;
        float nameY      = TOTAL_Y + THUMB_H_SEL + 10.f;

        sf::Text nm(STAGE_NAMES[i], font, isSelected ? 18 : 14);
        float nmX = curX + (w - nm.getLocalBounds().width) / 2.f;

        drawTextShadow(STAGE_NAMES[i], isSelected ? 18 : 14,
                       isSelected ? sf::Color(255, 215, 0) : sf::Color(180, 180, 180),
                       nmX, nameY);
        curX += w + 20.f;
    }

    if (stageAnimDone)
    {
        std::string msg = "Escenario: " + STAGE_NAMES[selectedStage];
        sf::Text t(msg, font, 24);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow(msg, 24, sf::Color(100, 255, 100), tx, 420.f, sf::Text::Bold);

        sf::Text t2("Haz clic para comenzar la batalla!", font, 20);
        float tx2 = (800.f - t2.getLocalBounds().width) / 2.f;
        drawTextShadow("Haz clic para comenzar la batalla!", 20, sf::Color::White, tx2, 460.f);
    }
    else
    {
        sf::Text t("Sorteando escenario...", font, 20);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow("Sorteando escenario...", 20, sf::Color(200, 200, 255), tx, 440.f);
    }

    window.display();
}

// ── render ────────────────────────────────────────────────────────────────────
void CharacterSelect::render()
{
    static const bool draftIsJ1[6] = { true,  false, false, true,  true,  false };
    static const int  draftSlot[6] = { 0,     0,     1,     1,     2,     2     };

    switch (phase)
    {
        case Phase::PICKING:
            drawPickingScreen();
            break;
        case Phase::ORDER:
            if (orderTurn < 6)
                drawOrderScreen(draftIsJ1[orderTurn], draftSlot[orderTurn]);
            break;
        case Phase::CONFIRM:
            drawConfirmScreen();
            break;
        case Phase::STAGE_SELECT:
            drawStageSelect();
            break;
    }
}

// ── run ───────────────────────────────────────────────────────────────────────
void CharacterSelect::run()
{
    while (window.isOpen())
    {
        handleEvents();
        if (!window.isOpen()) break;
        update();
        render();
    }
}
