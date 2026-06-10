cmake_minimum_required(VERSION 3.16)
project(PokemonGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuración para MSYS2
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

# Buscar SFML
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# Directorios de inclusión
include_directories(${CMAKE_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Pokemonster.cpp
    src/BattleUI.cpp
)

# Crear ejecutable
add_executable(pokemonster ${SOURCES})
target_include_directories(pokemonster PRIVATE ${CMAKE_SOURCE_DIR}/include)
# Enlazar librerías SFML
target_link_libraries(pokemonster
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copiar assets a la carpeta de construcción
add_custom_command(TARGET pokemonster POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:pokemonster>/assets
)
