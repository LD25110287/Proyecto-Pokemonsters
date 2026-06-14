#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SFML/Audio.hpp>

// ── AudioManager ──────────────────────────────────────────────────────────────
// Maneja la música de fondo de toda la aplicación con un único sf::Music
// estático. Cambia de pista solo cuando es necesario (evita reinicios
// si ya está sonando la pista correcta).
class AudioManager
{
public:
    enum class Track { NONE, MENU, BATTLE };

    // Sonido de introducción: se reproduce UNA VEZ al abrir la app (3 seg,
    // bloqueante porque todavía no hay ninguna ventana abierta).
    static void playIntro();

    // Música de menú (en bucle): menú principal, personajes, configuración,
    // selección de personajes y pantalla de confirmación.
    static void playMenuMusic();

    // Música de combate (en bucle): mientras dura la batalla.
    static void playBattleMusic();

    // Detiene cualquier música en reproducción.
    static void stopMusic();

private:
    static sf::Music music;
    static Track     currentTrack;
};

#endif // AUDIOMANAGER_H
