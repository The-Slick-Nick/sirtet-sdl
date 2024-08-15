# Sirtet
A simple block dropping game created with SDL2


### Building from source
#### Linux

#### Windows

* Ensure msys32 & gcc installed
* Ensure SDL libraries are downloaded, and headers are saved to
  C:/msys64/mingw32/include
  (or if different, copy/modify makefile and tweak to your own setup)

```powershell

mingw32-make -f Makefile.win

```

### Build Requirements


* SDL
```bash
$ sudo apt install libsdl2-2.0-0
```
* SDL_ttf extension
```bash
$ sudo apt install libsdl2-ttf-dev
```

* [EWENIT](https://github.com/The-Slick-Nick/c-testsuite) (for testing)

* Valgrind
```bash
$ sudo apt install valgrind
```



