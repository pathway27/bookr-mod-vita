# Motivations

Learning C++  
Programming the Vita  
Porting experience  
Graphics Programming


Note: I am a web developer professionally so besides my uni c++ i haven't used it much.

# Random Notes

## VITA Hardware

 CPU: ARM Cortex-A9 MPCore (32-bit armv7-a, 4 cores) 2GHz (NEON SIMD)  
 RAM: PowerVR SGX543MP4+ (512MB)  
 GPU: 128MB (4 cores)  
DISP: 960x544 (16:9) OLED multi-touch @ 220ppi

PSP   480x272 (16:9)

Compass?

## Directory Stucture

The original bookr had all it's c files outside, i've tried to arrange them in folders so as not to get
overwhelmed.

```
  data              - should have all images and external resources, should also subdivide into vita/psp
  ext               - external libraries used statically by bookr
  sce_sys           - vita resources, should refactor into data/vita
  src               - main directory for c++ source files
    filetypes       - classes for specific filetypes for books
    graphics        - has main rendering, texture classes and shaders (eventually)
```

## Licensing and Copyright

Porting an open source project is hard to know all the legal terms.  
So bookr uses the GNU GPLv2+ which requires 

- Include Original (all in this repo)
- Disclose Source (this repo)
- Include Copyright (Included to original files, modified files also have my copyright; is this correct?)
- State Changes (commits on this repo)
- Include License (LICENSE in this repo)

So for the things i change i'm adding my copright and a notice of modification before the GPL notice.  
[Some](http://stackoverflow.com/questions/11670368/gpl-copyright-notice-when-contributing-new-file) [links](http://softwareengineering.stackexchange.com/questions/157968/how-to-manage-a-copyright-notice-in-an-open-source-project)


## Macro definitions

```C
#ifdef __vita__
```

Checking for preprocessor macro variables on your tool chain
arm-vita-eabi-gcc -dM -E - < /dev/null | grep -i VITA

that's how i found __vita__

## versioning based on git tags

I read the cmake tutorial which goes through setting up
variables like version through an internal environment variables
I then found a stackoverflow for the exact thing, see CMakeLists.txt:32:38

## Using Visual Studio Code

http://stackoverflow.com/questions/37522462/visual-studio-code-includepath
link macos specific libs from /Frameworks.../ to /usr/local/include for intellisense.
```sh
ln -s /System/Library/Frameworks/OpenGL.framework/Headers/ /usr/local/include/OpenGL
```

## Going from GLUT to GLFW

On macOS 10.9+ i get deprecation warnings for the system's GLUT, so swapping to GLFW (newest and more supported)
```sh
... 17 other warnings
src/graphics/fzscreenglut.cpp:183:2: warning: 'glutMainLoop' is deprecated: first deprecated in macOS 10.9
      [-Wdeprecated-declarations]
        glutMainLoop();
        ^
/usr/local/include/GLUT/glut.h:438:22: note: 'glutMainLoop' has been explicitly marked deprecated here
extern void APIENTRY glutMainLoop(void) OPENGL_DEPRECATED(10_0, 10_9);
```

## C++11

The codebase is using more of the C subset than C++, maybe i can make it cleaner and learn the C++11 features too.  
e.g. FZScreen is only using static functions to do everything, and look so many static global variables!


### Initalization

```c++
int val = 5; //copy init
int val(5);  //direct init
int val{5};  //uniform init <--- C++11
```

### Double buffering

Two variables in ram; front and back buffer.  
Front is the one shown to you on the screen; back is the only one you can draw to.  
Swap them to show the new drawn one, but while loops seems to go so fast it read buttons twice.


### General GUI/Game loops

So it seems like most applications share the same paradigm of

- initalize display
- while (not exiting app)
  - render/draw the "state" of the app (start draw, swap buffers, stop draw)
  - check user input

- deinitalize display

### OpenGL

Looks like bookr used the old fixed-pipeline version of OpenGL which has been depricated.  
Replacing it with a good programmable-pipeline abstraction feels hard.  
Got a cool shader class so far. 

### MuPDF

The general idea seems to be to read the file and store it in a pixmap buffer.
Then put the pixmap buffer into a texture which you can render.

### Bitwise Operations

- "|=" - OR a |= b is a = a | b
- "&=" - AND a &= b is a = a & b


## Style

1. Use spaces instead of tabs
2. 4 Spaces for normal indentation
3. 2 Spaces for preprocessor indentation when required.

### Cmake Special Flags

```
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make

same for Debug
```