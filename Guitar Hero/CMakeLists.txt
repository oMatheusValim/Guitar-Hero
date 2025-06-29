cmake_minimum_required(VERSION 3.15)
project(GuitarHero)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(GuitarHero ${SOURCES} ${HEADERS})

if (WIN32)
    message(STATUS "Compilando para Windows (Cross-compilation)")

    set(ALLEGRO_WIN_DIR ${CMAKE_SOURCE_DIR}/allegro)

    target_include_directories(GuitarHero PRIVATE ${ALLEGRO_WIN_DIR}/include)

    target_link_libraries(GuitarHero PRIVATE
        "${ALLEGRO_WIN_DIR}/lib/liballegro_monolith.dll.a"
    )

    # --- Copia as DLLs necessárias para a pasta final ---
    add_custom_command(TARGET GuitarHero POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${ALLEGRO_WIN_DIR}/bin/allegro_monolith-5.2.dll"
        "$<TARGET_FILE_DIR:GuitarHero>"
        COMMENT "Copiando a DLL principal do Allegro"
    )

    # Copia as DLLs de runtime do MinGW 
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

add_custom_command(TARGET GuitarHero POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GuitarHero>/assets
)
