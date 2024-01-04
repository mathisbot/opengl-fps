<a name="readme-top"></a>

<br />
<div align="center">
    <img src="bin/assets/images/icon.webp" alt="Logo" width="128" height="128">

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
        <li><a href="#about-sdl2">About SDL2</a></li>
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
    <li>
      <a href="#usage">Usage</a>
      <ul>
        <li><a href="#gameplay">Gameplay</a></li>
        <li><a href="#bindings">Bindings</a></li>
      </ul>
    </li>
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

The aim of this project is to create a simple FPS game, using no game engine. This allows for a low-level approach to game design, and therefore very thorough optimization.

In this case, I chose the C language along with only one very low-level library: SDL2 (in order to use OpenGL).
This means that rendering methods are close to nil, and that I had to start from scratch (although SDL and OpenGL can take some of the work out of the equation).

Although the context of the project resembles that of the 1990s, the game's content should resemble a modern FPS with simplified graphics. The game should therefore include familiar mechanics.

<p align="right">(<a href="#readme-top">Up</a>)</p>

### About SDL2
<a name="about-sdl2"></a>

#### Context
The first question to ask is: how do you display elements on screen? As the display is connected directly to the GPU, you need to ask the graphics drivers politely.
This is usually done via standardized languages called specifications. Their implementation is the responsibility of the card manufacturer.

The best-known are DirectX, Vulkan and OpenGL. Although Vulkan is the most recent and was specifically designed for optimization, taking into account recent technological advances, OpenGL remains the most widely used, open-source and, above all, the easiest to learn.

> OpenGL® is the most widely adopted 2D and 3D graphics API in the industry, bringing thousands of applications to a wide variety of computer platforms. It is window-system and operating-system independent as well as network-transparent. OpenGL enables developers of software for PC, workstation, and supercomputing hardware to create high-performance, visually compelling graphics software application

on *khronos.org*

#### SDL

> Simple DirectMedia Layer is a cross-platform development library designed to provide low level access to audio, keyboard, mouse, joystick, and graphics hardware via OpenGL and Direct3D.

on *libsdl.org*

As OpenGL is a set of specifications and not a library, I had to choose a library that would allow me to create a context around it, like opening a window.
Among the many choices available (GLFW, GLUT, SFML, ...), one stood out for its characteristics:

- SDL2 is renowned for its lightweight design, which ensures efficient resource utilization without compromising performance.

- One of the key advantages of SDL2 is its provision of low-level access to graphics, but also audio drivers, and input devices.

- SDL2's cross-platform compatibility makes it easy to seamlessly operate on various operating systems, including Windows, macOS, and Linux.

Overall, I think SDL2 is best suited to video game design.

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Built with
<a name="built-with"></a>

* [C][c-url]
* [cglm][cglm-url] (0.9.2)
* [GLEW][glew-url] (2.1.0)
* [SDL2][sdl-url] (2.28.5)
* [SDL_mixer][sdl_mixer-url] (2.6.3)

#### Quick note

cglm stands for OpenGL Mathematics (glm) for C. Its is a highly optimized 2D/3D math library. cglm provides lot of utils to help math operations to be fast and quick to write. The functions implemented take advantage of SSE2 and vectorization.

SDL_mixer is not part of the default SDL library. However, it is an official SDL extension promoted on the SDL website.
It is also practically indispensable when developing a game, as it allows several sounds to be played at the same time, which is impossible with SDL alone.

GLEW stands for OpenGL Extension Wrangler. It is an extension that loads OpenGL function headers :
as stated before, OpenGL's implementation is the responsibility of the card manufacturer. GLEW takes care of retrieving and normalizing function names.

> GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform. OpenGL core and extension functionality is exposed in a single header file.

on *glew.curseforge.net*

<p align="right">(<a href="#readme-top">Up</a>)</p>


## Getting started
<a name="getting-started"></a>

#### Try the game
If you just want to try out the game, simply download the `release.zip` file.

Please note that the game files given in this zip file are only compatible with 64-bit Windows systems. In the future, I plan to compile the project for other widespread systems (Linux and 32-bit, maybe macOS).

For the moment, no interface or menu has been implemented, so that there's no way to change  key bindings from within the game.

Current bindings are detailed in <a href="#usage">Usage</a>.

#### Edit code

Otherwise, follow these steps:

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Prerequisites
<a name="prerequisites"></a>

* C : Any compiler will do. I personally used GCC 13.2.0 through [WinLibs][winlibs-url].
* SDL2 : Download the development (`devel`) version of SDL2. I chose the `mingw` one.
Inside the zip file are the 32- and 64-bit SDL library files. Choose the one that suits your system.
* SDL_mixer : As with SDL, download the development (`devel`) version of SDL_mixer. I chose the `mingw` one.
Inside the zip file are the 32- and 64-bit SDL library files. Choose the one that suits your system.
* GLEW : Download GLEW binaries on their website.
* cglm : Download latest release on GitHub (it is a source code).

### Installation
<a name="installation"></a>

1. Clone the repository
  ```sh
  git clone https://github.com/Dozer35/retro-fps.git
  ```

  or download code as Zip.

2. Add the `include` and `lib` directories of the SDL library files you downloaded earlier to `src`. Also put the content of the `bin` folder (`bin/SDL2.dll` for Windows) inside of `bin`
  The project tree should then contain :
  - `src/include/SDL2/*.h`
  - `src/lib/*.a`
  - `bin/SDL2.dll`

    Follow the same procedure for SDL_mixer and GLEW.

    As for cglm, cglm provides different ways of including it. The easiest one, and the one I chose, is to use inline functions. This means that all you have to do is add `cglm/*.h` to `include/cglm` and just include the header.

    I didn't include these files in the repository because I didn't write them myself. Besides, the repository would be unnecessarily heavier.
3. WINDOWS - Compile the project with :
  ```sh
    gcc -Wall -I src/include -L src/lib -o bin/retro_fps src/main.c $(Get-ChildItem -Recurse -Path src/include -Filter \"*.c\").FullName -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -lglew32
  ```

  UNIX - A Makefile is available. Alternatively, compile the project with :

  ```
    gcc -Wall -I src/include -L src/lib -o bin/retro_fps src/main.c $(find src/include -name "*.c") -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -lglew32
  ```
  A compiled file will then be generated as `bin/retro_fps` or `bin/retro_fps.exe`, depending on your OS.


<p align="right">(<a href="#readme-top">Up</a>)</p>

## Usage
<a name="usage"></a>

### Gameplay
<a name="gameplay"></a>

In its current state, the game only allows you to move freely in a black space. The player can run and look around.
The stage also contains brick cubes and a movable light source.

This is a placeholder for the level.

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Bindings
<a name="bindings"></a>

It is currently not possible to change default bindings without modifying `src/main.c` and recompiling the whole code.
In the future, the player should be able to modify its bindings in-game.

By default, the bindings are :
- H - Forward
- B - Reverse
- V - Left
- N - Right
- Space - Jump
- Left click - Shoot (Not fully implemented)

- Esc - Pause the game
- F1 - Close the game

Other bindings are set in the game files, but they are not used yet.

<p align="right">(<a href="#readme-top">Up</a>)</p>

## Product
<a name="product"></a>

This section organizes the development process, defining a minimal viable product and a roadmap for development.

### MVP
<a name="mvp"></a>

Achieving the Minimum Viable Product would already be a success. Further development is not guaranteed and may depend on factors such as development time, will or other external factors (free time, ...).

MVP should be a classic, straightforward FPS with a retro feel:
- First-person camera
- Retro 3D rendering
- Guns
- Enemies
- Sound effects

Overall, the game MUST be playable and enjoyable.

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Roadmap
<a name="roadmap"></a>

This roadmap helps organizing development and may be subject to change.

- [X] Opening a window
- [ ] First Person Camera
- [ ] Level rendering
- [ ] PAUSE - Need to move to OpenGL 4.6.0 Core
- [ ] Gun
- [ ] Audio
- [ ] Collisions
- [ ] Health packs and ammo
- [ ] Enemies
- [ ] MVP
- [ ] Level editor
- [ ] Better lights
- [ ] New functionalities

<p align="right">(<a href="#readme-top">Up</a>)</p>

### Known Bugs
<a name="known-bugs"></a>

Some bugs appear during development, but no direct solution is found. Here is a list of known bugs for which no solution has yet been found. By the way, if you manage to find a bug during a game, don't hesitate to share it in Issues.
- Light currently passes through objects. This is not a bug, this is a feature not implemented yet.

<p align="right">(<a href="#readme-top">Up</a>)</p>


[c-url]: https://fr.wikipedia.org/wiki/C_(langage)
[cglm-url]: https://github.com/recp/cglm
[glew-url]: https://glew.sourceforge.net/
[sdl-url]: https://www.libsdl.org/
[sdl_mixer-url]: https://github.com/libsdl-org/SDL_mixer
[winlibs-url]: https://winlibs.com/#download-release