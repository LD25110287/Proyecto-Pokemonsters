#include "Game.h"

#include <random>
#include <memory>
#include <iostream>

#include "../include/Move.h"
#include "../include/Pokemonster.h"
#include "../include/BattleUI.h"

enum class BattleState { WAITING_PLAYER, PLAYER_ATTACK, ENEMY_ATTACK };

class Battle
{
public:
    Battle(sf::RenderWindow& win)
        : window(win), ui(win.getSize()), player(nullptr), enemy(nullptr), state(BattleState::WAITING_PLAYER), rng(std::random_device{}())
    {
    }

    void init();
    void handleEvent(const sf::Event& event);
    void update();
    void render();
    bool isFinished() const { return finished; }

private:
    sf::RenderWindow& window;
    BattleUI ui;
    Pokemonster* player;
    Pokemonster* enemy;
    BattleState state;
    bool finished = false;

    std::mt19937 rng;
};

// Implementación mínima de la batalla
void Battle::init()
{
    // Crear movimientos de ejemplo
    std::vector<Move> pMoves = {
        Move("Tackle", 10, MoveType::Normal, "assets/sounds/tackle.wav"),
        Move("Flame", 14, MoveType::Fire, "assets/sounds/flame.wav"),
        Move("Splash", 8, MoveType::Water, "assets/sounds/splash.wav"),
        Move("Leaf Cut", 12, MoveType::Grass, "assets/sounds/leaf.wav")
    };

    std::vector<Move> eMoves = {
        Move("Bite", 11, MoveType::Normal, "assets/sounds/bite.wav"),
        Move("Ember", 13, MoveType::Fire, "assets/sounds/ember.wav"),
        Move("Water Jet", 9, MoveType::Water, "assets/sounds/waterjet.wav"),
        Move("Vine Lash", 12, MoveType::Grass, "assets/sounds/vinelash.wav")
    };

    static Pokemonster playerP("Pikachu", 100, 20, 10, pMoves);
    static Pokemonster enemyP("Bulbasaur", 90, 18, 8, eMoves);

    player = &playerP;
    enemy = &enemyP;

    // intentamos cargar texturas (si existen)
    player->loadTexture("assets/images/1.png");
    enemy->loadTexture("assets/images/2.png");

    player->setPosition(100.f, 300.f);
    enemy->setPosition(500.f, 80.f);

    ui.setPlayerPokemon(player);
    ui.setEnemyPokemon(enemy);
    ui.update();
}

void Battle::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        if (state == BattleState::WAITING_PLAYER)
        {
            int choice = ui.handleMouseClick(mousePos);
            if (choice >= 0 && choice < 4)
            {
                // almacenar elección en una variable local y cambiar de estado
                state = BattleState::PLAYER_ATTACK;

                // calcular daño
                const Move& mv = player->getMoves()[choice];
                int damage = std::max(1, mv.power + player->getAttack() - enemy->getDefense());

                // reproducir sonido si existe (comentado - no hay Audio en SFML 2.6)
                // sf::SoundBuffer buffer;
                // if (buffer.loadFromFile(mv.soundPath))
                // {
                //     sf::Sound sound(buffer);
                //     sound.play();
                // }

                enemy->takeDamage(damage);
                ui.update();

                if (enemy->isFainted())
                {
                    std::cout << "Enemy fainted! You win.\n";
                    finished = true;
                }
                else
                {
                    state = BattleState::ENEMY_ATTACK;
                }
            }
        }
    }
}

void Battle::update()
{
    if (state == BattleState::ENEMY_ATTACK && !finished)
    {
        std::uniform_int_distribution<int> dist(0, 3);
        int choice = dist(rng);
        const Move& mv = enemy->getMoves()[choice];
        int damage = std::max(1, mv.power + enemy->getAttack() - player->getDefense());

        // reproducir sonido si existe (comentado - no hay Audio en SFML 2.6)
        // sf::SoundBuffer buffer;
        // if (buffer.loadFromFile(mv.soundPath))
        // {
        //     sf::Sound sound(buffer);
        //     sound.play();
        // }

        player->takeDamage(damage);
        ui.update();

        if (player->isFainted())
        {
            std::cout << "Player fainted! You lose.\n";
            finished = true;
        }
        else
        {
            state = BattleState::WAITING_PLAYER;
        }
    }
}

void Battle::render()
{
    window.clear(sf::Color(30, 30, 30));

    // dibujar pokemons
    if (enemy) window.draw(*enemy);
    if (player) window.draw(*player);

    // dibujar UI encima
    ui.draw(window);

    window.display();
}


Game::Game() : isRunning(true)
{
    window.create(sf::VideoMode({800, 600}), "Pokemon Game");
    window.setFramerateLimit(60);
}

Game::~Game()
{
    window.close();
}

void Game::run()
{
    Battle battle(window);
    battle.init();

    while (window.isOpen() && isRunning && !battle.isFinished())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                isRunning = false;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
                isRunning = false;
            }

            // pasar evento a la batalla
            battle.handleEvent(event);
        }

        battle.update();
        battle.render();
    }
}

void Game::handleEvents()
{
    // ahora manejado dentro de run/battle
}

void Game::update()
{
}

void Game::render()
{
}

