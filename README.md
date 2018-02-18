## bookr-mod-vita

A document reader for the PSVita ported from the amalgamation of the PSP application bookr and its' various [forks](https://github.com/pathway27/bookr-mod-vita#source-and-forks).  
You can find some [notes here](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md).

## Building

### For VITA (on *nix)

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
git clone --recursive https://github.com/pathway27/bookr-mod-vita
# run pngquant on all your png images if not done already
./build.sh # Creates a build folder with vpk
# Install .vpk

# For devs - replace with your VITA ftp ip (assumes vpk was installed once)
export PSVITAIP=IP-HERE
cd build && make send
```

See [BUILDING.md](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md) for desktop dev. version.

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


## Credits

- [Original and Forks](https://github.com/pathway27/bookr-mod-vita/blob/master/forks.md)
- Team Molecule (yifan, xyzz, DaveeFTW, xerpi)
- TheFlow
- libvita2d - xerpi
- [learnopengl.com](learnopengl.com)
- peeps on #vitasdk on freenode
- people that read

All of the VITA hacking/dev. scene.

## License

Licensed under GNU GPLv3+, see [LICENSE](https://github.com/pathway27/bookr-mod-vita/blob/master/LICENSE).  
Third party libraries have their own licenses can be found in their own directories under `ext`.
