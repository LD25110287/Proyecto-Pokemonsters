#include "../include/CharacterSelect.h"
#include <iostream>

CharacterSelect::CharacterSelect()
    : window(sf::VideoMode(800, 600), "Pokemonsters - Seleccion de Personaje", sf::Style::Close)
    , phase(Phase::PLAYER1)
    , hoveredCard(-1)
    , p1Choice(-1)
    , p2Choice(-1)
    , launchBattle(false)
{
    window.setFramerateLimit(60);

    characters = {
        { "Exdarktyranomon", "assets/images/1.png", "assets/images/champ_select_1.png",  50,  50, 100, 22, 8  },
        { "BeelStarmon",     "assets/images/2.png", "assets/images/champ_select_2.png", 100, 100,  95, 20, 6  },
        { "Bioquetzalmon",   "assets/images/3.png", "assets/images/champ_select_3.png", 100, 100,  90, 18, 10 },
        { "Jesmon",          "assets/images/4.png", "assets/images/champ_select_4.png", 100, 100, 105, 25, 7  },
        { "Sleipmon",        "assets/images/5.png", "assets/images/champ_select_5.png", 150,  80, 110, 19, 12 },
        { "Magnamon",        "assets/images/6.png", "assets/images/champ_select_6.png", 100, 100,  98, 23, 9  }
    };

    loadAssets();
    buildGrid();
}

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
        std::cerr << "No se pudo cargar assets/images/imagen vs.png\n";
    else
    {
        vsBgSprite.setTexture(vsBgTexture);
        sf::Vector2u sz = vsBgTexture.getSize();
        vsBgSprite.setScale(800.f / sz.x, 600.f / sz.y);
    }

    // Portraits
    portraits.resize(characters.size());
    portraitSprites.resize(characters.size());
    for (size_t i = 0; i < characters.size(); ++i)
    {
        if (!portraits[i].loadFromFile(characters[i].portraitPath))
            std::cerr << "No se pudo cargar portrait: " << characters[i].portraitPath << "\n";
        else
        {
            portraitSprites[i].setTexture(portraits[i]);
            sf::Vector2u sz = portraits[i].getSize();
            portraitSprites[i].setScale(100.f / sz.x, 100.f / sz.y);
        }
    }
}

void CharacterSelect::buildGrid() {}

int CharacterSelect::getCardAt(sf::Vector2i mousePos) const
{
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;
            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);
            if (sf::FloatRect(x, y, CARD_W, CARD_H).contains(static_cast<sf::Vector2f>(mousePos)))
                return idx;
        }
    return -1;
}

void CharacterSelect::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::MouseMoved)
            if (phase != Phase::CONFIRM)
                hoveredCard = getCardAt(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);
            int clicked = getCardAt(mp);

            if (phase == Phase::PLAYER1 && clicked >= 0)
            { p1Choice = clicked; phase = Phase::PLAYER2; hoveredCard = -1; }

            else if (phase == Phase::PLAYER2 && clicked >= 0)
            { p2Choice = clicked; phase = Phase::CONFIRM; }

            else if (phase == Phase::CONFIRM)
            {
                if (sf::FloatRect(250.f, 510.f, 130.f, 50.f).contains(static_cast<sf::Vector2f>(mp)))
                { launchBattle = true; window.close(); }

                else if (sf::FloatRect(420.f, 510.f, 130.f, 50.f).contains(static_cast<sf::Vector2f>(mp)))
                { p1Choice = -1; p2Choice = -1; phase = Phase::PLAYER1; hoveredCard = -1; }
            }
        }
    }
}

void CharacterSelect::drawGrid(int highlightIndex, int lockedIndex, const std::string& label)
{
    window.clear(sf::Color(18, 18, 32));

    sf::Text title(label, font, 30);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setStyle(sf::Text::Bold);
    title.setPosition((800.f - title.getLocalBounds().width) / 2.f, 20.f);
    window.draw(title);

    if (lockedIndex >= 0)
    {
        sf::Text sub("Jugador 1 eligio: " + characters[lockedIndex].name, font, 18);
        sub.setFillColor(sf::Color(180, 180, 255));
        sub.setPosition(20.f, 70.f);
        window.draw(sub);
    }

    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;

            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);

            sf::RectangleShape card(sf::Vector2f(CARD_W, CARD_H));
            card.setPosition(x, y);

            if (idx == lockedIndex)
            { card.setFillColor(sf::Color(0,80,160)); card.setOutlineColor(sf::Color(100,180,255)); card.setOutlineThickness(3.f); }
            else if (idx == highlightIndex)
            { card.setFillColor(sf::Color(60,60,90)); card.setOutlineColor(sf::Color(255,215,0)); card.setOutlineThickness(3.f); }
            else
            { card.setFillColor(sf::Color(40,40,60)); card.setOutlineColor(sf::Color(100,100,140)); card.setOutlineThickness(2.f); }
            window.draw(card);

            portraitSprites[idx].setPosition(x + 30.f, y + 10.f);
            window.draw(portraitSprites[idx]);

            sf::Text nameText(characters[idx].name, font, 13);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + (CARD_W - nameText.getLocalBounds().width) / 2.f - 4.f, y + CARD_H - 30.f);
            window.draw(nameText);
        }

    window.display();
}

// Dibuja texto con sombra para que sea legible sobre cualquier fondo
void drawTextWithShadow(sf::RenderWindow& win, const std::string& str,
                        const sf::Font& font, unsigned int size,
                        sf::Color color, float x, float y,
                        sf::Text::Style style = sf::Text::Regular)
{
    sf::Text shadow(str, font, size);
    shadow.setFillColor(sf::Color(0, 0, 0, 200));
    shadow.setStyle(style);
    shadow.setPosition(x + 2.f, y + 2.f);
    win.draw(shadow);

    sf::Text text(str, font, size);
    text.setFillColor(color);
    text.setStyle(style);
    text.setPosition(x, y);
    win.draw(text);
}

void CharacterSelect::drawConfirmScreen()
{
    // Fondo VS ocupa toda la pantalla, sin recuadros
    if (vsBgTexture.getSize().x > 0)
        window.draw(vsBgSprite);
    else
        window.clear(sf::Color(18, 18, 32));

    // ── Título ────────────────────────────────────────────────────────────────
    {
        sf::Text t("Confirmacion", font, 36);
        t.setStyle(sf::Text::Bold);
        float tx = (800.f - t.getLocalBounds().width) / 2.f;
        drawTextWithShadow(window, "Confirmacion", font, 36,
                           sf::Color(255, 215, 0), tx, 15.f, sf::Text::Bold);
    }

    // ── Tamaño del portrait en confirmación ───────────────────────────────────
    const float PORTRAIT_SIZE = 220.f;

    // ── Jugador 1 (izquierda) ─────────────────────────────────────────────────
    {
        // Portrait centrado en el lado izquierdo
        // Zona izquierda: x = 0..380, centro = 190
        sf::Vector2u sz = portraits[p1Choice].getSize();
        float scaleX = PORTRAIT_SIZE / sz.x;
        float scaleY = PORTRAIT_SIZE / sz.y;
        portraitSprites[p1Choice].setScale(scaleX, scaleY);

        float portraitX = 190.f - PORTRAIT_SIZE / 2.f;  // centrado en zona izquierda
        float portraitY = 130.f;
        portraitSprites[p1Choice].setPosition(portraitX, portraitY);
        window.draw(portraitSprites[p1Choice]);

        // Label "Jugador 1" encima del portrait
        sf::Text lbl("Jugador 1", font, 24);
        float lblX = 190.f - lbl.getLocalBounds().width / 2.f;
        drawTextWithShadow(window, "Jugador 1", font, 24,
                           sf::Color(100, 200, 255), lblX, 90.f, sf::Text::Bold);

        // Nombre del personaje debajo del portrait
        sf::Text nm(characters[p1Choice].name, font, 20);
        float nmX = 190.f - nm.getLocalBounds().width / 2.f;
        drawTextWithShadow(window, characters[p1Choice].name, font, 20,
                           sf::Color::White, nmX, portraitY + PORTRAIT_SIZE + 8.f);

        // Reset escala para la grilla
        portraitSprites[p1Choice].setScale(100.f / sz.x, 100.f / sz.y);
    }

    // ── Jugador 2 (derecha) ───────────────────────────────────────────────────
    {
        // Zona derecha: x = 420..800, centro = 610
        sf::Vector2u sz = portraits[p2Choice].getSize();
        float scaleX = PORTRAIT_SIZE / sz.x;
        float scaleY = PORTRAIT_SIZE / sz.y;
        portraitSprites[p2Choice].setScale(scaleX, scaleY);

        float portraitX = 610.f - PORTRAIT_SIZE / 2.f;
        float portraitY = 130.f;
        portraitSprites[p2Choice].setPosition(portraitX, portraitY);
        window.draw(portraitSprites[p2Choice]);

        sf::Text lbl("Jugador 2", font, 24);
        float lblX = 610.f - lbl.getLocalBounds().width / 2.f;
        drawTextWithShadow(window, "Jugador 2", font, 24,
                           sf::Color(255, 120, 120), lblX, 90.f, sf::Text::Bold);

        sf::Text nm(characters[p2Choice].name, font, 20);
        float nmX = 610.f - nm.getLocalBounds().width / 2.f;
        drawTextWithShadow(window, characters[p2Choice].name, font, 20,
                           sf::Color::White, nmX, portraitY + PORTRAIT_SIZE + 8.f);

        portraitSprites[p2Choice].setScale(100.f / sz.x, 100.f / sz.y);
    }

    // ── Botones JUGAR / VOLVER ────────────────────────────────────────────────
    sf::RectangleShape btnJ(sf::Vector2f(130.f, 50.f));
    btnJ.setFillColor(sf::Color(0, 160, 0));
    btnJ.setOutlineColor(sf::Color::White);
    btnJ.setOutlineThickness(2.f);
    btnJ.setPosition(250.f, 510.f);
    window.draw(btnJ);

    sf::Text txtJ("JUGAR", font, 22);
    txtJ.setFillColor(sf::Color::White);
    txtJ.setStyle(sf::Text::Bold);
    txtJ.setPosition(250.f + (130.f - txtJ.getLocalBounds().width) / 2.f, 520.f);
    window.draw(txtJ);

    sf::RectangleShape btnV(sf::Vector2f(130.f, 50.f));
    btnV.setFillColor(sf::Color(160, 0, 0));
    btnV.setOutlineColor(sf::Color::White);
    btnV.setOutlineThickness(2.f);
    btnV.setPosition(420.f, 510.f);
    window.draw(btnV);

    sf::Text txtV("VOLVER", font, 22);
    txtV.setFillColor(sf::Color::White);
    txtV.setStyle(sf::Text::Bold);
    txtV.setPosition(420.f + (130.f - txtV.getLocalBounds().width) / 2.f, 520.f);
    window.draw(txtV);

    window.display();
}

void CharacterSelect::update() {}

void CharacterSelect::render()
{
    if (phase == Phase::PLAYER1)
        drawGrid(hoveredCard, -1, "Jugador 1 - Elige tu personaje");
    else if (phase == Phase::PLAYER2)
        drawGrid(hoveredCard, p1Choice, "Jugador 2 - Elige tu personaje");
    else
        drawConfirmScreen();
}

void CharacterSelect::run()
{
    while (window.isOpen())
    {
        handleEvents();
        if (!window.isOpen()) break;
        render();
    }
}
