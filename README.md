
# Digital Monsters Battle

## 📝 Descripción del Proyecto 
**Digital Monsters Battle** es un juego de combate estratégico 2D con estética *pixel art* que une la toma de decisiones tácticas de las batallas de monstruos clásicas con el formato competitivo de equipos de los juegos de lucha. Selecciona tu equipo de Digital Monsters, elige el orden de combate y utiliza tus mejores ataques para derrotar al equipo rival en intensos combates por turnos.

## 🎯 Objetivo del Juego
Eliminar a los monstruos del equipo rival reduciendo sus Puntos de Vida (HP) a cero en un combate al mejor de 3 rondas. El primer jugador en derrotar 2 veces a su oponente gana la partida.

## 🎮 Controles
El juego está diseñado para ser completamente controlado mediante una interfaz gráfica interactiva, ideal para jugar en modo local compartido (*Hotseat*):
* **Clic Izquierdo del Mouse**: Seleccionar opciones en los menús, elegir personajes y activar las habilidades en pantalla durante el turno correspondiente.

## ⚙️ Mecánicas
* **Selección de Equipo (Drafting):** Cada jugador elige un equipo de 3 personajes y define estratégicamente el orden en el que entrarán a la arena.
* **Sistema de Supervivencia (Estilo KOF):** Cuando un personaje derrota a su rival, se mantiene en combate para la siguiente ronda conservando los Puntos de Vida (HP) y Energía exactos con los que finalizó el encuentro anterior.
* **Combate Táctico por turnos:** Dinámica de turnos donde cada monstruo cuenta con un conjunto de 4 habilidades únicas para mermar la vida del oponente.
* **Triángulo de Atributos (Ventajas Dinámicas):** Existen 3 clases de atributos en el juego, y cada monstruo posee solo uno. Se rigen bajo una lógica circular (fuerte, neutro o débil). Atacar con ventaja elemental otorga un multiplicador de daño de **1.2x a 1.5x**, obligando al jugador a analizar la compatibilidad antes de actuar.
* **Economía y Gestión de Energía:** Los jugadores inician el combate con 3 de 10 barras de energía máximas. Al finalizar cada turno (sin importar de quién sea), ambos jugadores reciben de forma pasiva +1 barra de energía. Dado que 3 de las 4 habilidades de cada monstruo exigen un coste específico de este recurso, se mitiga el uso repetitivo de ataques poderosos y se fomenta la planificación a mediano plazo.
* **Variabilidad Dinámica de Daño (RNG / Factor Suerte):** Las habilidades reducen la salud del rival basándose en un porcentaje de vida, pero el daño neto no es plano. Incorpora un sistema de fluctuación aleatoria (por ejemplo, permitiendo que un mismo ataque inflija desde un 10% hasta un 18% de daño). Este factor predictivo simula la adrenalina de los golpes críticos, permitiendo giros inesperados en momentos decisivos.
* **Relevo Estratégico con Costo:** Los jugadores pueden cambiar a su monstruo activo en cualquier turno para defenderse de un atributo desfavorable. Sin embargo, este cambio consume la acción del turno actual, sacrificando la posibilidad de infligir daño en esa ronda.
* **Interfaz Dinámica:** Al cambiar el turno del jugador, el sistema actualiza dinámicamente el panel de habilidades en pantalla, permitiendo que ambos compartan el mismo dispositivo de forma fluida.

## 🏆 Características
* **Sistema de Menús Interactivo:** Implementación de botones dinámicos basados en la detección de colisiones del puntero del mouse y gestión de estados de juego en SFML.
* **Profundidad Táctica (Atributos y Relevos):** Matriz de daño variable integrada en el código para calcular ventajas elementales en tiempo real y sistema de penalización de turnos por cambios de personajes.
* **Simulación de Combate Avanzada (Energía y RNG):** Algoritmos de actualización pasiva de recursos por ciclo de turno y generación de números aleatorios en C++ para calcular las desviaciones de daño de las habilidades.
* **Fidelidad Estética Pixel Art:** Renderizado e integración de hojas de sprites (*spritesheets*) con animaciones fluidas para representar fielmente el estilo clásico de Digimon/Pokémon.

## 👥 Equipo
* **Andrea Ivonne Sánchez Aguayo** ([@AndreaSan27206](https://github.com/AndreaSan27206))
* **Leonardo Daniel Alvarado Peraza** ([@LD25110287](https://github.com/LD25110287))

## 🛠️ Tecnologías
* **Motor/Framework:** SFML 2.6
* **Lenguaje:** C++
* **Entorno/Herramientas:** MSYS2
* **Edición Gráfica:** Paint.NET

## 📜 Créditos
* **Personajes Pixel Art:** [Dragonrod342](https://www.deviantart.com/dragonrod342)
* **Sonidos de Ronda:** [MurdoinkGS en YouTube](https://www.youtube.com/@MurdoinkGS) — *Street Fighter 6 green screen: Round 1 2 3 4 Final Round, Fight, KO, Perfect KO*
* **Música de Combate:** [Toby Wheeler en YouTube](https://www.youtube.com/@toby_wheeler) — *The Last Element / Hyper Spirit Evolution Instrumental · Digimon Frontier*
* **Música de Menú:** [Markhfz en YouTube](https://www.youtube.com/@markhfz) — *Under Night In-Birth character select BGM*
* **Iconos de Selección (Champ_Select 1-6):** [Grindosaur](https://www.grindosaur.com/en)
* **Fondo de la Arena (Background Arena):** [Zerostin en Reddit](https://www.reddit.com/user/Zerostin/) — ([Post original](https://www.reddit.com/r/PixelArt/comments/n6frxp/made_this_battle_background_yesterday_for_my_game/))
* **Fondo de Cristal Rojo (Background Red Cristal):** [Namatnieks en X](https://x.com/Namatnieks) — ([Post original](https://x.com/Namatnieks/status/1603002590920429571))
* **Fondo de Campo (Background Campo):** [Funjeffrey en Pinterest](https://mx.pinterest.com/funjeffrey/) — ([Pin original](https://mx.pinterest.com/pin/713257659715497242/))
* **Tabla de Atributos:** [DMOWiki](https://dmowiki.com) — ([Imagen original](https://dmowiki.com/File:DA_VA_VI_NO_UN.png))
* **Recursos Propios:** El resto de las imágenes e interfaces del juego fueron diseñadas y elaboradas de forma personal por Leonardo Daniel Alvarado Peraza utilizando Paint.NET con base en referencias visuales de Google Fotos y ediciones de autoría propia.