cmake_minimum_required(VERSION 3.10)
project(GuitarHero)

# Configuração básica
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontra a Allegro e seus componentes usando PkgConfig (o método confiável para Debian)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED 
    allegro-5 
    allegro_image-5 
    allegro_audio-5 
    allegro_acodec-5 
    allegro_ttf-5 
    allegro_primitives-5
)

# Inclui os diretórios de cabeçalho da Allegro e do nosso projeto
include_directories(
    include
    ${ALLEGRO_INCLUDE_DIRS}
)

# Arquivos fonte
set(SOURCE_FILES
    src/main.cpp
    src/game.cpp
    src/file_handler.cpp
    src/note_manager.cpp
)

# Cria o executável
add_executable(guitar_hero ${SOURCE_FILES})

# Linka o executável com as bibliotecas da Allegro
target_link_libraries(guitar_hero PRIVATE ${ALLEGRO_LIBRARIES})

# Copia a pasta de assets para o diretório de build para que o jogo encontre as fontes e músicas
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})