# Pokemon Game - SFML

Un juego sencillo de Pokemon desarrollado en C++ usando la librería SFML3.

## Requisitos

- MSYS2 instalado en `C:\msys64`
- SFML3 instalado en MSYS2
- CMake
- GCC/G++ (incluido en MSYS2)

## Compilación

```bash
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```

## Ejecución

```bash
./pokemon-game
```

## Estructura del Proyecto

```
pokemon-game/
├── src/              - Archivos fuente (.cpp)
├── include/          - Archivos header (.h)
├── assets/
│   ├── images/       - Imágenes y texturas
│   └── sounds/       - Efectos de sonido
├── CMakeLists.txt    - Configuración de CMake
└── README.md
```

## Características

- Ventana básica de 800x600 píxeles
- Sistema de eventos (tecla ESC para cerrar)
- Base para agregar sprites y lógica de juego

## Controles

- `ESC` - Salir del juego
- Cerrar ventana - Salir del juego
