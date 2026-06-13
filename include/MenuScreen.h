#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class MenuScreen
{
public:
    MenuScreen();
    void run();
    bool shouldLaunchGame() const;

private:
    enum class MenuState
    {
        MAIN_MENU,
        CHARACTERS,
        CHARACTER_DETAIL,  // ← pantalla de detalle de un personaje
        SETTINGS
    };

    struct CharEntry
    {
        std::string name;
        std::string description;
        std::string portraitPath;
        sf::Texture texture;
        sf::Sprite  sprite;
        bool        loaded = false;
    };

    void loadAssets();
    void loadCharacterPortraits();
    void createMenuButtons();
    void updateButtonHover(const sf::Vector2i& mousePos);
    int  getButtonAtPosition(const sf::Vector2i& mousePos) const;
    void handleMainMenuClick(int index);
    void handleSubmenuEvents(const sf::Event& event);

    void drawMainMenu();
    void drawCharactersScreen();
    void drawCharacterDetail(int index);
    void drawSettingsScreen();
    void drawBackButton();

    sf::RenderWindow window;
    sf::Font         font;
    sf::Text         titleText;

    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text>           buttonTexts;

    sf::RectangleShape backButton;
    sf::Text           backText;

    MenuState state;
    int       hoveredButton;
    bool      launchGame;

    // Personajes para la pantalla de detalle
    std::vector<CharEntry> chars;
    int detailIndex; // cuál personaje se está mostrando (-1 = ninguno)

    // Botones de la pantalla Characters (6 cards)
    std::vector<sf::RectangleShape> charCards;
    std::vector<sf::Text>           charCardTexts;
};

#endif // MENUSCREEN_H
