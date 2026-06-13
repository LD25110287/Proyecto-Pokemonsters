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

    // ── Datos de los 6 personajes en orden 1-6 ───────────────────────────
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
    // Fuente
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (const auto& p : fontPaths)
        if (font.loadFromFile(p)) break;

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
            // Escalar a 100x100 si no lo son
            sf::Vector2u sz = portraits[i].getSize();
            portraitSprites[i].setScale(100.f / sz.x, 100.f / sz.y);
        }
    }
}

void CharacterSelect::buildGrid()
{
    // Las posiciones se calculan en getCardAt() y drawGrid() de forma consistente
}

int CharacterSelect::getCardAt(sf::Vector2i mousePos) const
{
    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;

            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);
            sf::FloatRect rect(x, y, CARD_W, CARD_H);
            if (rect.contains(static_cast<sf::Vector2f>(mousePos)))
                return idx;
        }
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
        {
            if (phase != Phase::CONFIRM)
                hoveredCard = getCardAt(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);
            int clicked = getCardAt(mp);

            if (phase == Phase::PLAYER1 && clicked >= 0)
            {
                p1Choice = clicked;
                phase    = Phase::PLAYER2;
                hoveredCard = -1;
            }
            else if (phase == Phase::PLAYER2 && clicked >= 0)
            {
                p2Choice = clicked;
                phase    = Phase::CONFIRM;
            }
            else if (phase == Phase::CONFIRM)
            {
                // Botón JUGAR
                sf::FloatRect btnJugar(250.f, 480.f, 130.f, 50.f);
                sf::FloatRect btnVolver(420.f, 480.f, 130.f, 50.f);

                if (btnJugar.contains(static_cast<sf::Vector2f>(mp)))
                {
                    launchBattle = true;
                    window.close();
                }
                else if (btnVolver.contains(static_cast<sf::Vector2f>(mp)))
                {
                    // Volver a selección J1
                    p1Choice    = -1;
                    p2Choice    = -1;
                    phase       = Phase::PLAYER1;
                    hoveredCard = -1;
                }
            }
        }
    }
}

void CharacterSelect::drawGrid(int highlightIndex, int lockedIndex, const std::string& label)
{
    window.clear(sf::Color(18, 18, 32));

    // Título
    sf::Text title(label, font, 30);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setStyle(sf::Text::Bold);
    float titleX = (800.f - title.getLocalBounds().width) / 2.f;
    title.setPosition(titleX, 20.f);
    window.draw(title);

    // Subtítulo con personaje ya elegido (fase J2)
    if (lockedIndex >= 0)
    {
        sf::Text sub("Jugador 1 eligio: " + characters[lockedIndex].name, font, 18);
        sub.setFillColor(sf::Color(180, 180, 255));
        sub.setPosition(20.f, 70.f);
        window.draw(sub);
    }

    // Grid de personajes
    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            int idx = row * COLS + col;
            if (idx >= (int)characters.size()) continue;

            float x = GRID_X + col * (CARD_W + PADDING);
            float y = GRID_Y + row * (CARD_H + PADDING);

            // Fondo de la card
            sf::RectangleShape card(sf::Vector2f(CARD_W, CARD_H));
            card.setPosition(x, y);

            if (idx == lockedIndex)
            {
                card.setFillColor(sf::Color(0, 80, 160));
                card.setOutlineColor(sf::Color(100, 180, 255));
                card.setOutlineThickness(3.f);
            }
            else if (idx == highlightIndex)
            {
                card.setFillColor(sf::Color(60, 60, 90));
                card.setOutlineColor(sf::Color(255, 215, 0));
                card.setOutlineThickness(3.f);
            }
            else
            {
                card.setFillColor(sf::Color(40, 40, 60));
                card.setOutlineColor(sf::Color(100, 100, 140));
                card.setOutlineThickness(2.f);
            }
            window.draw(card);

            // Portrait centrado en la card
            portraitSprites[idx].setPosition(x + 30.f, y + 10.f);
            window.draw(portraitSprites[idx]);

            // Nombre del personaje
            sf::Text nameText(characters[idx].name, font, 13);
            nameText.setFillColor(sf::Color::White);
            float nameX = x + (CARD_W - nameText.getLocalBounds().width) / 2.f - 4.f;
            nameText.setPosition(nameX, y + CARD_H - 30.f);
            window.draw(nameText);
        }
    }

    window.display();
}

void CharacterSelect::drawConfirmScreen()
{
    window.clear(sf::Color(18, 18, 32));

    sf::Text title("Confirmacion", font, 34);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setStyle(sf::Text::Bold);
    title.setPosition((800.f - title.getLocalBounds().width) / 2.f, 20.f);
    window.draw(title);

    // Panel J1
    {
        sf::RectangleShape panel(sf::Vector2f(300.f, 350.f));
        panel.setFillColor(sf::Color(30, 30, 60));
        panel.setOutlineColor(sf::Color(100, 180, 255));
        panel.setOutlineThickness(3.f);
        panel.setPosition(60.f, 80.f);
        window.draw(panel);

        sf::Text lbl("Jugador 1", font, 22);
        lbl.setFillColor(sf::Color(100, 180, 255));
        lbl.setPosition(60.f + (300.f - lbl.getLocalBounds().width) / 2.f, 90.f);
        window.draw(lbl);

        portraitSprites[p1Choice].setScale(2.f, 2.f);
        portraitSprites[p1Choice].setPosition(160.f, 140.f);
        window.draw(portraitSprites[p1Choice]);
        portraitSprites[p1Choice].setScale(1.f, 1.f); // reset

        sf::Text nm(characters[p1Choice].name, font, 18);
        nm.setFillColor(sf::Color::White);
        nm.setPosition(60.f + (300.f - nm.getLocalBounds().width) / 2.f, 360.f);
        window.draw(nm);
    }

    // Panel J2
    {
        sf::RectangleShape panel(sf::Vector2f(300.f, 350.f));
        panel.setFillColor(sf::Color(60, 30, 30));
        panel.setOutlineColor(sf::Color(255, 100, 100));
        panel.setOutlineThickness(3.f);
        panel.setPosition(440.f, 80.f);
        window.draw(panel);

        sf::Text lbl("Jugador 2", font, 22);
        lbl.setFillColor(sf::Color(255, 100, 100));
        lbl.setPosition(440.f + (300.f - lbl.getLocalBounds().width) / 2.f, 90.f);
        window.draw(lbl);

        portraitSprites[p2Choice].setScale(2.f, 2.f);
        portraitSprites[p2Choice].setPosition(540.f, 140.f);
        window.draw(portraitSprites[p2Choice]);
        portraitSprites[p2Choice].setScale(1.f, 1.f);

        sf::Text nm(characters[p2Choice].name, font, 18);
        nm.setFillColor(sf::Color::White);
        nm.setPosition(440.f + (300.f - nm.getLocalBounds().width) / 2.f, 360.f);
        window.draw(nm);
    }

    // Botón JUGAR
    sf::RectangleShape btnJ(sf::Vector2f(130.f, 50.f));
    btnJ.setFillColor(sf::Color(0, 160, 0));
    btnJ.setOutlineColor(sf::Color::White);
    btnJ.setOutlineThickness(2.f);
    btnJ.setPosition(250.f, 480.f);
    window.draw(btnJ);
    sf::Text txtJ("JUGAR", font, 22);
    txtJ.setFillColor(sf::Color::White);
    txtJ.setStyle(sf::Text::Bold);
    txtJ.setPosition(250.f + (130.f - txtJ.getLocalBounds().width) / 2.f, 490.f);
    window.draw(txtJ);

    // Botón VOLVER
    sf::RectangleShape btnV(sf::Vector2f(130.f, 50.f));
    btnV.setFillColor(sf::Color(160, 0, 0));
    btnV.setOutlineColor(sf::Color::White);
    btnV.setOutlineThickness(2.f);
    btnV.setPosition(420.f, 480.f);
    window.draw(btnV);
    sf::Text txtV("VOLVER", font, 22);
    txtV.setFillColor(sf::Color::White);
    txtV.setStyle(sf::Text::Bold);
    txtV.setPosition(420.f + (130.f - txtV.getLocalBounds().width) / 2.f, 490.f);
    window.draw(txtV);

    window.display();
}

void CharacterSelect::update()  {}

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
