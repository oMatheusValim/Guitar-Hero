cmake_minimum_required(VERSION 3.15)
project(GuitarHero)

set(CMAKE_CXX_STANDARD 17)

# Adiciona todos os arquivos fonte
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_executable(GuitarHero ${SOURCES} ${HEADERS})

# Detecta o sistema operacional
if (WIN32)
    message(STATUS "Compilando para Windows")

    # Diretórios personalizados do Allegro incluídos no projeto
    target_include_directories(GuitarHero PRIVATE ${CMAKE_SOURCE_DIR}/allegro/include)
    link_directories(${CMAKE_SOURCE_DIR}/allegro/lib)

    # Bibliotecas Allegro para Windows
    target_link_libraries(GuitarHero
        allegro_monolith
    )

    # Copiar DLLs para o diretório de build automaticamente (somente no Windows)
    file(GLOB DLLS "${CMAKE_SOURCE_DIR}/allegro/bin/*.dll")
    foreach(DLL ${DLLS})
        add_custom_command(TARGET GuitarHero POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL}
            $<TARGET_FILE_DIR:GuitarHero>
        )
    endforeach()

    target_include_directories(GuitarHero PRIVATE ${CMAKE_SOURCE_DIR}/include)

else() # Linux
    message(STATUS "Compilando para Linux")

    # Usa Allegro do sistema (instale com: sudo apt install liballegro5-dev)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(ALLEGRO REQUIRED allegro-5 allegro_font-5 allegro_audio-5 allegro_acodec-5 allegro_primitives-5 allegro_ttf-5)

    include_directories(${ALLEGRO_INCLUDE_DIRS})
    link_directories(${ALLEGRO_LIBRARY_DIRS})

    target_link_libraries(GuitarHero ${ALLEGRO_LIBRARIES})
    target_include_directories(GuitarHero PRIVATE ${CMAKE_SOURCE_DIR}/include)
endif()

# Copiar pasta assets para o build final (Windows e Linux)
add_custom_command(TARGET GuitarHero POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GuitarHero>/assets
)
