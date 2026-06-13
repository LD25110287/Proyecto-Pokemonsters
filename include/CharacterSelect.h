#ifndef CHARACTER_SELECT_H
#define CHARACTER_SELECT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Datos de un personaje seleccionable
struct CharacterData
{
    std::string name;
    std::string spritePath;
    std::string portraitPath;
    int frameWidth;
    int frameHeight;
    int hp;
    int attack;
    int defense;
};

class CharacterSelect
{
public:
    CharacterSelect();
    void run();

    bool shouldLaunchBattle() const { return launchBattle; }
    int  getPlayer1Choice()   const { return p1Choice; }
    int  getPlayer2Choice()   const { return p2Choice; }

private:
    // Fases de selección
    enum class Phase { PLAYER1, PLAYER2, CONFIRM };

    void loadAssets();
    void buildGrid();
    void handleEvents();
    void update();
    void render();
    void drawGrid(int highlightIndex, int lockedIndex, const std::string& label);
    void drawConfirmScreen();
    int  getCardAt(sf::Vector2i mousePos) const;

    sf::RenderWindow window;
    sf::Font         font;
    Phase            phase;

    // Los 6 personajes
    std::vector<CharacterData> characters;

    // Portraits cargados
    std::vector<sf::Texture> portraits;
    std::vector<sf::Sprite>  portraitSprites;

    // Grid: 3 columnas x 2 filas, cada card 160x160
    static const int COLS      = 3;
    static const int ROWS      = 2;
    static const int CARD_W    = 160;
    static const int CARD_H    = 160;
    static const int GRID_X    = 80;   // margen izquierdo
    static const int GRID_Y    = 120;  // margen superior
    static const int PADDING   = 20;

    int hoveredCard;  // índice bajo el mouse (-1 si ninguno)
    int p1Choice;     // índice elegido por J1 (-1 = no elegido)
    int p2Choice;     // índice elegido por J2 (-1 = no elegido)

    bool launchBattle;
};

#endif // CHARACTER_SELECT_H
