## bookr-mod-vita

A document reader for the PSVita ported from the PSP application bookr-mod, canonically based on the [HBS](https://github.com/pathway27/bookr-mod-vita/tree/bookr-hbs) version.  
You can find some [notes here](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md).

## Installation and Usage

Use the .vpk to install.

```
Default Controls:

Menu
X - Choose Item
O - Cancel
Triangle - Parent Folder in FileManger
Directional Arrows - Select File
Start - Show/Hide Menu

In-Book Default Controls
Triangle - Next Page
Square - Previous Page
O - Previous 10 Pages
X - Next 10 Pages
Up/Down D-Pad - Pan Up and Down
Left Analog Stick - Free Pan with Bounds
Start - Show/Hide Menu
```

## Building


### For VITA (on *nix)


MuPDF is built using cmake automatically.

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
git clone --recursive https://github.com/pathway27/bookr-mod-vita
# run pngquant on all your png images if not done already
mkdir -p vita/Release && cd $_
cmake ..
make -j8
# Install .vpk

# For devs - replace with your VITA ftp ip (assumes vpk was installed once)
export PSVITAIP=IP-HERE
make send
```

### For Switch

NOTE: You may need to build MuPDF manually.

```sh
# Setup devkitpro development evironment: 
dkp-pacman -S switch-dev
dkp-pacman -S switch-freetype switch-mesa switch-glad switch-glm

mkdir -p switch/Release && cd $_
cmake ..
make

```

See [BUILDING.md](https://github.com/pathway27/bookr-mod-vita/blob/master/BUILDING.md) for OpenGL version.


## Thanks

- [Original and Forks](https://github.com/pathway27/bookr-mod-vita/blob/master/forks.md)
- Team Molecule
- VITA Hacking/Dev. Scene
- libvita2d - xerpi
- [learnopengl.com](learnopengl.com)
- People on vitasdk/henkaku on freenode and discord
- People that read


## License

Licensed under GNU GPLv3+, see [LICENSE.md](https://github.com/pathway27/bookr-mod-vita/blob/master/LICENSE).  
Third party libraries have their own licenses can be found in their own directories under `ext`.
