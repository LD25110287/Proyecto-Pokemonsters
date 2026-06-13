#include "../include/MenuScreen.h"
#include <iostream>

MenuScreen::MenuScreen()
    : window(sf::VideoMode(800, 600), "Pokemonsters", sf::Style::Close),
      state(MenuState::MAIN_MENU),
      hoveredButton(-1),
      launchGame(false),
      detailIndex(-1),
      volumeLevel(5) // Volumen inicial por defecto en la mitad (5 de 10)
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

    // ── Inicialización de la Interfaz de Configuración de Volumen ─────────────
    txtVolumeTitle.setFont(font);
    txtVolumeTitle.setString("Volumen Musica:");
    txtVolumeTitle.setCharacterSize(26);
    txtVolumeTitle.setFillColor(sf::Color::White);
    txtVolumeTitle.setPosition(70.f, 180.f);

    // Botón Menos [-]
    btnDecrease.setSize(sf::Vector2f(50.f, 50.f));
    btnDecrease.setFillColor(sf::Color(50, 50, 70));
    btnDecrease.setOutlineColor(sf::Color(100, 100, 130));
    btnDecrease.setOutlineThickness(2.f);
    btnDecrease.setPosition(70.f, 240.f);

    txtDecrease.setFont(font);
    txtDecrease.setString("-");
    txtDecrease.setCharacterSize(32);
    txtDecrease.setFillColor(sf::Color::White);
    txtDecrease.setPosition(88.f, 245.f);

    // Botón Más [+]
    btnIncrease.setSize(sf::Vector2f(50.f, 50.f));
    btnIncrease.setFillColor(sf::Color(50, 50, 70));
    btnIncrease.setOutlineColor(sf::Color(100, 100, 130));
    btnIncrease.setOutlineThickness(2.f);
    btnIncrease.setPosition(470.f, 240.f);

    txtIncrease.setFont(font);
    txtIncrease.setString("+");
    txtIncrease.setCharacterSize(32);
    txtIncrease.setFillColor(sf::Color::White);
    txtIncrease.setPosition(483.f, 245.f);

    // Texto del valor numérico
    txtVolumeValue.setFont(font);
    txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
    txtVolumeValue.setCharacterSize(24);
    txtVolumeValue.setFillColor(sf::Color(255, 215, 0));
    txtVolumeValue.setPosition(545.f, 250.f);

    // ── Inicialización y reproducción de la música ────────────────────────────
    if (!menuMusic.openFromFile("assets/audio/musica menu.mp3"))
    {
        std::cerr << "Error: No se pudo cargar assets/audio/musica menu.mp3\n";
    }
    else
    {
        menuMusic.setLoop(true);
        // volumeLevel (5) * 10 = 50% de volumen real en SFML
        menuMusic.setVolume(static_cast<float>(volumeLevel * 10));
        menuMusic.play();
    }
}

// ── Assets ────────────────────────────────────────────────────────────────────
void MenuScreen::loadAssets()
{
    std::vector<std::string> fallbacks = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf"
    };
    for (auto& p : fallbacks)
    {
        if (font.loadFromFile(p))
            break;
    }
}

void MenuScreen::loadCharacterPortraits()
{
    chars = {
        { "Exdarktyranomon", "Un monstruo oscuro digital con un poder viral devastador y garras de hierro.", "assets/images/champ_select_1.png" },
        { "BeelStarmon",     "Una pistolera letal de los campos digitales que ataca a distancia con gran agilidad.", "assets/images/champ_select_2.png" },
        { "Bioquetzalmon",   "Ser alado ancestral que controla corrientes bio-electricas y posee una defensa mitica.", "assets/images/champ_select_3.png" },
        { "Jesmon",          "Caballero sagrado que domina el arte de la espada de energia con un espitiru indomable.", "assets/images/champ_select_4.png" },
        { "Sleipmon",        "Guerrero de armadura helada que posee una velocidad colosal y resistencia extrema.", "assets/images/champ_select_5.png" },
        { "Magnamon",        "Protector dorado con una armadura de Chrome Digizoid capaz de soportar ataques masivos.", "assets/images/champ_select_6.png" }
    };

    charCards.resize(6);
    charCardTexts.resize(6);

    for (size_t i = 0; i < chars.size(); ++i)
    {
        if (!chars[i].texture.loadFromFile(chars[i].portraitPath))
        {
            std::cerr << "Error cargando portrait: " << chars[i].portraitPath << "\n";
        }
        else
        {
            chars[i].loaded = true;
            chars[i].sprite.setTexture(chars[i].texture);
            
            auto sz = chars[i].texture.getSize();
            chars[i].sprite.setScale(120.f / sz.x, 120.f / sz.y);
        }

        int col = i % 3;
        int row = i / 3;
        float x = 110.f + col * 200.f;
        float y = 160.f + row * 180.f;

        charCards[i].setSize(sf::Vector2f(160.f, 150.f));
        charCards[i].setFillColor(sf::Color(45, 45, 65));
        charCards[i].setOutlineColor(sf::Color(100, 100, 150));
        charCards[i].setOutlineThickness(2.f);
        charCards[i].setPosition(x, y);

        if (chars[i].loaded)
        {
            chars[i].sprite.setPosition(x + 20.f, y + 10.f);
        }

        charCardTexts[i].setFont(font);
        charCardTexts[i].setString(chars[i].name);
        charCardTexts[i].setCharacterSize(14);
        charCardTexts[i].setFillColor(sf::Color::White);
        
        float tx = x + (160.f - charCardTexts[i].getLocalBounds().width) / 2.f;
        charCardTexts[i].setPosition(tx, y + 125.f);
    }
}

void MenuScreen::createMenuButtons()
{
    std::vector<std::string> names = {
        "Iniciar Batalla",
        "Personajes",
        "Configuracion",
        "Salir"
    };

    buttons.resize(4);
    buttonTexts.resize(4);

    float startX = 70.f;
    float startY = 170.f;
    float spacing = 75.f;

    for (size_t i = 0; i < names.size(); ++i)
    {
        buttons[i].setSize(sf::Vector2f(320.f, 55.f));
        buttons[i].setFillColor(sf::Color(30, 30, 45));
        buttons[i].setOutlineColor(sf::Color(70, 70, 100));
        buttons[i].setOutlineThickness(2.f);
        buttons[i].setPosition(startX, startY + i * spacing);

        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(names[i]);
        buttonTexts[i].setCharacterSize(24);
        buttonTexts[i].setFillColor(sf::Color(210, 210, 230));
        buttonTexts[i].setPosition(startX + 25.f, startY + i * spacing + 12.f);
    }
}

// ── Lógica e Interacción ──────────────────────────────────────────────────────
void MenuScreen::updateButtonHover(const sf::Vector2i& mousePos)
{
    hoveredButton = getButtonAtPosition(mousePos);

    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if ((int)i == hoveredButton)
        {
            buttons[i].setFillColor(sf::Color(50, 50, 80));
            buttons[i].setOutlineColor(sf::Color(255, 215, 0));
            buttonTexts[i].setFillColor(sf::Color::White);
        }
        else
        {
            buttons[i].setFillColor(sf::Color(30, 30, 45));
            buttons[i].setOutlineColor(sf::Color(70, 70, 100));
            buttonTexts[i].setFillColor(sf::Color(210, 210, 230));
        }
    }
}

int MenuScreen::getButtonAtPosition(const sf::Vector2i& mousePos) const
{
    sf::Vector2f m(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i].getGlobalBounds().contains(m))
            return static_cast<int>(i);
    }
    return -1;
}

void MenuScreen::handleMainMenuClick(int index)
{
    if (index == 0) // Iniciar Batalla
    {
        launchGame = true;
        menuMusic.stop(); // Detiene la música al ir a la pantalla de juego
        window.close();
    }
    else if (index == 1) // Personajes
    {
        state = MenuState::CHARACTERS;
    }
    else if (index == 2) // Configuración
    {
        state = MenuState::SETTINGS;
    }
    else if (index == 3) // Salir
    {
        menuMusic.stop();
        window.close();
    }
}

void MenuScreen::handleSubmenuEvents(const sf::Event& event)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f m(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Efecto visual simple al pasar el mouse por los botones de Configuración
    if (state == MenuState::SETTINGS)
    {
        if (btnDecrease.getGlobalBounds().contains(m))
            btnDecrease.setFillColor(sf::Color(75, 75, 105));
        else
            btnDecrease.setFillColor(sf::Color(50, 50, 70));

        if (btnIncrease.getGlobalBounds().contains(m))
            btnIncrease.setFillColor(sf::Color(75, 75, 105));
        else
            btnIncrease.setFillColor(sf::Color(50, 50, 70));
    }

    // Comportamiento del botón Volver
    if (backButton.getGlobalBounds().contains(m))
    {
        backButton.setFillColor(sf::Color(255, 0, 0));
    }
    else
    {
        backButton.setFillColor(sf::Color(180, 0, 0));
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        // Si se hace clic en "Volver"
        if (backButton.getGlobalBounds().contains(m))
        {
            if (state == MenuState::CHARACTER_DETAIL)
                state = MenuState::CHARACTERS;
            else
                state = MenuState::MAIN_MENU;
            return;
        }

        // Clics específicos de la pantalla CONFIGURACIÓN
        if (state == MenuState::SETTINGS)
        {
            // Botón Bajar Volumen [-]
            if (btnDecrease.getGlobalBounds().contains(m))
            {
                if (volumeLevel > 0)
                {
                    volumeLevel--;
                    menuMusic.setVolume(static_cast<float>(volumeLevel * 10));
                    txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
                }
            }
            // Botón Subir Volumen [+]
            else if (btnIncrease.getGlobalBounds().contains(m))
            {
                if (volumeLevel < 10)
                {
                    volumeLevel++;
                    menuMusic.setVolume(static_cast<float>(volumeLevel * 10));
                    txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
                }
            }
        }

        // Clics específicos de la pantalla de listado de Personajes
        if (state == MenuState::CHARACTERS)
        {
            for (size_t i = 0; i < charCards.size(); ++i)
            {
                if (charCards[i].getGlobalBounds().contains(m))
                {
                    detailIndex = static_cast<int>(i);
                    state = MenuState::CHARACTER_DETAIL;
                    break;
                }
            }
        }
    }
}

// ── Renderizado ───────────────────────────────────────────────────────────────
void MenuScreen::drawMainMenu()
{
    window.clear(sf::Color(15, 15, 25));

    window.draw(titleText);
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        window.draw(buttons[i]);
        window.draw(buttonTexts[i]);
    }

    window.display();
}

void MenuScreen::drawCharactersScreen()
{
    window.clear(sf::Color(15, 15, 25));

    sf::Text subTitle("Selecciona un personaje para ver detalles", font, 24);
    subTitle.setFillColor(sf::Color(200, 200, 220));
    subTitle.setPosition(70.f, 40.f);
    window.draw(subTitle);

    sf::Vector2i mp = sf::Mouse::getPosition(window);
    sf::Vector2f m(static_cast<float>(mp.x), static_cast<float>(mp.y));

    for (size_t i = 0; i < charCards.size(); ++i)
    {
        if (charCards[i].getGlobalBounds().contains(m))
            charCards[i].setOutlineColor(sf::Color(255, 215, 0));
        else
            charCards[i].setOutlineColor(sf::Color(100, 100, 150));

        window.draw(charCards[i]);
        if (chars[i].loaded)
            window.draw(chars[i].sprite);
        window.draw(charCardTexts[i]);
    }

    drawBackButton();
    window.display();
}

void MenuScreen::drawCharacterDetail(int index)
{
    window.clear(sf::Color(20, 20, 35));

    if (index >= 0 && index < (int)chars.size())
    {
        sf::Text nameTxt(chars[index].name, font, 36);
        nameTxt.setFillColor(sf::Color(255, 215, 0));
        nameTxt.setStyle(sf::Text::Bold);
        nameTxt.setPosition(320.f, 60.f);
        window.draw(nameTxt);

        if (chars[index].loaded)
        {
            sf::Sprite largeSprite = chars[index].sprite;
            largeSprite.setScale(200.f / chars[index].texture.getSize().x, 200.f / chars[index].texture.getSize().y);
            largeSprite.setPosition(70.f, 120.f);
            window.draw(largeSprite);
        }

        sf::Text descTxt(chars[index].description, font, 18);
        descTxt.setFillColor(sf::Color(200, 200, 200));
        descTxt.setPosition(320.f, 130.f);
        
        std::string rawStr = chars[index].description;
        std::string formattedStr = "";
        size_t currentLineLength = 0;
        for (char c : rawStr)
        {
            formattedStr += c;
            currentLineLength++;
            if (currentLineLength > 35 && c == ' ')
            {
                formattedStr += "\n";
                currentLineLength = 0;
            }
        }
        descTxt.setString(formattedStr);
        window.draw(descTxt);
    }

    drawBackButton();
    window.display();
}

void MenuScreen::drawSettingsScreen()
{
    window.clear(sf::Color(15, 15, 25));

    sf::Text subTitle("Configuracion", font, 36);
    subTitle.setFillColor(sf::Color(255, 215, 0));
    subTitle.setStyle(sf::Text::Bold);
    subTitle.setPosition(70.f, 40.f);
    window.draw(subTitle);

    // Dibujar textos y botones de volumen
    window.draw(txtVolumeTitle);
    window.draw(btnDecrease);
    window.draw(txtDecrease);
    window.draw(btnIncrease);
    window.draw(txtIncrease);
    window.draw(txtVolumeValue);

    // Dibujar la barra de volumen segmentada de 10 bloques
    float startBarX = 140.f;
    float barY = 245.f;
    float blockWidth = 28.f;
    float blockHeight = 40.f;
    float spacing = 4.f;

    for (int i = 0; i < 10; ++i)
    {
        sf::RectangleShape block(sf::Vector2f(blockWidth, blockHeight));
        block.setPosition(startBarX + i * (blockWidth + spacing), barY);

        // Si el bloque actual está dentro del volumen actual, se pinta encendido (Celeste/Cian)
        if (i < volumeLevel)
        {
            block.setFillColor(sf::Color(0, 191, 255));
            block.setOutlineColor(sf::Color::White);
            block.setOutlineThickness(1.f);
        }
        else // Si no, se pinta apagado (Gris oscuro)
        {
            block.setFillColor(sf::Color(40, 40, 45));
            block.setOutlineColor(sf::Color(60, 60, 65));
            block.setOutlineThickness(1.f);
        }
        window.draw(block);
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