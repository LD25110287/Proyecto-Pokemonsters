#include "../include/MenuScreen.h"
#include <iostream>

// Sin #include "Game.h" — dependencia circular eliminada

MenuScreen::MenuScreen()
    : window(sf::VideoMode(800, 600), "Pokemonsters", sf::Style::Close),
      state(MenuState::MAIN_MENU),
      hoveredButton(-1),
      launchGame(false),
      detailIndex(-1)
{
    window.setFramerateLimit(60);
    loadAssets();
    loadCharacterPortraits();
    createMenuButtons();

    titleText.setFont(font);
    titleText.setString("Pokemonsters");
    titleText.setCharacterSize(56);
    titleText.setFillColor(sf::Color(255, 215, 0));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(70.f, 40.f);

    backButton.setSize(sf::Vector2f(180.f, 45.f));
    backButton.setFillColor(sf::Color(180, 0, 0));
    backButton.setOutlineColor(sf::Color::White);
    backButton.setOutlineThickness(2.f);
    backButton.setPosition(20.f, 520.f);

    backText.setFont(font);
    backText.setString("Volver");
    backText.setCharacterSize(22);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(42.f, 530.f);
}

// ── Assets ────────────────────────────────────────────────────────────────────
void MenuScreen::loadAssets()
{
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/pokemon.ttf",
        "assets/fonts/font.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/verdana.ttf"
    };
    for (const auto& p : fontPaths)
        if (font.loadFromFile(p)) { std::cout << "Fuente: " << p << "\n"; break; }
}

void MenuScreen::loadCharacterPortraits()
{
    // Datos de los 6 personajes
    struct Info { std::string name, desc, portrait; };
    std::vector<Info> data = {
        { "Exdarktyranomon",
          "Digimon no oficial, producto de confundir al letal\n"
          "DarkTyrannomon con el tetrico peluche ExTyrannomon.",
          "assets/images/champ_select_1.png" },
        { "BeelStarmon",
          "Digimon Senor Demonio conocida como la\n"
          "'Beelzebumon Lady'; maneja expertamente\n"
          "dos pistolas gemelas.",
          "assets/images/champ_select_2.png" },
        { "Bioquetzalmon",
          "Forma hibrida creada cuando un humano se\n"
          "fusiona con la data de un Quetzalmon.",
          "assets/images/champ_select_3.png" },
        { "Jesmon",
          "Caballero Real con espadas en sus extremidades\n"
          "y tres espiritus familiares.\n"
          "Rescata y protege Digimon.",
          "assets/images/champ_select_4.png" },
        { "Sleipmon",
          "Caballero Real sagrado con seis patas y\n"
          "armadura roja, que protege las ruinas\n"
          "del Polo Norte.",
          "assets/images/champ_select_5.png" },
        { "Magnamon",
          "Caballero Real blindado con el Digimental\n"
          "de los Milagros; posee una defensa\n"
          "dorada impenetrable.",
          "assets/images/champ_select_6.png" }
    };

    chars.resize(data.size());
    for (size_t i = 0; i < data.size(); ++i)
    {
        chars[i].name        = data[i].name;
        chars[i].description = data[i].desc;
        chars[i].portraitPath = data[i].portrait;

        if (chars[i].texture.loadFromFile(data[i].portrait))
        {
            chars[i].sprite.setTexture(chars[i].texture);
            chars[i].loaded = true;
        }
        else
            std::cerr << "No se cargo portrait: " << data[i].portrait << "\n";
    }

    // Cards de la pantalla Characters (3x2)
    charCards.resize(6);
    charCardTexts.resize(6);
    const float cw = 200.f, ch = 60.f, sx = 60.f, sy = 160.f, gap = 20.f;
    for (int i = 0; i < 6; ++i)
    {
        int col = i % 3, row = i / 3;
        float x = sx + col * (cw + gap);
        float y = sy + row * (ch + gap);

        charCards[i].setSize(sf::Vector2f(cw, ch));
        charCards[i].setFillColor(sf::Color(40, 40, 70));
        charCards[i].setOutlineColor(sf::Color(100, 100, 180));
        charCards[i].setOutlineThickness(2.f);
        charCards[i].setPosition(x, y);

        charCardTexts[i].setFont(font);
        charCardTexts[i].setString(chars[i].name);
        charCardTexts[i].setCharacterSize(16);
        charCardTexts[i].setFillColor(sf::Color::White);
        float tx = x + (cw - charCardTexts[i].getLocalBounds().width) / 2.f - 4.f;
        float ty = y + (ch - charCardTexts[i].getLocalBounds().height) / 2.f - 4.f;
        charCardTexts[i].setPosition(tx, ty);
    }
}

// ── Botones del menú principal ────────────────────────────────────────────────
void MenuScreen::createMenuButtons()
{
    const float bw = 360.f, bh = 60.f;
    const float sx = (window.getSize().x - bw) / 2.f;
    const float sy = 200.f, sp = 80.f;
    std::vector<std::string> labels = {"Jugar", "Personajes", "Configuracion", "Salir"};

    buttons.resize(labels.size());
    buttonTexts.resize(labels.size());
    for (size_t i = 0; i < labels.size(); ++i)
    {
        buttons[i].setSize(sf::Vector2f(bw, bh));
        buttons[i].setFillColor(sf::Color(200, 0, 0));
        buttons[i].setOutlineColor(sf::Color::White);
        buttons[i].setOutlineThickness(3.f);
        buttons[i].setPosition(sx, sy + i * sp);

        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(labels[i]);
        buttonTexts[i].setCharacterSize(28);
        buttonTexts[i].setFillColor(sf::Color::White);
        buttonTexts[i].setStyle(sf::Text::Bold);
        float tx = sx + (bw - buttonTexts[i].getLocalBounds().width) / 2.f - 8.f;
        float ty = sy + i * sp + (bh - buttonTexts[i].getLocalBounds().height) / 2.f - 8.f;
        buttonTexts[i].setPosition(tx, ty);
    }
}

// ── Hover y click ─────────────────────────────────────────────────────────────
void MenuScreen::updateButtonHover(const sf::Vector2i& mp)
{
    hoveredButton = -1;
    for (size_t i = 0; i < buttons.size(); ++i)
        if (buttons[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mp)))
        { hoveredButton = static_cast<int>(i); break; }
}

int MenuScreen::getButtonAtPosition(const sf::Vector2i& mp) const
{
    for (size_t i = 0; i < buttons.size(); ++i)
        if (buttons[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mp)))
            return static_cast<int>(i);
    return -1;
}

void MenuScreen::handleMainMenuClick(int index)
{
    switch (index)
    {
        case 0: launchGame = true;  window.close(); break;
        case 1: state = MenuState::CHARACTERS;       break;
        case 2: state = MenuState::SETTINGS;         break;
        case 3: launchGame = false; window.close();  break;
    }
}

void MenuScreen::handleSubmenuEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mp(event.mouseButton.x, event.mouseButton.y);

        // Botón volver
        if (backButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mp)))
        {
            if (state == MenuState::CHARACTER_DETAIL)
                state = MenuState::CHARACTERS;
            else
                state = MenuState::MAIN_MENU;
            return;
        }

        // Click en una card de personaje
        if (state == MenuState::CHARACTERS)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (charCards[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mp)))
                {
                    detailIndex = i;
                    state = MenuState::CHARACTER_DETAIL;
                    return;
                }
            }
        }
    }
}

// ── Draw: Menú principal ──────────────────────────────────────────────────────
void MenuScreen::drawMainMenu()
{
    window.clear(sf::Color(18, 18, 32));
    window.draw(titleText);

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if (static_cast<int>(i) == hoveredButton)
        {
            buttons[i].setFillColor(sf::Color(255, 100, 0));
            buttons[i].setOutlineColor(sf::Color(255, 215, 0));
        }
        else
        {
            buttons[i].setFillColor(sf::Color(200, 0, 0));
            buttons[i].setOutlineColor(sf::Color::White);
        }
        window.draw(buttons[i]);
        window.draw(buttonTexts[i]);
    }
    window.display();
}

// ── Draw: Lista de personajes ─────────────────────────────────────────────────
void MenuScreen::drawCharactersScreen()
{
    window.clear(sf::Color(15, 20, 40));

    sf::Text header("Personajes", font, 42);
    header.setFillColor(sf::Color(255, 215, 0));
    header.setStyle(sf::Text::Bold);
    header.setPosition(40.f, 40.f);
    window.draw(header);

    sf::Text hint("Haz click en un personaje para ver su informacion", font, 16);
    hint.setFillColor(sf::Color(180, 180, 180));
    hint.setPosition(40.f, 110.f);
    window.draw(hint);

    // Cards 3x2
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    for (int i = 0; i < 6; ++i)
    {
        bool hovered = charCards[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mp));
        charCards[i].setFillColor(hovered ? sf::Color(70, 70, 120) : sf::Color(40, 40, 70));
        charCards[i].setOutlineColor(hovered ? sf::Color(255, 215, 0) : sf::Color(100, 100, 180));
        window.draw(charCards[i]);
        window.draw(charCardTexts[i]);
    }

    drawBackButton();
    window.display();
}

// ── Draw: Detalle de un personaje ─────────────────────────────────────────────
void MenuScreen::drawCharacterDetail(int idx)
{
    if (idx < 0 || idx >= (int)chars.size()) return;
    const auto& ch = chars[idx];

    window.clear(sf::Color(15, 20, 40));

    // ── Panel izquierdo: portrait grande ─────────────────────────────────
    sf::RectangleShape portraitPanel(sf::Vector2f(250.f, 300.f));
    portraitPanel.setFillColor(sf::Color(25, 30, 55));
    portraitPanel.setOutlineColor(sf::Color(255, 215, 0));
    portraitPanel.setOutlineThickness(3.f);
    portraitPanel.setPosition(40.f, 100.f);
    window.draw(portraitPanel);

    if (ch.loaded)
    {
        // Escalar portrait para llenar el panel (210x210 centrado)
        sf::Sprite big = ch.sprite;
        sf::Vector2u sz = ch.texture.getSize();
        float scale = 210.f / std::max(sz.x, sz.y);
        big.setScale(scale, scale);
        float px = 40.f + (250.f - sz.x * scale) / 2.f;
        float py = 100.f + (300.f - sz.y * scale) / 2.f;
        big.setPosition(px, py);
        window.draw(big);
    }

    // ── Panel derecho: nombre + descripción ──────────────────────────────
    sf::RectangleShape infoPanel(sf::Vector2f(440.f, 300.f));
    infoPanel.setFillColor(sf::Color(25, 30, 55));
    infoPanel.setOutlineColor(sf::Color(100, 100, 200));
    infoPanel.setOutlineThickness(2.f);
    infoPanel.setPosition(320.f, 100.f);
    window.draw(infoPanel);

    // Nombre
    sf::Text nameText(ch.name, font, 28);
    nameText.setFillColor(sf::Color(255, 215, 0));
    nameText.setStyle(sf::Text::Bold);
    nameText.setPosition(335.f, 115.f);
    window.draw(nameText);

    // Separador
    sf::RectangleShape sep(sf::Vector2f(420.f, 2.f));
    sep.setFillColor(sf::Color(100, 100, 200));
    sep.setPosition(330.f, 160.f);
    window.draw(sep);

    // Descripción (línea por línea)
    float lineY = 175.f;
    std::string desc = ch.description;
    std::string line;
    for (size_t i = 0; i <= desc.size(); ++i)
    {
        if (i == desc.size() || desc[i] == '\n')
        {
            sf::Text lineText(line, font, 18);
            lineText.setFillColor(sf::Color(220, 220, 220));
            lineText.setPosition(335.f, lineY);
            window.draw(lineText);
            lineY += 30.f;
            line.clear();
        }
        else
            line += desc[i];
    }

    // Título encima
    sf::Text title("Informacion del Personaje", font, 22);
    title.setFillColor(sf::Color(200, 200, 255));
    title.setPosition(40.f, 55.f);
    window.draw(title);

    drawBackButton();
    window.display();
}

// ── Draw: Configuración ───────────────────────────────────────────────────────
void MenuScreen::drawSettingsScreen()
{
    window.clear(sf::Color(20, 22, 40));

    sf::Text header("Configuracion", font, 42);
    header.setFillColor(sf::Color(255, 215, 0));
    header.setStyle(sf::Text::Bold);
    header.setPosition(40.f, 40.f);
    window.draw(header);

    std::vector<std::string> lines = {
        "- Volumen: Ajusta el sonido del juego",
        "- Pantalla: Modo ventana o pantalla completa",
        "- Controles: Personaliza teclas",
        "- Idioma: Espanol o Ingles"
    };
    for (size_t i = 0; i < lines.size(); ++i)
    {
        sf::Text line(lines[i], font, 24);
        line.setFillColor(sf::Color::White);
        line.setPosition(60.f, 130.f + i * 50.f);
        window.draw(line);
    }

    drawBackButton();
    window.display();
}

void MenuScreen::drawBackButton()
{
    window.draw(backButton);
    window.draw(backText);
}

// ── Game loop ─────────────────────────────────────────────────────────────────
void MenuScreen::run()
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == MenuState::MAIN_MENU)
            {
                if (event.type == sf::Event::MouseMoved)
                    updateButtonHover(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    int idx = getButtonAtPosition(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    if (idx >= 0)
                        handleMainMenuClick(idx);
                }
            }
            else
            {
                handleSubmenuEvents(event);
            }
        }

        if (!window.isOpen()) break;

        if      (state == MenuState::MAIN_MENU)        drawMainMenu();
        else if (state == MenuState::CHARACTERS)        drawCharactersScreen();
        else if (state == MenuState::CHARACTER_DETAIL)  drawCharacterDetail(detailIndex);
        else if (state == MenuState::SETTINGS)          drawSettingsScreen();
    }
}

bool MenuScreen::shouldLaunchGame() const { return launchGame; }
