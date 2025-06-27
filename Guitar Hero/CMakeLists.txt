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
    message(STATUS "Compilando para Windows")

    # Diretórios do Allegro
    target_include_directories(GuitarHero PRIVATE ${CMAKE_INCLUDE_PATH})
    link_directories(${CMAKE_LIBRARY_PATH})
    link_directories(${CMAKE_SOURCE_DIR}/allegro/lib)

    # Bibliotecas necessárias
    target_link_libraries(GuitarHero
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_audio.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_acodec.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_font.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_image.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_primitives.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_ttf.dll.a
    ${CMAKE_SOURCE_DIR}/allegro/lib/liballegro_main.dll.a
)


    # Copiar DLLs automaticamente para o build
    file(GLOB ALLEGRO_DLLS "${CMAKE_SOURCE_DIR}/allegro/bin/*.dll")
    foreach(DLL_FILE ${ALLEGRO_DLLS})
        add_custom_command(TARGET GuitarHero POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_FILE}"
            "$<TARGET_FILE_DIR:GuitarHero>"
            COMMENT "Copiando ${DLL_FILE} para a pasta de saida"
        )
    endforeach()
    
    file(GLOB MINGW_DLLS "${CMAKE_SOURCE_DIR}/runtime-dlls/*.dll")
    foreach(DLL_FILE ${MINGW_DLLS})
        add_custom_command(TARGET GuitarHero POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_FILE}"
            "$<TARGET_FILE_DIR:GuitarHero>"
            COMMENT "Copiando runtime DLL ${DLL_FILE} para a pasta de saida"
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
