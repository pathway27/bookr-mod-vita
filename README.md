## bookr-mod-vita

A document reader for the PSVita ported from the amalgamation of the PSP application bookr and its' various [forks](https://github.com/pathway27/bookr-mod-vita#forks).  
You can find some [notes here](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md).

TODO:

- Custom Fonts
- MuPDF (HTML, PDF)

## Building

### For VITA (on *nix)

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
git clone --recursive https://github.com/pathway27/bookr-mod-vita
./build.sh # Creates a build folder with vpk
# run pngquant on all your png images if not done already
# Convert images used in app to binary: arm-vita-eabi-ld -r -b binary -o image.o image.png
# Install .vpk

# For devs - replace with your VITA ftp ip (assumes vpk was installed once)
export PSVITAIP=IP-HERE
cd build && make send
```

See [BUILDING.md](https://github.com/pathway27/bookr-mod-vita/blob/master/notes.md) for desktop dev. version.

## Installation

### PSVITA

Use the .vpk to install.

## Source and Forks

**[Original bookr](https://sourceforge.net/projects/bookr/)**

- Carlos Carrasco Martinez (carloscm at gmail dot com)
- Christian Payeur (christian dot payeur at gmail dot com)
- Nguyen Chi Tam (nguyenchitam at gmail dot com)

**[bookr-mod](https://code.google.com/archive/p/bookr-mod/) - 2525275 @ gmail **  
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

All of the VITA hacking/dev. scene.

## License

Licensed under GNU GPLv3+, see [LICENSE](https://github.com/pathway27/bookr-mod-vita/blob/master/LICENSE).  
Third party libraries have their own licenses can be found in their own directories under `ext`.
