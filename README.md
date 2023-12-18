<a name="readme-top"></a>

<br />
<div align="center">
    <img src="https://github.com/othneildrew/Best-README-Template/raw/master/images/logo.png" alt="Logo" width="80" height="80">

  <h3 align="center">Retro FPS</h3>

  <p align="center">
    This project aims to recreate a retro FPS shooter in C using only low-level libraries like SDL.
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

This project aims to recreate a retro FPS shooter, like the first Doom, using the C langage using only low-level libraries like SDL.

This project is motivated by the knowledge it can provide about the low-level operation of a computer.

<p align="right">(<a href="#readme-top">Up</a>)</p>


### Built with
<a name="built-with"></a>

* [C][c-url]
* [SDL][sdl-url]

<p align="right">(<a href="#readme-top">Up</a>)</p>


## Getting started
<a name="getting-started"></a>

If you just want to try out the game, simply download the `release.zip` file.

Otherwise, follow these steps:

### Prerequisites
<a name="prerequisites"></a>

* C : Any compiler will do. I personally used GCC 13.2.0
* SDL : Download the development (`devel`) version of SDL2. I used the `mingw` version.
Inside the archive are the 32- and 64-bit SDL library files. Choose the one that suits your system.

### Installation
<a name="installation"></a>

1. Clone the repository
  ```sh
  git clone https://github.com/Dozer35/retro-fps.git
  ```
2. Add the `include/SDL2` and `lib` directories of the SDL library files you downloaded earlier to `src`. Also put `bin/SDL2.dll` inside of `src`
  The project tree should then contain :
  - `src/include/SDL2/*.h.`
  - `src/lib/*.a`
  - `src/SDL2.dll`
3. Compile the project with `gcc -I src/include -L src/lib -o src/main src/main.c -lmingw32 -lSDL2main -lSDL2`.
  You will then find the compiled file inside of `src`.


<p align="right">(<a href="#readme-top">Up</a>)</p>

## Usage
<a name="usage"></a>

In its current state, the game can only display a blank window.

<p align="right">(<a href="#readme-top">Up</a>)</p>

## Product
<a name="product"></a>

### MVP
<a name="mvp"></a>

MVP must be a standard retro FPS:
- First-person camera
- Guns to shoot
- Enemies
- Retro 3D rendering

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Roadmap
<a name="roadmap"></a>

- [X] Opening a window
- [ ] Having an MVP
- [ ] 

<p align="right">(<a href="#readme-top">Up</a>)</p>


[c-url]: https://fr.wikipedia.org/wiki/C_(langage)
[sdl-url]: https://www.libsdl.org/