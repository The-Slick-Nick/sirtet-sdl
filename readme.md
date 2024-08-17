# Sirtet
A simple block dropping game created with SDL2


## Controls

### Menu

#### `←` Modify option - previous
#### `→` Modify option - next
#### `↑` Previous menu option
#### `↓` Next menu option
#### `Enter` Select current option

### Game

#### `←` Move left 
#### `→` Move right
#### `hold ↑` Speed up block 
#### `↓` Rotate block
#### `p` Pause
#### `Space` Hard drop


## Building from source
### Linux

#### Prerequisites

Using your favorite package manager, install SDL libraries
* SDL2
* SDL2_ttf
* SDL2_mixer


```bash
$ sudo apt install libsdl2-dev
$ sudo apt install libsdl2-mixer-dev
$ sudo apt install libsdl2-ttf-dev
```

With the project's root directory as your current working directory, the 
following will build the main executable in the root directory.

```bash
make build_exe
```

Optionally, there are some build targets used for profiling and identifying
memory issues using valgrind.
```bash
$ sudo apt install valgrind
```

Which can then be run with

```bash
make run_profile
```

Logs will be saved in /logs/valgrind.log


### Windows

#### Prerequisites

* This project uses gcc to build for windows, requiring installation through
[MSYS2](https://msys64.org). Installation instructions can be found at the link provided. You will need both gcc and make. With the UCRT64 environment running, run:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-x86_64-make
```

* Update include path 
  * start
  * search environment variables
  * "Edit the system environment variables"
  * "Environment variables"
  * "Path" - add C:\msys64\ucrt64\bin (to make "mingw32-make" work)
    * Replace with your msys64 location as needed
  * 

```bash
pacman -S mingw-w64-x86_64-SDL2
pacman -S mingw-w64-x86_64-SDL2_ttf
pacman -S mingw-w64-x86_64-SDL2_mixer
```

#### Building

With the prerequisites added and environment variables set, the following
command should build the game's executable in the source folder:

```powershell

mingw32-make -f build_exe

```


#### Testing 

I've only direct build support for Linux here - while it is technically
possible build/compile EWENIT for windows and use it to hook into the static 
library file as for Linux, I have not yet done so myself and have no pre-built
build system set up for it.

### Linux

#### Dependencies

##### EWENIT
The testing I use for this project depends on [EWENIT](), a unit testing
library I've built for C. Install instructions can be found at the link
provided.

##### Valgrind
For extra coverage/certainty of performance, the unit testing build makefiles
have options for profiling tests with Valgrind:
```bash
$ sudo apt install valgrind
```

#### Running tests

To build and run the tests, one simply runs
```bash
$ make run_tests
```
Valgrind's logs will be saved to `/tests/logs`

## Credits
* Sounds from [freesound.org](https://freesound.org)
  * [Beep sounds](https://freesound.org/people/Kenneth_Cooney/sounds/463067/)
  * [Click and bump noises](https://freesound.org/people/rellable/sounds/537978/)
* Fonts from [fontsquirrel.com](https://www.fontsquirrel.com/)
  * [VT323](https://www.fontsquirrel.com/fonts/vt323)
  * [Lekton](https://www.fontsquirrel.com/fonts/lekton)

