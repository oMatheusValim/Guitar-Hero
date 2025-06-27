cmake_minimum_required(VERSION 3.15)
project(GuitarHero)

set(CMAKE_CXX_STANDARD 17)

# Arquivos fonte e headers
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(GuitarHero ${SOURCES} ${HEADERS})

# Detecta o sistema operacional
if (WIN32)
    message(STATUS "Compilando para Windows (Cross-compilation)")

    # Aponta para a pasta 'allegro' do seu projeto
    set(ALLEGRO_WIN_DIR ${CMAKE_SOURCE_DIR}/allegro)

    # Adiciona o diretório 'include' para o compilador
    target_include_directories(GuitarHero PRIVATE ${ALLEGRO_WIN_DIR}/include)

    # Linka diretamente com a biblioteca de importação "monolith"
    # Esta é a forma mais simples e robusta para este pacote
    target_link_libraries(GuitarHero PRIVATE
        "${ALLEGRO_WIN_DIR}/lib/liballegro_monolith.dll.a"
    )

    # --- Copia as DLLs necessárias para a pasta final ---

    # Copia a DLL principal do Allegro ("monolith")
    file(COPY "${ALLEGRO_WIN_DIR}/bin/allegro_monolith-5.2.dll" DESTINATION $<TARGET_FILE_DIR:GuitarHero>)

    # Copia as DLLs de runtime do MinGW (ainda são necessárias)
    file(GLOB MINGW_DLLS "${CMAKE_SOURCE_DIR}/runtime-dlls/*.dll")
    foreach(DLL_FILE ${MINGW_DLLS})
        add_custom_command(TARGET GuitarHero POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_FILE}"
            "$<TARGET_FILE_DIR:GuitarHero>"
        )
    endforeach()
else()
    message(STATUS "Compilando para Linux")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(ALLEGRO REQUIRED
        allegro-5
        allegro_font-5
        allegro_audio-5
        allegro_acodec-5
        allegro_primitives-5
        allegro_ttf-5
    )

    include_directories(${ALLEGRO_INCLUDE_DIRS})
    link_directories(${ALLEGRO_LIBRARY_DIRS})

    target_link_libraries(GuitarHero ${ALLEGRO_LIBRARIES})
    endif()

# Copia a pasta assets
add_custom_command(TARGET GuitarHero POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GuitarHero>/assets
)
