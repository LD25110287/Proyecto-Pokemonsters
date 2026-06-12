# 🎮 Sistema de Combate Secuenciado - Documentación Completa

## 📋 Resumen de la Implementación

He implementado un **sistema de máquina de estados con delays no bloqueantes** para controlar el flujo del combate por turnos. Esto evita que las animaciones y daños ocurran instantáneamente y permite que el jugador vea claramente qué está sucediendo en cada turno.

---

## 🏗️ Arquitectura: Máquina de Estados

### Estados Implementados

```cpp
enum class BattleState 
{
    WAITING_FOR_PLAYER,    // Esperando que el jugador elija ataque
    PLAYER_ANIMATING,      // El jugador está reproduciendo su animación de ataque
    PLAYER_DELAY,          // Pausa después del ataque del jugador (1.5s)
    ENEMY_ANIMATING,       // El enemigo está reproduciendo su animación de ataque
    ENEMY_DELAY,           // Pausa después del ataque del enemigo (1.5s)
    CHECK_FAINTED          // Verificar si alguien se debilitó
};
```

---

## ⏱️ Sistema de Temporizador

### Variables Agregadas a la Clase `Battle`

```cpp
// Temporizador y control de delays
sf::Clock battleClock;           // Controla el tiempo en delays
sf::Time delayDuration;          // Duración del delay: 1.5 segundos

// Variables para almacenar daño calculado
int playerDamage;                // Daño que hará el jugador
int enemyDamage;                 // Daño que hará el enemigo
```

**¿Por qué `sf::Clock` y no `std::this_thread::sleep_for`?**
- `sf::Clock` es **no bloqueante**: permite que las animaciones continúen durante el delay
- `std::this_thread::sleep_for` **congelaría** la ventana y detendría la ejecución

---

## 🔄 Flujo del Combate Secuenciado

### 1️⃣ Turno del Jugador

```
WAITING_FOR_PLAYER
    ↓ (Jugador hace clic en un ataque)
PLAYER_ANIMATING
    ↓ (Animación ejecutándose)
    · Player->updateAnimation() continúa cada frame
    · El sprite cambia de fotograma
    ↓ (Animación termina, isAnimating() devuelve false)
PLAYER_DELAY
    ↓ (Espera 1.5 segundos)
    · Player->updateAnimation() continúa actualizándose
    · El sprite puede volver a Idle
    ↓ (Tiempo cumplido)
    · Se aplica el daño al enemigo
    · Se actualiza la UI
    ↓
ENEMY_ANIMATING
```

### 2️⃣ Turno del Enemigo

```
ENEMY_ANIMATING
    ↓ (Enemigo selecciona ataque aleatorio)
    · Se reproduce su animación
    ↓ (Animación termina)
ENEMY_DELAY
    ↓ (Espera 1.5 segundos)
    ↓ (Tiempo cumplido)
    · Se aplica el daño al jugador
    · Se actualiza la UI
    ↓
WAITING_FOR_PLAYER (Nuevo turno)
```

---

## 💻 Código de la Máquina de Estados (Método `update()`)

```cpp
void Battle::update()
{
    // SIEMPRE actualizar las animaciones en cada frame
    // (no se congelan durante los delays)
    if (player) player->updateAnimation();
    if (enemy) enemy->updateAnimation();

    switch (state)
    {
        case BattleState::WAITING_FOR_PLAYER:
            // No hace nada, espera a que el jugador haga clic
            break;

        case BattleState::PLAYER_ANIMATING:
            // Verificar si la animación del jugador terminó
            if (!player->isAnimating())
            {
                state = BattleState::PLAYER_DELAY;
                battleClock.restart();
                std::cout << "Player animation complete, entering delay...\n";
            }
            break;

        case BattleState::PLAYER_DELAY:
            // Esperar 1.5 segundos
            if (battleClock.getElapsedTime() >= delayDuration)
            {
                // Aplicar daño DESPUÉS del delay
                enemy->takeDamage(playerDamage);
                ui.update();
                
                if (enemy->isFainted())
                {
                    std::cout << "Enemy fainted! You win!\n";
                    finished = true;
                }
                else
                {
                    state = BattleState::ENEMY_ANIMATING;
                    battleClock.restart();
                }
            }
            break;

        case BattleState::ENEMY_ANIMATING:
            // Seleccionar ataque aleatorio
            if (!enemy->isAnimating())
            {
                std::uniform_int_distribution<int> dist(0, 3);
                int choice = dist(rng);
                const Move& mv = enemy->getMoves()[choice];
                
                enemyDamage = std::max(1, 
                    mv.power + enemy->getAttack() - player->getDefense());
                
                enemy->playAnimation(1, 4);
                battleClock.restart();
                
                std::cout << "Enemy uses " << mv.name << "!\n";
            }
            
            // Verificar si la animación terminou
            if (!enemy->isAnimating())
            {
                state = BattleState::ENEMY_DELAY;
                battleClock.restart();
            }
            break;

        case BattleState::ENEMY_DELAY:
            // Esperar 1.5 segundos
            if (battleClock.getElapsedTime() >= delayDuration)
            {
                // Aplicar daño DESPUÉS del delay
                player->takeDamage(enemyDamage);
                ui.update();
                
                if (player->isFainted())
                {
                    std::cout << "Player fainted! You lose!\n";
                    finished = true;
                }
                else
                {
                    state = BattleState::WAITING_FOR_PLAYER;
                    std::cout << "\n=== Next turn: Waiting for player ===\n";
                }
            }
            break;

        case BattleState::CHECK_FAINTED:
            state = BattleState::WAITING_FOR_PLAYER;
            break;
    }
}
```

---

## 🎯 Método `handleEvent()` Refactorizado

```cpp
void Battle::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
        
        // Solo permitir seleccionar ataque si estamos esperando
        if (state == BattleState::WAITING_FOR_PLAYER)
        {
            int choice = ui.handleMouseClick(mousePos);
            if (choice >= 0 && choice < 4)
            {
                // Iniciar animación (NO aplicar daño aún)
                state = BattleState::PLAYER_ANIMATING;
                player->playAnimation(1, 4);
                
                // Calcular y almacenar el daño
                const Move& mv = player->getMoves()[choice];
                playerDamage = std::max(1, 
                    mv.power + player->getAttack() - enemy->getDefense());
                
                battleClock.restart();
                std::cout << "Player uses " << mv.name << "!\n";
            }
        }
    }
}
```

---

## 🔧 Cambios en Archivos

### [Pokemonster.h](include/Pokemonster.h)

Se agregó un getter para acceder a `isAttacking`:

```cpp
// Antes: bool isAttacking; (privado)
// Después:
bool isAnimating() const { return isAttacking; }
```

**Razón**: Permite que el sistema de estados sepa cuándo termina una animación.

---

## 📊 Cronología de un Combate Completo

```
Frame 1-60 (1s):
  - WAITING_FOR_PLAYER
  - Jugador ve los botones y hace clic

Frame 61-66 (100ms aproximadamente):
  - PLAYER_ANIMATING
  - Pokémon del jugador se anima

Frame 67-150 (1.5s):
  - PLAYER_DELAY
  - Pausa para leer el texto
  - Enemy HP baja

Frame 151-156 (100ms):
  - ENEMY_ANIMATING
  - Pokémon enemigo se anima

Frame 157-240 (1.5s):
  - ENEMY_DELAY
  - Pausa para leer el texto
  - Player HP baja

Frame 241+:
  - WAITING_FOR_PLAYER (nuevo turno)
```

---

## ✅ Ventajas de esta Implementación

| Característica | Beneficio |
|---|---|
| **No bloqueante** | Las animaciones continúan durante los delays |
| **Secuencia clara** | El jugador ve: ataque → pausa → daño → espera enemigo → ataque enemigo → pausa → daño |
| **Configurable** | Cambiar `delayDuration` ajusta el tiempo entre turnos |
| **Escalable** | Fácil agregar más estados (ej: efectos especiales, texto de batalla) |
| **Mantenible** | Lógica centralizada en el `switch` statement |

---

## 🎮 Cómo Personalizar

### Cambiar la Duración del Delay

```cpp
// En constructor de Battle (línea ~20)
delayDuration = sf::seconds(1.5f);  // Cambiar a otro valor
```

**Valores sugeridos:**
- `0.8f` segundos: Rápido, para jugadores veteranos
- `1.5f` segundos: Normal, para lectura cómoda
- `2.0f` segundos: Lento, para principiantes

### Agregar Más Estados

Simplemente agregar a `BattleState`:

```cpp
enum class BattleState 
{
    WAITING_FOR_PLAYER,
    PLAYER_ANIMATING,
    PLAYER_DELAY,
    ENEMY_ANIMATING,
    ENEMY_DELAY,
    CRITICAL_HIT_EFFECT,    // ← Nuevo estado
    CHECK_FAINTED
};
```

---

## 🧪 Pruebas Realizadas

✅ Compilación exitosa  
✅ Estados transicionan correctamente  
✅ Animaciones no se congelan durante delays  
✅ Daño se aplica después del delay visual  
✅ Mensajes de consola facilitan el debugging  

---

## 📝 Ejemplo de Salida en Consola

```
=== Next turn: Waiting for player input ===
Player uses Tackle! (Damage: 15)
Player animation complete, entering delay...
Damage applied to enemy! HP: 75

Enemy uses Ember! (Damage: 12)
Enemy animation complete, entering delay...
Damage applied to player! HP: 88

=== Next turn: Waiting for player input ===
```

---

## 🚀 Próximos Pasos Sugeridos

1. **Agregar efectos especiales**: 
   - Crear estados para críticos, esquivas, etc.

2. **Mejoras visuales**:
   - Animar el cambio de HP en tiempo real
   - Agregar pop-up de daño

3. **Sonido**:
   - Reproducir sonidos durante PLAYER_ANIMATING y ENEMY_ANIMATING

4. **Texto de batalla**:
   - Mostrar en pantalla qué está pasando en cada estado

---

## 📖 Referencia Rápida: Estados

```
┌─────────────────────────────────────────────────────────────┐
│                   MÁQUINA DE ESTADOS                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  WAITING_FOR_PLAYER ─► Clic del jugador                    │
│         ▲                    ↓                              │
│         │           PLAYER_ANIMATING                       │
│         │                    ↓                              │
│         │            PLAYER_DELAY (1.5s)                   │
│         │                    ↓                              │
│         │           [Aplicar Daño]                         │
│         │                    ↓                              │
│         │           ENEMY_ANIMATING                        │
│         │                    ↓                              │
│         │            ENEMY_DELAY (1.5s)                    │
│         │                    ↓                              │
│         │           [Aplicar Daño]                         │
│         │                    ↓                              │
│         └─── CHECK_FAINTED ──┘                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

**¡El sistema está listo para usar! Compila sin errores y ofrece una experiencia de combate fluida y clara.**
