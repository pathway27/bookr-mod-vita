# bookr-mod-vita

A document reader for the PSVita ported from the amalgamation of the PSP application bookr and its' various forks.

# Building

## For VITA (on *nix)

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
$ ./build.sh # Creates a build folder with vpk
# Install .vpk

# For devs - replace with your VITA ftp ip (assumes vpk was installed once)
export PSVITAIP=192.168.1.5
cd build && make send
```

## For Mac

```sh
# This is just for devel.
# Currently on OSX El Capitan 10.11.5
brew install glfw GLEW
make -f Makefile.macglfw
./bookr.glut
```

## For PSP

```
TODO
```

# Installation

## PSVITA

Use the .vpk to install.

# CJK PDF Support

Considering Noto fonts.

# Credits

**Original bookr**

Carlos Carrasco Martinez (carloscm at gmail dot com),  
Christian Payeur (christian dot payeur at gmail dot com),  
Nguyen Chi Tam (nguyenchitam at gmail dot com)  

**bookr-mod - 2525275 @ gmail**

https://code.google.com/archive/p/bookr-mod/

**bookr-hbs - HomebrewStore Development Team**  
**ozbookr - by Oleg Zhuk (zhukoo@gmail.com)**  
**gtzampanaskis/bookr**

## book-mod-vita

All the VITA hacking/dev scene

- Team Molecule (yifan, xyzz, DaveeFTW, xerpi)
- TheFlow

Software Used

- libvita2d - xerpi

pathway27 (Sree) - ME! - This port to VITA

## License

Uses the same license as bookr, GNU GPLv2+, see LICENSE.  
Original sources at respective orphan branches of this repository.
