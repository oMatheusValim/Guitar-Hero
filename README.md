# Guitar Hero em C++

![Linguagem](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Linux%20%7C%20Windows-brightgreen.svg)
![Licença](https://img.shields.io/badge/Licen%C3%A7a-MIT-yellow.svg)

Um protótipo do clássico jogo de ritmo Guitar Hero, desenvolvido em C++ com a biblioteca gráfica Allegro 5. O projeto foi compilado para rodar nativamente em Linux e também em Windows através de compilação cruzada.

---

### Tabela de Conteúdos
1.  [Descrição do Projeto](#descrição-do-projeto)
2.  [Recursos do Jogo](#recursos-do-jogo)
3.  [Tecnologias Utilizadas](#tecnologias-utilizadas)
4.  [Como Jogar (Para Usuários)](#como-jogar-para-usuários)
5.  [Instruções para Compilação (Para Desenvolvedores)](#instruções-para-compilação-para-desenvolvedores)
6.  [Autores](#autores)

---

## Descrição do Projeto
Este projeto foi desenvolvido como um protótipo do famoso jogo Guitar Hero para a disciplina SSC0601 - Introdução à Ciência da Computação I, na Universidade de São Paulo (USP), campus São Carlos.

O intuito foi aplicar os conhecimentos adquiridos ao longo da matéria, como funções, vetores, ponteiros, structs, manipulação de arquivos e alocação dinâmica de memória, para criar um jogo funcional na linguagem C++. O projeto foi realizado em parceria pelos alunos Matheus Valim Nogueira e Arthur Silva de Albuquerque.

## Recursos do Jogo
* **Menu Principal:** Navegue entre as opções de "Jogar" e "Sair".
* **Seleção de Músicas:** O jogo carrega automaticamente as músicas da pasta `assets/songs`.
* **Gameplay de 5 Pistas:** Jogue no ritmo da música usando as teclas **A, S, J, K, L**.
* **Feedback Visual:** Os alvos mudam de cor ao acertar uma nota, fornecendo feedback imediato da sua precisão.
* **Sistema de Pontuação:** Cada nota certa aumenta sua pontuação.
* **Tela de Pontuação Final:** Ao final de cada música, sua pontuação é exibida com opções para jogar novamente, escolher outra música ou voltar ao menu.
* **Portabilidade:** Jogue no Linux ou no Windows!

## Tecnologias Utilizadas
* **Linguagem C++17**
* **Biblioteca Allegro 5** para interface gráfica, áudio e gerenciamento de eventos.
* **CMake** para gerenciamento de compilação multiplataforma.
* **MinGW-w64** para compilação cruzada de Linux para Windows.

## Como Jogar (Para Usuários)
A maneira mais fácil de jogar é baixar a versão já compilada do jogo.

1.  Vá para a seção de **[Releases](https://github.com/oMatheusValim/Guitar-Hero/releases)** deste repositório no GitHub.
2.  **Para Windows:** Baixe o arquivo `.zip` mais recente (ex: `GuitarHero-Windows-v1.0.zip`).
    * Descompacte o arquivo.
    * **Importante:** Se o Windows exibir um aviso de segurança, clique com o botão direito no `.zip`, vá em `Propriedades` e marque a caixa `Desbloquear` **antes** de extrair.
    * Abra a pasta e execute o `GuitarHero.exe`.
3.  **Para Linux:** Baixe o arquivo `.zip` ou `.tar.gz` para Linux.
    * Descompacte o arquivo.
    * Abra a pasta e execute o arquivo `GuitarHero`.

## Instruções para Compilação (Para Desenvolvedores)
Se você deseja compilar o código-fonte por conta própria, siga os passos abaixo.

### Pré-requisitos no Linux
Você precisará do `g++`, `cmake`, `make` e das bibliotecas de desenvolvimento do Allegro 5.
```bash
sudo apt-get update
sudo apt-get install build-essential cmake liballegro5-dev liballegro-font5-dev liballegro-image5-dev liballegro-acodec5-dev liballegro-primitives5-dev liballegro-ttf5-dev
```

### Compilando no Linux
```bash
# A partir da pasta raiz do projeto

# Apague a pasta de build antiga (se existir)
rm -rf build

# Crie e entre na nova pasta de build
mkdir build
cd build

# Gere os arquivos de compilação com o CMake
cmake ..

# Compile o projeto
make

# Execute o jogo
./GuitarHero
```

### Compilando para Windows (Compilação Cruzada no Linux)
Primeiro, instale o compilador cruzado MinGW:
```bash
sudo apt-get install mingw-w64
```
Depois, siga os passos para compilar:
```bash
# A partir da pasta raiz do projeto

# Apague a pasta de build antiga (se existir)
rm -rf build-windows

# Crie e entre na nova pasta de build
mkdir build-windows
cd build-windows

# Gere os arquivos de compilação, especificando o toolchain do Windows
cmake .. -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake

# Compile o projeto para gerar o .exe e as .dlls
make
```
A pasta `build-windows` conterá todos os arquivos prontos para serem compactados e executados em uma máquina Windows.
    
## Autores
* **Matheus Valim Nogueira** - [oMatheusValim](https://github.com/oMatheusValim)
* **Arthur Silva de Albuquerque** - [Arthas01 ](https://github.com/Arthas01)