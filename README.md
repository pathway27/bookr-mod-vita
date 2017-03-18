## bookr-mod-vita

A document reader for the PSVita ported from the amalgamation of the PSP application bookr and its' various [forks](https://github.com/pathway27/bookr-mod-vita#forks).  
You can find some [notes here](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md).

## Building

### For VITA (on *nix)

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
git clone --recursive https://github.com/pathway27/bookr-mod-vita
./build.sh # Creates a build folder with vpk
# Install .vpk

# For devs - replace with your VITA ftp ip (assumes vpk was installed once)
export PSVITAIP=192.168.1.5
cd build && make send
```

### For Mac

```sh
# This is just for devel.
# Currently on OSX El Capitan 10.11.5
git clone --recursive https://github.com/pathway27/bookr-mod-vita
# install ext/SOIL (there's a readme)
brew install glfw GLEW glm
make -f Makefile.macglfw
./bookr.glut
```

### For PSP

```
TODO
```

## Installation

### PSVITA

Use the .vpk to install.

## CJK PDF Support

Considering Noto fonts.

## Forks

**[Original bookr.](https://sourceforge.net/projects/bookr/)**  
Carlos Carrasco Martinez (carloscm at gmail dot com),  
Christian Payeur (christian dot payeur at gmail dot com),  
Nguyen Chi Tam (nguyenchitam at gmail dot com)  

**[bookr-mod](https://code.google.com/archive/p/bookr-mod/)**  
2525275 @ gmail

**bookr-hbs - HomebrewStore Development Team**  
**ozbookr - by Oleg Zhuk (zhukoo @ gmail)**  
**gtzampanaskis/bookr**

## Credits

The original and forks above.  

- Team Molecule (yifan, xyzz, DaveeFTW, xerpi)
- TheFlow
- libvita2d - xerpi
- [learnopengl.com](learnopengl.com)
- peeps on #vitasdk on freenode
- people that read


All the VITA hacking/dev scene.

## License

Uses the same license as bookr, GNU GPLv2+, see [LICENSE](https://github.com/pathway27/bookr-mod-vita/blob/master/LICENSE).  
Original sources at respective tags of this repository.
