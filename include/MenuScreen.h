#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>

class MenuScreen
{
public:
    MenuScreen();
    void run();

private:
    enum class MenuState
    {
        MAIN_MENU,
        CHARACTERS,
        SETTINGS
    };

    void loadAssets();
    void createMenuButtons();
    void updateButtonHover(const sf::Vector2i& mousePos);
    int getButtonAtPosition(const sf::Vector2i& mousePos) const;
    void handleMainMenuClick(int index);
    void drawMainMenu();
    void drawCharactersScreen();
    void drawSettingsScreen();
    void drawBackButton();
    void handleSubmenuEvents(const sf::Event& event);

    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonTexts;
    sf::RectangleShape backButton;
    sf::Text backText;
    MenuState state;
    int hoveredButton;
};

#endif // MENUSCREEN_H
