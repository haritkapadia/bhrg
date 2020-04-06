# Bullet Hell Rhythm Game

This is a bullet hell rhythm game. In the futute, most actions will be synced to accompanying background music.

## NO READING, ONLY DEMO!!!

Make sure you've downloaded/installed GNU Make, SDL2, SDL_mixer, and SDL_ttf.

Are you on Windows?

```
copy C:\Windows\Fonts\calibri.ttf .
winmake mapmaker
winmake bhrg
build\bhrg region_test.map region_test.time
```

Are you on Linux? Sorry, you need to do a little work. Edit `src/mapmaker`, change `"calibri.ttf"` to the font of your choice. Then you can compile as normal.

```
./make mapmaker
./make bhrg
build/bhrg region_test.map region_test.time
```

## Building the project

Building uses GNU Make.
Building is OS dependent, you may need to modify the Makefile for your OS. The Windows Makefile I use is `windows.Makefile`. The Linux Makefile I use is `linux.Makefile`.

To build the project, use `make`.

```
$ make --directory=build --makefile=linux.Makefile mapmaker
$ make --directory=build --makefile=linux.Makefile bhrg
$ build/mapmaker make_your_own.map make_your_own.time
$ build/bhrg make_your_own.map make_your_own.time
```

`make` can be replaced by the following platform-dependent commands.

```
$ ./make bhrg
C:\bhrg> winmake bhrg
```

## Libraries

Window and graphics management is done using [SDL2](https://www.libsdl.org/).

This project also requires [SDL\_mixer](https://www.libsdl.org/projects/SDL_mixer/) and [SDL\_ttf](https://www.libsdl.org/projects/SDL_ttf/). Expect to require [SDL\_image](https://www.libsdl.org/projects/SDL_image/) in the future.

## Music

The music in the repo is "Chill or Be Chilled" by "The Polish Ambassador | Nitty Scott", found [here](https://freemusicarchive.org/music/The_Polish_Ambassador__Nitty_Scott).
