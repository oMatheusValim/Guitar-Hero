# windows-toolchain.cmake
# Configura o CMake para compilação cruzada para Windows

set(CMAKE_SYSTEM_NAME Windows)

# Aponta para os compiladores corretos do MinGW-w64
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

# Define o caminho raiz para encontrar as bibliotecas e headers do MinGW
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Configura como o CMake procura por programas e bibliotecas
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)