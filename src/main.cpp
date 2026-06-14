#include "../include/Game.h"
#include "../include/MenuScreen.h"
#include "../include/CharacterSelect.h"
#include "../include/AudioManager.h"

int main()
{
    // ── Sonido de introducción (3 seg, una sola vez) ─────────────────────────
    AudioManager::playIntro();

    // Bucle principal: permite regresar al menú después de una partida
    while (true)
    {
        // ── 1. Menú principal ─────────────────────────────────────────────────
        AudioManager::playMenuMusic();
        MenuScreen menu;
        menu.run();

        if (!menu.shouldLaunchGame())
            return 0;   // El jugador eligió Salir

        // ── 2. Selección de personajes (3 por jugador) ────────────────────────
        // Misma música de menú (no se reinicia si ya estaba sonando)
        AudioManager::playMenuMusic();
        CharacterSelect selector;
        selector.run();

        if (!selector.shouldLaunchBattle())
            continue;   // Volvió al menú sin jugar

        // ── 3. Serie de 3 rondas ──────────────────────────────────────────────
        const auto& p1Team = selector.getPlayer1Team();
        const auto& p2Team = selector.getPlayer2Team();
        int stage          = selector.getSelectedStage();

        int p1Wins = 0;
        int p2Wins = 0;

        // Música de combate: empieza antes de la primera ronda y continúa
        // sonando sin interrupción entre rondas (currentTrack ya es BATTLE)
        AudioManager::playBattleMusic();

        for (int round = 0; round < 3; ++round)
        {
            if (p1Wins >= 2 || p2Wins >= 2)
                break;

            Game game(p1Team[round], p2Team[round], stage);
            game.run();

            if (game.getWinner() == 1)
                p1Wins++;
            else if (game.getWinner() == 2)
                p2Wins++;
        }

        // La serie termina → vuelve al while(true) y AudioManager::playMenuMusic()
        // cambiará de pista automáticamente al inicio de la siguiente vuelta.

    }   // fin while(true)

    return 0;
}
