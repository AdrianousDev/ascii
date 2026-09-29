# Animações ASCII no terminal

Este projeto contém dois programas independentes:

- `donut.c`: um donut girando, escrito em C.
- `alien.cpp`: um rosto alienígena girando, escrito em C++.

Execute os comandos abaixo na pasta que contém os dois arquivos. As animações
usam sequências ANSI para atualizar a tela. Use um terminal com pelo menos 80
colunas e 24 linhas. Pressione `Ctrl+C` para encerrar.

## Linux

É necessário ter `gcc` e `g++` instalados. Confira com `gcc --version` e
`g++ --version`.

Compile cada programa separadamente:

```bash
gcc -O2 donut.c -o donut -lm
g++ -std=c++17 -O2 alien.cpp -o alien
```

Execute o programa desejado:

```bash
./donut
./alien
```

A opção `-lm` liga a biblioteca matemática usada por `donut.c`.

## Windows

Instale o [MSYS2](https://www.msys2.org/) e abra o terminal **MSYS2 UCRT64**.
Nesse terminal, instale o compilador C/C++:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

O pacote e o ambiente UCRT64 são os indicados na
[documentação do MSYS2](https://www.msys2.org/docs/environments/).
No terminal UCRT64, entre na pasta do projeto. Por exemplo, para
`C:\Users\SeuNome\projetos\et-ascii`:

```bash
cd /c/Users/SeuNome/projetos/et-ascii
```

Compile os dois programas:

```bash
gcc -O2 donut.c -o donut.exe -lm
g++ -std=c++17 -O2 alien.cpp -o alien.exe
```

Execute o programa desejado no mesmo terminal:

```bash
./donut.exe
./alien.exe
```

Para conferir apenas um quadro do alienígena, use `./alien --once` no Linux
ou `./alien.exe --once` no Windows.
