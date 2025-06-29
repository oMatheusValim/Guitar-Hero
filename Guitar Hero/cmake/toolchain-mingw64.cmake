# Define que esta compilando para Windows
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Compiladores
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Diretórios customizados do Allegro que estão dentro do repositório
set(ALLEGRO_ROOT "${CMAKE_SOURCE_DIR}/allegro")

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/allegro")
set(CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/allegro/lib")
set(CMAKE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/allegro/include")


set(CMAKE_PREFIX_PATH "${ALLEGRO_ROOT}")
set(CMAKE_INCLUDE_PATH "${ALLEGRO_ROOT}/include")
set(CMAKE_LIBRARY_PATH "${ALLEGRO_ROOT}/lib")
