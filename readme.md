# Sirtet
A simple block dropping game created with SDL2


### Controls

#### Menu

##### `←` Modify option - previous
##### `→` Modify option - next
##### `↑` Previous menu option
##### `↓` Next menu option
##### `Enter` Select current option

#### Game

##### `←` Move left 
##### `→` Move right
##### `hold ↑` Speed up block 
##### `↓` Rotate block
##### `p` Pause


### Building from source
#### Linux

##### Prerequisites

Using your favorite package manager, install SDL libraries
* SDL2
* SDL2_ttf
* SDL2_mixer


```bash
$ apt install libsdl2-dev
$ apt install libsdl2-mixer-dev
$ apt install libsdl2-ttf-dev
```

With the project's root directory as your current working directory, the 
following will build the main executable in the root directory.

```bash
make build_exe
```

Optionally, there are some build targets used for profiling and identifying
memory issues using valgrind.
```bash
$ apt install valgrind
```

Which can then be run with

```bash
make run_profile
```

Logs will be saved in /logs/valgrind.log


#### Windows

##### Prerequisites

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

##### Building

With the prerequisites added and environment variables set, the following
command should build the game's executable in the source folder:

```powershell

mingw32-make -f build_exe

```


##### Testing 
#### Linux

* [EWENIT](https://github.com/The-Slick-Nick/c-testsuite) (for testing)

* Valgrind
```bash
```





