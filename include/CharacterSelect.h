#ifndef CHARACTER_SELECT_H
#define CHARACTER_SELECT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

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
    int  getSelectedStage()   const { return selectedStage; }

private:
    enum class Phase { PLAYER1, PLAYER2, CONFIRM, STAGE_SELECT };

    void loadAssets();
    void buildGrid();
    void handleEvents();
    void update();
    void render();
    void drawGrid(int highlightIndex, int lockedIndex, const std::string& label);
    void drawConfirmScreen();
    void drawStageSelect();
    int  getCardAt(sf::Vector2i mousePos) const;

    sf::RenderWindow window;
    sf::Font         font;
    Phase            phase;

    std::vector<CharacterData> characters;
    std::vector<sf::Texture>   portraits;
    std::vector<sf::Sprite>    portraitSprites;

    // Fondo de la pantalla de confirmación (imagen VS)
    sf::Texture vsBgTexture;
    sf::Sprite  vsBgSprite;

    // Fondos de batalla para la pantalla de selección de escenario
    static const int NUM_STAGES = 3;
    sf::Texture      stageBgTextures[NUM_STAGES];
    sf::Sprite       stageBgSprites[NUM_STAGES];
    int              selectedStage;    // -1 = sin elegir, 0-2 = elegido
    sf::Clock        stageAnimClock;   // controla la animación de ruleta
    bool             stageAnimDone;

    static const int COLS    = 3;
    static const int ROWS    = 2;
    static const int CARD_W  = 160;
    static const int CARD_H  = 160;
    static const int GRID_X  = 80;
    static const int GRID_Y  = 120;
    static const int PADDING = 20;

    int  hoveredCard;
    int  p1Choice;
    int  p2Choice;
    bool launchBattle;
};

#endif // CHARACTER_SELECT_H
