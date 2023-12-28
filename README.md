<a name="readme-top"></a>

<br />
<div align="center">
    <img src="src/assets/images/icon.webp" alt="Logo" width="128" height="128">

  <h3 align="center">Retro FPS</h3>

  <p align="center">
    This project aims to recreate a simple FPS in C using only one low-level library.
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
        <li><a href="#known-bugs">Known bugs</a></li>
      </ul>
    </li>
  </ol>
</details>


## About
<a name="about"></a>

The aim of this project is to recreate a simple FPS game, using techniques from the 1990's that allows for advanced optimization. In this case, I chose the C language along with only one very low-level library: SDL2. This library talks to the operating system API to display what is requested on the screen. Its greatest advantage is its compatibility with a very large number of systems, as well as its lightness.
This means that rendering techniques are close to what was done before, such as in the first Doom game. However, the game should feature modern improvements such as movements in all 3 axis.

<p align="right">(<a href="#readme-top">Up</a>)</p>


### Built with
<a name="built-with"></a>

* [C][c-url]
* [SDL2][sdl-url]

<p align="right">(<a href="#readme-top">Up</a>)</p>


## Getting started
<a name="getting-started"></a>

If you just want to try out the game, simply download the `release.zip` file.

Currently, the game files contained in this directory are only compatible with 64-bit Windows systems. In the future, I plan to compile the project for other widespread systems (Linux and 32-bit).

For the moment, no menu has been implemented: there's no way to change your keys from within the game.
Furthermore, it is likely that default bindings won't be modifiable in the final version of the game.
Current bindings are detailed in <a href="#usage">Usage</a>.

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
2. Add the `include` and `lib` directories of the SDL library files you downloaded earlier to `src`. Also put `bin/SDL2.dll` inside of `src`
  The project tree should then contain :
  - `src/include/SDL2/*.h`
  - `src/lib/*.a`
  - `src/SDL2.dll`

    I didn't include these files in the repository because I didn't write them myself. Besides, the repository would be unnecessarily heavier.
3. WINDOWS - Compile the project with :
  ```sh
    gcc -Wall -I src/include -L src/lib -o src/retro_fps src/main.c $(Get-ChildItem -Recurse -Path src/include -Filter \"*.c\").FullName -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
  ```

  UNIX - A Makefile is available. Alternatively, compile the project with :

  ```
    gcc -Wall -I src/include -L src/lib -o src/retro_fps src/main.c $(find src/include -name "*.c") -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
  ```
  A compiled file will then be generated as `src/retro_fps` or `src/retro_fps.exe`, depending on your OS.


<p align="right">(<a href="#readme-top">Up</a>)</p>

## Usage
<a name="usage"></a>

In its current state, the game contains only a black background with colorful cubes and a brick wall as a placeholder for the level. The player can move around freely, with a fully functioning first-person camera.

By default, the bindings are :
- H - Forward
- B - Reverse
- V - Left
- N - Right
- Space - Jump
- Left click - Shoot

Other bindings are set in the game files, but they are not used yet.

<p align="right">(<a href="#readme-top">Up</a>)</p>

## Product
<a name="product"></a>

This section organizes the development process, defining a minimum product and a roadmap for additions.

### MVP
<a name="mvp"></a>

Achieving the Minimum Viable Product would already be a success. Further development is not guaranteed and may depend on factors such as development time, will or other external factors (free time, ...).

MVP should be a classic, straightforward FPS with a retro feel:
- Full first-person camera
- Retro 3D rendering
- Guns
- Enemies

Overall, the game MUST be playable and enjoyable.

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Roadmap
<a name="roadmap"></a>

This roadmap helps organizing development and may be subject to change.

- [X] Opening a window
- [X] First Person Camera
- [X] Level rendering
- [ ] Collisions
- [ ] Guns and enemies
- [ ] Health packs and ammo
- [ ] MVP
- [ ] Level editor
- [ ] Better lights
- [ ] New functionalities

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Known Bugs
<a name="known-bugs"></a>

Some bugs appear during development, but no direct solution is found. Here is a list of known bugs for which no solution has yet been found. By the way, if you manage to find a bug during a game, don't hesitate to share it in Issues.
- For some graphics drivers, presumably for recent graphics cards, launching the game in full screen mode takes longer than expected: the screen remains dark (while the game runs correctly in the background).
- Fullscreen mode seems to reset refresh rate to 60Hz when Vsync is enabled. This limits FPS count to 60, even on fast monitors.

<p align="right">(<a href="#readme-top">Up</a>)</p>


[c-url]: https://fr.wikipedia.org/wiki/C_(langage)
[sdl-url]: https://www.libsdl.org/
[winlibs-url]: https://winlibs.com/#download-release