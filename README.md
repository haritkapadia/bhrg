# Bullet Hell Rhythm Game

This is a bullet hell rhythm game. Most actions are synced to accompanying background music.

Example music is found in `data/music/`.

## Building the project

To build or run the project, simply use `make`.

```
$ make all
$ make run
```

Alternatively, one can execute `bhrg` to run the project.

## Libraries

Music analysis is done using [Essentia](https://essentia.upf.edu/). When compiling Essentia from source, be sure to include the [FFTW](http://www.fftw.org/) dependency.

Window and graphics management is done using [SDL2](https://www.libsdl.org/).

SDL2 also includes utilising [SDL2\_mixer](), [SDL2\_ttf](), and [SDL2\_image]().
