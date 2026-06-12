#include "MenuScreen.h"
#include "Game.h"

#include <iostream>

MenuScreen::MenuScreen()
    : window(sf::VideoMode(800, 600), "Pokemonsters", sf::Style::Close),
      state(MenuState::MAIN_MENU),
      hoveredButton(-1)
{
    // Cargar fuente de la carpeta de assets o mostrar advertencia si no existe
    loadAssets();
    createMenuButtons();

    titleText.setFont(font);
    titleText.setString("Pokemonsters");
    titleText.setCharacterSize(56);
    titleText.setFillColor(sf::Color(255, 215, 0));
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(70.f, 40.f);

    // Botón de regreso para subpantallas
    backButton.setSize({180.f, 45.f});
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

void MenuScreen::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/arial.ttf"))
    {
        std::cerr << "Warning: No se pudo cargar assets/fonts/arial.ttf. Se usará fuente por defecto.\n";
    }
}

void MenuScreen::createMenuButtons()
{
    const float buttonWidth = 360.f;
    const float buttonHeight = 60.f;
    const float startX = (window.getSize().x - buttonWidth) / 2.f;
    const float startY = 200.f;
    const float spacing = 80.f;

    std::vector<std::string> labels = {"Jugar", "Personajes", "Configuraci\xF3n", "Salir"};

    buttons.resize(labels.size());
    buttonTexts.resize(labels.size());

    for (size_t i = 0; i < labels.size(); ++i)
    {
        buttons[i].setSize({buttonWidth, buttonHeight});
        buttons[i].setFillColor(sf::Color(200, 0, 0));
        buttons[i].setOutlineColor(sf::Color::White);
        buttons[i].setOutlineThickness(3.f);
        buttons[i].setPosition(startX, startY + i * spacing);

        buttonTexts[i].setFont(font);
        buttonTexts[i].setString(labels[i]);
        buttonTexts[i].setCharacterSize(28);
        buttonTexts[i].setFillColor(sf::Color::White);
        buttonTexts[i].setStyle(sf::Text::Bold);
        float textX = startX + (buttonWidth - buttonTexts[i].getLocalBounds().width) / 2.f - 8.f;
        float textY = startY + i * spacing + (buttonHeight - buttonTexts[i].getLocalBounds().height) / 2.f - 8.f;
        buttonTexts[i].setPosition(textX, textY);
    }
}

void MenuScreen::updateButtonHover(const sf::Vector2i& mousePos)
{
    hoveredButton = -1;
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            hoveredButton = static_cast<int>(i);
            break;
        }
    }
}

int MenuScreen::getButtonAtPosition(const sf::Vector2i& mousePos) const
{
    for (size_t i = 0; i < buttons.size(); ++i)
    {
        if (buttons[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            return static_cast<int>(i);
    }
    return -1;
}

void MenuScreen::handleMainMenuClick(int index)
{
    switch (index)
    {
        case 0: // Jugar
        {
            window.close();
            Game game;
            game.run();
            break;
        }
        case 1: // Personajes
            state = MenuState::CHARACTERS;
            break;
        case 2: // Configuración
            state = MenuState::SETTINGS;
            break;
        case 3: // Salir
            window.close();
            break;
    }
}

void MenuScreen::handleSubmenuEvents(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        if (backButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
        {
            state = MenuState::MAIN_MENU;
        }
    }
}

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

void MenuScreen::drawCharactersScreen()
{
    window.clear(sf::Color(15, 20, 40));

    sf::Text header("Personajes", font, 42);
    header.setFillColor(sf::Color(255, 215, 0));
    header.setStyle(sf::Text::Bold);
    header.setPosition(40.f, 40.f);
    window.draw(header);

    std::vector<std::string> lines = {
        "- Pikachu: El eléctrico explorador",
        "- Bulbasaur: Planta fuerte y resistente",
        "- Charmander: Fuego ardiente",
        "- Squirtle: Agua refrescante"
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

void MenuScreen::drawSettingsScreen()
{
    window.clear(sf::Color(20, 22, 40));

    sf::Text header("Configuraci\xF3n", font, 42);
    header.setFillColor(sf::Color(255, 215, 0));
    header.setStyle(sf::Text::Bold);
    header.setPosition(40.f, 40.f);
    window.draw(header);

    std::vector<std::string> lines = {
        "- Volumen: Ajusta el sonido del juego",
        "- Pantalla: Activa modo ventana o pantalla completa",
        "- Controles: Personaliza teclas y acceso rapido",
        "- Idioma: Cambia entre español e ingles"
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
                {
                    updateButtonHover(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                }
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    int buttonIndex = getButtonAtPosition(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    if (buttonIndex >= 0)
                    {
                        handleMainMenuClick(buttonIndex);
                    }
                }
            }
            else
            {
                handleSubmenuEvents(event);
            }
        }

        if (!window.isOpen())
            break;

        if (state == MenuState::MAIN_MENU)
        {
            drawMainMenu();
        }
        else if (state == MenuState::CHARACTERS)
        {
            drawCharactersScreen();
        }
        else if (state == MenuState::SETTINGS)
        {
            drawSettingsScreen();
        }
    }
}
