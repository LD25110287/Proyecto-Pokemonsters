#include "../include/CharacterSelect.h"
#include <iostream>
#include <algorithm>

// ── Constructor ───────────────────────────────────────────────────────────────
CharacterSelect::CharacterSelect()
    : window(sf::VideoMode(800, 600), "Pokemonsters - Seleccion de Personaje", sf::Style::Close)
    , phase(Phase::PICKING)
    , hoveredCard(-1)
    , pickTurn(0)
    , orderSlot(0)
    , selectedStage(-1)
    , stageAnimDone(false)
    , launchBattle(false)
{
    window.setFramerateLimit(60);
    p1Team.fill(-1);
    p2Team.fill(-1);

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

// ── Carga de assets ───────────────────────────────────────────────────────────
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
}

// ── Helper: texto con sombra ──────────────────────────────────────────────────
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

// ── Helper: dibujar portrait centrado en (x,y) con tamaño size ───────────────
void CharacterSelect::drawPortrait(int charIndex, float cx, float cy, float size,
                                   sf::Color borderColor, float borderThick)
{
    if (charIndex < 0 || charIndex >= (int)portraits.size()) return;
    if (portraits[charIndex].getSize().x == 0) return;

    auto sz = portraits[charIndex].getSize();
    float scaleX = size / sz.x;
    float scaleY = size / sz.y;
    portraitSprites[charIndex].setScale(scaleX, scaleY);
    portraitSprites[charIndex].setPosition(cx - size / 2.f, cy - size / 2.f);

    // Borde opcional
    if (borderThick > 0.f && borderColor != sf::Color::Transparent)
    {
        sf::RectangleShape border(sf::Vector2f(size + borderThick * 2, size + borderThick * 2));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(borderColor);
        border.setOutlineThickness(borderThick);
        border.setPosition(cx - size / 2.f - borderThick, cy - size / 2.f - borderThick);
        window.draw(border);
    }

    window.draw(portraitSprites[charIndex]);
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

        // Hover solo en fase PICKING
        if (event.type == sf::Event::MouseMoved && phase == Phase::PICKING)
            hoveredCard = getCardAt(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);

            // ── PICKING: J1 y J2 eligen alternando ───────────────────────────
            if (phase == Phase::PICKING)
            {
                int clicked = getCardAt(mp);
                if (clicked < 0) continue;

                bool isJ1Turn = (pickTurn % 2 == 0);

                if (isJ1Turn)
                    p1Picks.push_back(clicked);
                else
                    p2Picks.push_back(clicked);

                pickTurn++;

                // Cuando ambos tienen 3 picks, pasar a ordenar equipo J1
                if (pickTurn >= 6)
                {
                    phase       = Phase::ORDER_P1;
                    orderSlot   = 0;
                    orderSelected.clear();
                }
            }

            // ── ORDER_P1: J1 elige el orden de su equipo ─────────────────────
            else if (phase == Phase::ORDER_P1)
            {
                // Detectar click en una de las 3 miniaturas de J1
                for (int i = 0; i < 3; ++i)
                {
                    // Posiciones de las 3 miniaturas de J1 (izquierda)
                    float cx = 130.f + i * 160.f;
                    float cy = 380.f;
                    float half = 60.f;
                    sf::FloatRect rect(cx - half, cy - half, half * 2, half * 2);

                    if (rect.contains(static_cast<sf::Vector2f>(mp)))
                    {
                        // Solo si no fue ya seleccionado
                        if (std::find(orderSelected.begin(), orderSelected.end(), i)
                            == orderSelected.end())
                        {
                            p1Team[orderSlot] = p1Picks[i];
                            orderSelected.push_back(i);
                            orderSlot++;

                            if (orderSlot >= 3)
                            {
                                phase = Phase::ORDER_P2;
                                orderSlot = 0;
                                orderSelected.clear();
                            }
                        }
                        break;
                    }
                }
            }

            // ── ORDER_P2: J2 elige el orden de su equipo ─────────────────────
            else if (phase == Phase::ORDER_P2)
            {
                for (int i = 0; i < 3; ++i)
                {
                    float cx = 130.f + i * 160.f;
                    float cy = 380.f;
                    float half = 60.f;
                    sf::FloatRect rect(cx - half, cy - half, half * 2, half * 2);

                    if (rect.contains(static_cast<sf::Vector2f>(mp)))
                    {
                        if (std::find(orderSelected.begin(), orderSelected.end(), i)
                            == orderSelected.end())
                        {
                            p2Team[orderSlot] = p2Picks[i];
                            orderSelected.push_back(i);
                            orderSlot++;

                            if (orderSlot >= 3)
                                phase = Phase::CONFIRM;
                        }
                        break;
                    }
                }
            }

            // ── CONFIRM: botones JUGAR / VOLVER ──────────────────────────────
            else if (phase == Phase::CONFIRM)
            {
                sf::FloatRect btnJugar(250.f, 520.f, 130.f, 50.f);
                sf::FloatRect btnVolver(420.f, 520.f, 130.f, 50.f);

                if (btnJugar.contains(static_cast<sf::Vector2f>(mp)))
                {
                    // Ir a selección de escenario
                    phase = Phase::STAGE_SELECT;
                    selectedStage   = -1;
                    stageAnimDone   = false;
                    stageAnimClock.restart();
                }
                else if (btnVolver.contains(static_cast<sf::Vector2f>(mp)))
                {
                    // Reiniciar todo
                    pickTurn = 0;
                    p1Picks.clear();
                    p2Picks.clear();
                    p1Team.fill(-1);
                    p2Team.fill(-1);
                    orderSelected.clear();
                    orderSlot = 0;
                    phase = Phase::PICKING;
                    hoveredCard = -1;
                }
            }

            // ── STAGE_SELECT: elegir escenario manualmente ────────────────────
            else if (phase == Phase::STAGE_SELECT && stageAnimDone)
            {
                // Click en cualquier lugar lanza la batalla
                launchBattle = true;
                window.close();
            }
        }
    }
}

// ── update ────────────────────────────────────────────────────────────────────
void CharacterSelect::update()
{
    // Animación de ruleta de escenario (2 segundos de barajeo)
    if (phase == Phase::STAGE_SELECT && !stageAnimDone)
    {
        float elapsed = stageAnimClock.getElapsedTime().asSeconds();
        if (elapsed >= 2.0f)
        {
            // Elegir escenario aleatorio al terminar
            selectedStage = std::rand() % NUM_STAGES;
            stageAnimDone = true;
        }
        else
        {
            // Mostrar escenarios en secuencia rápida (efecto ruleta)
            int frame = static_cast<int>(elapsed / 0.15f) % NUM_STAGES;
            selectedStage = frame;
        }
    }
}

// ── drawPickingScreen ─────────────────────────────────────────────────────────
void CharacterSelect::drawPickingScreen()
{
    window.clear(sf::Color(18, 18, 32));

    bool isJ1Turn = (pickTurn % 2 == 0);

    // ── Títulos de turno ──────────────────────────────────────────────────────
    sf::Color j1Color(100, 180, 255);
    sf::Color j2Color(255, 120, 120);

    // J1 título (izquierda)
    drawTextShadow("Jugador 1 - Elige tu personaje", 18,
                   isJ1Turn ? j1Color : sf::Color(80, 80, 120),
                   10.f, 10.f,
                   isJ1Turn ? sf::Text::Bold : sf::Text::Regular);

    // J2 título (derecha)
    {
        sf::Text t("Jugador 2 - Elige tu personaje", font, 18);
        float tx = 800.f - t.getLocalBounds().width - 10.f;
        drawTextShadow("Jugador 2 - Elige tu personaje", 18,
                       !isJ1Turn ? j2Color : sf::Color(120, 80, 80),
                       tx, 10.f,
                       !isJ1Turn ? sf::Text::Bold : sf::Text::Regular);
    }

    // ── Miniaturas de picks ya realizados ────────────────────────────────────
    // J1 picks (arriba izquierda, pequeñitos)
    for (int i = 0; i < (int)p1Picks.size(); ++i)
        drawPortrait(p1Picks[i], 30.f + i * 50.f, 80.f, 40.f,
                     j1Color, 2.f);

    // J2 picks (arriba derecha)
    for (int i = 0; i < (int)p2Picks.size(); ++i)
        drawPortrait(p2Picks[i], 800.f - 30.f - i * 50.f, 80.f, 40.f,
                     j2Color, 2.f);

    // ── Indicador de turno ────────────────────────────────────────────────────
    std::string turnMsg = isJ1Turn
        ? ">>> Turno de Jugador 1 <<<"
        : ">>> Turno de Jugador 2 <<<";
    sf::Color turnColor = isJ1Turn ? j1Color : j2Color;
    {
        sf::Text t(turnMsg, font, 16);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow(turnMsg, 16, turnColor, tx, 118.f, sf::Text::Bold);
    }

    // ── Grid de personajes ────────────────────────────────────────────────────
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;

            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);

            // Color de la carta según estado
            sf::Color fillColor(40, 40, 60);
            sf::Color outColor(100, 100, 140);
            float outThick = 2.f;

            // ¿Ya fue elegido por J1?
            bool pickedByJ1 = std::find(p1Picks.begin(), p1Picks.end(), idx) != p1Picks.end();
            bool pickedByJ2 = std::find(p2Picks.begin(), p2Picks.end(), idx) != p2Picks.end();

            if (pickedByJ1 && pickedByJ2)
            { fillColor = sf::Color(60, 40, 80); outColor = sf::Color(180, 100, 255); outThick = 3.f; }
            else if (pickedByJ1)
            { fillColor = sf::Color(20, 50, 100); outColor = j1Color; outThick = 3.f; }
            else if (pickedByJ2)
            { fillColor = sf::Color(80, 20, 20); outColor = j2Color; outThick = 3.f; }
            else if (idx == hoveredCard)
            { fillColor = sf::Color(60, 60, 90); outColor = sf::Color(255, 215, 0); outThick = 3.f; }

            sf::RectangleShape card(sf::Vector2f(CARD_W, CARD_H));
            card.setFillColor(fillColor);
            card.setOutlineColor(outColor);
            card.setOutlineThickness(outThick);
            card.setPosition(x, y);
            window.draw(card);

            // Portrait centrado en la carta
            drawPortrait(idx, x + CARD_W / 2.f, y + CARD_H / 2.f - 15.f, CARD_W - 20.f);

            // Nombre
            sf::Text nm(characters[idx].name, font, 13);
            nm.setFillColor(sf::Color::White);
            nm.setPosition(x + (CARD_W - nm.getLocalBounds().width) / 2.f,
                           y + CARD_H - 28.f);
            window.draw(nm);

            // Badge de quién lo eligió
            if (pickedByJ1)
            {
                drawTextShadow("J1", 12, j1Color, x + 4.f, y + 4.f, sf::Text::Bold);
            }
            if (pickedByJ2)
            {
                sf::Text t2("J2", font, 12);
                drawTextShadow("J2", 12, j2Color,
                               x + CARD_W - t2.getLocalBounds().width - 8.f,
                               y + 4.f, sf::Text::Bold);
            }
        }

    window.display();
}

// ── drawOrderScreen ───────────────────────────────────────────────────────────
void CharacterSelect::drawOrderScreen(bool isP1)
{
    window.clear(sf::Color(18, 18, 32));

    sf::Color playerColor = isP1 ? sf::Color(100, 180, 255) : sf::Color(255, 120, 120);
    std::string playerName = isP1 ? "Jugador 1" : "Jugador 2";
    const std::vector<int>& picks = isP1 ? p1Picks : p2Picks;

    // Título
    std::string title = playerName + " - Elige el orden de tu equipo";
    {
        sf::Text t(title, font, 22);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow(title, 22, playerColor, tx, 20.f, sf::Text::Bold);
    }

    drawTextShadow("Haz clic en el orden que quieres que salgan a pelear", 16,
                   sf::Color(200, 200, 200), 80.f, 60.f);

    // Slots de orden (arriba, donde se van colocando)
    for (int i = 0; i < 3; ++i)
    {
        float cx = 200.f + i * 200.f;
        float cy = 200.f;

        // Número de slot
        std::string slotLabel = "Ronda " + std::to_string(i + 1);
        {
            sf::Text t(slotLabel, font, 16);
            drawTextShadow(slotLabel, 16, sf::Color(200, 200, 200),
                           cx - t.getLocalBounds().width / 2.f, cy - 90.f);
        }

        // ¿Ya se llenó este slot?
        bool filled = (isP1 ? p1Team[i] : p2Team[i]) >= 0;
        if (filled)
        {
            int charIdx = isP1 ? p1Team[i] : p2Team[i];
            drawPortrait(charIdx, cx, cy, 120.f, playerColor, 3.f);
            drawTextShadow(characters[charIdx].name, 14, sf::Color::White,
                           cx - 50.f, cy + 65.f);
        }
        else
        {
            // Slot vacío
            sf::RectangleShape slot(sf::Vector2f(120.f, 120.f));
            slot.setFillColor(sf::Color(30, 30, 50));
            slot.setOutlineColor(i == orderSlot
                ? sf::Color(255, 215, 0) : sf::Color(80, 80, 100));
            slot.setOutlineThickness(i == orderSlot ? 3.f : 2.f);
            slot.setPosition(cx - 60.f, cy - 60.f);
            window.draw(slot);

            if (i == orderSlot)
                drawTextShadow("?", 32, sf::Color(255, 215, 0), cx - 10.f, cy - 20.f);
        }
    }

    // Separador
    sf::RectangleShape sep(sf::Vector2f(700.f, 2.f));
    sep.setFillColor(sf::Color(80, 80, 120));
    sep.setPosition(50.f, 310.f);
    window.draw(sep);

    drawTextShadow("Tu equipo:", 16, sf::Color(180, 180, 255), 50.f, 320.f);

    // Miniaturas de los 3 picks (abajo, para hacer clic)
    for (int i = 0; i < 3; ++i)
    {
        float cx = 130.f + i * 160.f;
        float cy = 430.f;

        bool alreadyPlaced = std::find(orderSelected.begin(),
                                       orderSelected.end(), i) != orderSelected.end();

        sf::Color borderCol = alreadyPlaced
            ? sf::Color(60, 60, 60) : playerColor;
        float alpha = alreadyPlaced ? 0.4f : 1.f;

        drawPortrait(picks[i], cx, cy, 110.f, borderCol, 3.f);

        if (alreadyPlaced)
        {
            // Overlay oscuro encima
            sf::RectangleShape overlay(sf::Vector2f(110.f, 110.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            overlay.setPosition(cx - 55.f, cy - 55.f);
            window.draw(overlay);
            drawTextShadow("OK", 20, sf::Color(100, 255, 100),
                           cx - 15.f, cy - 12.f, sf::Text::Bold);
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
    // Fondo VS
    if (vsBgTexture.getSize().x > 0)
        window.draw(vsBgSprite);
    else
        window.clear(sf::Color(18, 18, 32));

    // Título
    {
        sf::Text t("Confirmacion", font, 34);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextShadow("Confirmacion", 34, sf::Color(255, 215, 0),
                       tx, 15.f, sf::Text::Bold);
    }

    // ── Equipo J1 (izquierda) ─────────────────────────────────────────────────
    drawTextShadow("Jugador 1", 20, sf::Color(100, 180, 255), 30.f, 65.f, sf::Text::Bold);
    for (int i = 0; i < 3; ++i)
    {
        float cx = 80.f + i * 105.f;
        float cy = 175.f;

        // Badge de ronda
        std::string badge = "R" + std::to_string(i + 1);
        drawTextShadow(badge, 13, sf::Color(255, 215, 0), cx - 8.f, cy - 75.f);

        drawPortrait(p1Team[i], cx, cy, 110.f,
                     i == 0 ? sf::Color(255, 215, 0) : sf::Color(100, 180, 255),
                     i == 0 ? 4.f : 2.f);

        drawTextShadow(characters[p1Team[i]].name, 12, sf::Color::White,
                       cx - 45.f, cy + 60.f);
    }

    // ── Equipo J2 (derecha) ───────────────────────────────────────────────────
    drawTextShadow("Jugador 2", 20, sf::Color(255, 120, 120), 490.f, 65.f, sf::Text::Bold);
    for (int i = 0; i < 3; ++i)
    {
        float cx = 540.f + i * 105.f;
        float cy = 175.f;

        std::string badge = "R" + std::to_string(i + 1);
        drawTextShadow(badge, 13, sf::Color(255, 215, 0), cx - 8.f, cy - 75.f);

        drawPortrait(p2Team[i], cx, cy, 110.f,
                     i == 0 ? sf::Color(255, 215, 0) : sf::Color(255, 120, 120),
                     i == 0 ? 4.f : 2.f);

        drawTextShadow(characters[p2Team[i]].name, 12, sf::Color::White,
                       cx - 45.f, cy + 60.f);
    }

    // VS central
    drawTextShadow("VS", 48, sf::Color::White, 372.f, 150.f, sf::Text::Bold);

    // Separador
    sf::RectangleShape sep(sf::Vector2f(700.f, 2.f));
    sep.setFillColor(sf::Color(255, 255, 255, 80));
    sep.setPosition(50.f, 380.f);
    window.draw(sep);

    // Resumen de rondas
    drawTextShadow("Orden de batalla:", 16, sf::Color(220, 220, 220), 50.f, 395.f);
    for (int i = 0; i < 3; ++i)
    {
        std::string line = "Ronda " + std::to_string(i+1) + ":  "
                         + characters[p1Team[i]].name
                         + "  vs  "
                         + characters[p2Team[i]].name;
        drawTextShadow(line, 14, sf::Color(200, 200, 200), 60.f, 420.f + i * 22.f);
    }

    // Botones
    sf::RectangleShape btnJ(sf::Vector2f(130.f, 45.f));
    btnJ.setFillColor(sf::Color(0, 160, 0));
    btnJ.setOutlineColor(sf::Color::White);
    btnJ.setOutlineThickness(2.f);
    btnJ.setPosition(250.f, 525.f);
    window.draw(btnJ);
    drawTextShadow("JUGAR", 20, sf::Color::White, 281.f, 534.f, sf::Text::Bold);

    sf::RectangleShape btnV(sf::Vector2f(130.f, 45.f));
    btnV.setFillColor(sf::Color(160, 0, 0));
    btnV.setOutlineColor(sf::Color::White);
    btnV.setOutlineThickness(2.f);
    btnV.setPosition(420.f, 525.f);
    window.draw(btnV);
    drawTextShadow("VOLVER", 20, sf::Color::White, 446.f, 534.f, sf::Text::Bold);

    window.display();
}

// ── drawStageSelect ───────────────────────────────────────────────────────────
void CharacterSelect::drawStageSelect()
{
    if (selectedStage >= 0 && selectedStage < NUM_STAGES)
        window.draw(stageBgSprites[selectedStage]);
    else
        window.clear(sf::Color(18, 18, 32));

    if (!stageAnimDone)
    {
        drawTextShadow("Sorteando escenario...", 28, sf::Color(255, 215, 0),
                       200.f, 260.f, sf::Text::Bold);
    }
    else
    {
        const std::string stageNames[NUM_STAGES] = {
            "Campo Abierto", "Cristales Rojos", "Arena Gladiadores"
        };
        drawTextShadow("Escenario: " + stageNames[selectedStage], 28,
                       sf::Color(255, 215, 0), 160.f, 240.f, sf::Text::Bold);
        drawTextShadow("Haz clic para comenzar la batalla!", 20,
                       sf::Color::White, 190.f, 290.f);
    }

    window.display();
}

// ── render ────────────────────────────────────────────────────────────────────
void CharacterSelect::render()
{
    switch (phase)
    {
        case Phase::PICKING:    drawPickingScreen();      break;
        case Phase::ORDER_P1:   drawOrderScreen(true);    break;
        case Phase::ORDER_P2:   drawOrderScreen(false);   break;
        case Phase::CONFIRM:    drawConfirmScreen();       break;
        case Phase::STAGE_SELECT: drawStageSelect();      break;
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
