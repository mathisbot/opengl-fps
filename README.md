<a name="readme-top"></a>

<br />
<div align="center">
    <img src="https://github.com/othneildrew/Best-README-Template/raw/master/images/logo.png" alt="Logo" width="80" height="80">

  <h3 align="center">Retro FPS</h3>

  <p align="center">
    This project aims to recreate a retro FPS in C using only one low-level library.
  </p>
</div>

<details>
  <summary>Table of contents</summary>
  <ol>
    <li>
      <a href="#about">About</a>
      <ul>
        <li><a href="#built-with">Built with</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installing</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li>
      <a href="#product">Product</a>
      <ul>
        <li><a href="#mvp">MVP</a></li>
        <li><a href="#roadmap">Roadmap</a></li>
      </ul>
    </li>
  </ol>
</details>


## About
<a name="about"></a>

This project aims to recreate a retro/doomlike FPS, using the C langage and only one, low-level, library : SDL2.
It should evolve over time, once the main project is finished.

<p align="right">(<a href="#readme-top">Up</a>)</p>


### Built with
<a name="built-with"></a>

* [C][c-url]
* [SDL][sdl-url]

<p align="right">(<a href="#readme-top">Up</a>)</p>


## Getting started
<a name="getting-started"></a>

If you just want to try out the game, simply download the `release.zip` file.
However, there's currently no way to change the key bindings, and I'm left-handed.

Otherwise, follow these steps:

### Prerequisites
<a name="prerequisites"></a>

* C : Any compiler will do. I personally used GCC 13.2.0 through [WinLibs][winlibs-url].
* SDL : Download the development (`devel`) version of SDL2. I chose the `mingw` one.
Inside the zip file are the 32- and 64-bit SDL library files. Choose the one that suits your system.

### Installation
<a name="installation"></a>

1. Clone the repository
  ```sh
  git clone https://github.com/Dozer35/retro-fps.git
  ```
2. Add the `include/SDL2` and `lib` directories of the SDL library files you downloaded earlier to `src`. Also put `bin/SDL2.dll` inside of `src`
  The project tree should then contain :
  - `src/include/SDL2/*.h`
  - `src/lib/*.a`
  - `src/SDL2.dll`
3. WINDOWS - Compile the project with :
  ```sh
    gcc -Wall -I src/include -L src/lib -o src/retro_fps src/main.c $(Get-ChildItem -Recurse -Path src/include -Filter \"*.c\").FullName -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
  ```
  UNIX - A Makefile is available. Alternatively, compile the project with :
  ```
    gcc -Wall -I src/include -L src/lib -o src/retro_fps src/main.c $(find src/include -name "*.c") -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
  ```
  A compiled file will then be generated as `src/retro_fps` or `src/retro_fps.exe`.


<p align="right">(<a href="#readme-top">Up</a>)</p>

## Usage
<a name="usage"></a>

Currenlty, the game features a fully functional first person camera.
It can also read files to import and render a level.
Move with HVBN (will be changed) and jump with Space.

<p align="right">(<a href="#readme-top">Up</a>)</p>

## Product
<a name="product"></a>

### MVP
<a name="mvp"></a>

MVP must be a standard retro FPS:
- First-person camera
- Retro 3D rendering
- Guns
- Enemies

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Roadmap
<a name="roadmap"></a>

- [X] Opening a window
- [X] Working First Person Camera
- [ ] Level rendering
- [ ] Collisions
- [ ] Guns and enemies
- [ ] Polishing (Level finish, bug fixes, ...)
- [ ] MVP
- [ ] Better lights
- [ ] Level editor
- [ ] New functionalities

<p align="right">(<a href="#readme-top">Up</a>)</p>


[c-url]: https://fr.wikipedia.org/wiki/C_(langage)
[sdl-url]: https://www.libsdl.org/
[winlibs-url]: https://winlibs.com/#download-release