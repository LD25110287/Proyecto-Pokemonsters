#include "../include/MenuScreen.h"
#include "../include/AudioManager.h"
#include "../include/Pokemonster.h"   // para Attribute
#include <iostream>

// ── Atributos por personaje (mismo orden que chars[]) ─────────────────────────
static const Attribute MENU_CHAR_ATTR[6] = {
    Attribute::Vacuna,  // 0: Exdarktyranomon
    Attribute::Virus,   // 1: BeelStarmon
    Attribute::Vacuna,  // 2: Bioquetzalmon
    Attribute::Data,    // 3: Jesmon
    Attribute::Vacuna,  // 4: Sleipmon
    Attribute::Vacuna,  // 5: Magnamon
};

// Nombre legible del atributo (para mostrar texto de respaldo si no carga el ícono)
static const char* ATTR_NAME[3] = { "Vacuna", "Virus", "Data" };

static int menuAttrIndex(Attribute a)
{
    switch (a) {
        case Attribute::Vacuna: return 0;
        case Attribute::Virus:  return 1;
        case Attribute::Data:   return 2;
    }
    return 0;
}

MenuScreen::MenuScreen()
    : window(sf::VideoMode(800, 600), "Pokemonsters", sf::Style::Close),
      state(MenuState::MAIN_MENU),
      hoveredButton(-1),
      launchGame(false),
      detailIndex(-1),
      volumeLevel(static_cast<int>(AudioManager::getVolume() / 10.f + 0.5f))
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

    // ── Configuración de Volumen ──────────────────────────────────────────────
    txtVolumeTitle.setFont(font);
    txtVolumeTitle.setString("Volumen Musica:");
    txtVolumeTitle.setCharacterSize(26);
    txtVolumeTitle.setFillColor(sf::Color::White);
    txtVolumeTitle.setPosition(70.f, 180.f);

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

    txtVolumeValue.setFont(font);
    txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
    txtVolumeValue.setCharacterSize(24);
    txtVolumeValue.setFillColor(sf::Color(255, 215, 0));
    txtVolumeValue.setPosition(545.f, 250.f);
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
        if (font.loadFromFile(p)) break;

    // ── Íconos de atributo ────────────────────────────────────────────────────
    const std::string attrPaths[3] = {
        "assets/images/atributo_Va.png",   // 0 = Vacuna
        "assets/images/atributo_Vi.png",   // 1 = Virus
        "assets/images/atributo_Da.png",   // 2 = Data
    };
    for (int i = 0; i < 3; ++i)
    {
        attrLoaded[i] = attrTextures[i].loadFromFile(attrPaths[i]);
        if (!attrLoaded[i])
            std::cerr << "[Warning] No se pudo cargar: " << attrPaths[i] << "\n";
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
            std::cerr << "Error cargando portrait: " << chars[i].portraitPath << "\n";
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
            chars[i].sprite.setPosition(x + 20.f, y + 10.f);

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
        "Iniciar Batalla", "Personajes", "Configuracion", "Salir"
    };

    buttons.resize(4);
    buttonTexts.resize(4);

    float startX = 70.f, startY = 170.f, spacing = 75.f;

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
        if (buttons[i].getGlobalBounds().contains(m))
            return static_cast<int>(i);
    return -1;
}

void MenuScreen::handleMainMenuClick(int index)
{
    if      (index == 0) { launchGame = true; window.close(); }
    else if (index == 1) state = MenuState::CHARACTERS;
    else if (index == 2) state = MenuState::SETTINGS;
    else if (index == 3) window.close();
}

void MenuScreen::handleSubmenuEvents(const sf::Event& event)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f m(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (state == MenuState::SETTINGS)
    {
        btnDecrease.setFillColor(btnDecrease.getGlobalBounds().contains(m)
            ? sf::Color(75, 75, 105) : sf::Color(50, 50, 70));
        btnIncrease.setFillColor(btnIncrease.getGlobalBounds().contains(m)
            ? sf::Color(75, 75, 105) : sf::Color(50, 50, 70));
    }

    backButton.setFillColor(backButton.getGlobalBounds().contains(m)
        ? sf::Color(255, 0, 0) : sf::Color(180, 0, 0));

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        if (backButton.getGlobalBounds().contains(m))
        {
            state = (state == MenuState::CHARACTER_DETAIL)
                    ? MenuState::CHARACTERS : MenuState::MAIN_MENU;
            return;
        }

        if (state == MenuState::SETTINGS)
        {
            if (btnDecrease.getGlobalBounds().contains(m) && volumeLevel > 0)
            {
                volumeLevel--;
                AudioManager::setVolume(static_cast<float>(volumeLevel * 10));
                txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
            }
            else if (btnIncrease.getGlobalBounds().contains(m) && volumeLevel < 10)
            {
                volumeLevel++;
                AudioManager::setVolume(static_cast<float>(volumeLevel * 10));
                txtVolumeValue.setString(std::to_string(volumeLevel) + " / 10");
            }
        }

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
        charCards[i].setOutlineColor(charCards[i].getGlobalBounds().contains(m)
            ? sf::Color(255, 215, 0) : sf::Color(100, 100, 150));

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
        // ── Nombre + ícono de atributo ────────────────────────────────────────
        // El nombre va en x=320, y=60. Ponemos el ícono a su izquierda.
        const float ICON_SIZE = 32.f;
        const float NAME_X    = 320.f;
        const float NAME_Y    = 60.f;

        // Calcular ancho real del nombre para centrar ícono verticalmente con él
        sf::Text nameTxt(chars[index].name, font, 36);
        nameTxt.setFillColor(sf::Color(255, 215, 0));
        nameTxt.setStyle(sf::Text::Bold);
        nameTxt.setPosition(NAME_X, NAME_Y);
        window.draw(nameTxt);

        // Ícono a la izquierda del nombre, centrado verticalmente con él
        Attribute attr = MENU_CHAR_ATTR[index];
        int ai = menuAttrIndex(attr);
        if (attrLoaded[ai])
        {
            sf::Sprite iconSprite(attrTextures[ai]);
            auto sz = attrTextures[ai].getSize();
            iconSprite.setScale(ICON_SIZE / sz.x, ICON_SIZE / sz.y);

            // Centrar verticalmente respecto al texto del nombre
            float nameHeight = nameTxt.getLocalBounds().height;
            float iconX = NAME_X - ICON_SIZE - 8.f;
            float iconY = NAME_Y + (nameHeight - ICON_SIZE) / 2.f;
            iconSprite.setPosition(iconX, iconY);
            window.draw(iconSprite);
        }
        else
        {
            // Fallback: texto con el nombre del atributo si no cargó la imagen
            sf::Text attrTxt("[" + std::string(ATTR_NAME[ai]) + "]", font, 14);
            attrTxt.setFillColor(sf::Color(180, 220, 255));
            attrTxt.setPosition(NAME_X - 70.f, NAME_Y + 10.f);
            window.draw(attrTxt);
        }

        // ── Imagen grande del personaje ───────────────────────────────────────
        if (chars[index].loaded)
        {
            sf::Sprite largeSprite = chars[index].sprite;
            largeSprite.setScale(
                200.f / chars[index].texture.getSize().x,
                200.f / chars[index].texture.getSize().y);
            largeSprite.setPosition(70.f, 120.f);
            window.draw(largeSprite);
        }

        // ── Descripción con salto de línea automático ─────────────────────────
        std::string rawStr = chars[index].description;
        std::string formattedStr;
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

        sf::Text descTxt(formattedStr, font, 18);
        descTxt.setFillColor(sf::Color(200, 200, 200));
        descTxt.setPosition(320.f, 130.f);
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

    window.draw(txtVolumeTitle);
    window.draw(btnDecrease);
    window.draw(txtDecrease);
    window.draw(btnIncrease);
    window.draw(txtIncrease);
    window.draw(txtVolumeValue);

    float startBarX = 140.f, blockWidth = 28.f, blockHeight = 18.f,
          spacing = 4.f, barY = 256.f;

    for (int i = 0; i < 10; ++i)
    {
        sf::RectangleShape block(sf::Vector2f(blockWidth, blockHeight));
        block.setPosition(startBarX + i * (blockWidth + spacing), barY);
        if (i < volumeLevel)
        {
            block.setFillColor(sf::Color(0, 191, 255));
            block.setOutlineColor(sf::Color::White);
        }
        else
        {
            block.setFillColor(sf::Color(40, 40, 45));
            block.setOutlineColor(sf::Color(60, 60, 65));
        }
        block.setOutlineThickness(1.f);
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
                    if (idx >= 0) handleMainMenuClick(idx);
                }
            }
            else
            {
                handleSubmenuEvents(event);
            }
        }

        if (!window.isOpen()) break;

        if      (state == MenuState::MAIN_MENU)       drawMainMenu();
        else if (state == MenuState::CHARACTERS)       drawCharactersScreen();
        else if (state == MenuState::CHARACTER_DETAIL) drawCharacterDetail(detailIndex);
        else if (state == MenuState::SETTINGS)         drawSettingsScreen();
    }
}

bool MenuScreen::shouldLaunchGame() const { return launchGame; }
