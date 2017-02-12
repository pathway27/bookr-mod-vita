# bookr-mod-vita

A document reader for the PSVita ported from the amalgamation of the PSP application bookr and its' various forks.

# Building

## For VITA

```sh
# Setup VITA development evironment: https://henkaku.xyz/developer/
$ ./build.sh # Creates a build folder with vpk

# For devs - replace with your VITA ftp ip
export PSVITAIP=192.168.1.5
cd build && make send
```

## For Mac

```sh
# Currently on OSX El Capitan 10.11.5
make -f Makefile.macglut
./bookr.glut
```

## For PSP

```
# Setup VITA development evironment: https://henkaku.xyz/developer/
$ ./build.sh # Creates a build folder with vpk

# For devs - replace with your VITA ftp ip
$ export PSVITAIP=192.168.1.5
$ cd build && make send
```

TODO

# Installation

## PSVITA

Use the .vpk to install.

# CJK PDF Support

# Credits

## bookr

Carlos Carrasco Martinez and Edward Choh

MuPDF library - By Artifex Software, Inc. licensed under the AFPL license.
Visit: http://ghostpdf.com/new.html

## bookr-mod

https://code.google.com/archive/p/bookr-mod/

## bookr-hbs



## ozbookr


## gtzampanaskis/bookr

## book-mod-vita

All the VITA hacking/dev scene
    - Team Molecule (yifan, xyzz, DaveeFTW, xerpi)... need a library or a standardized list of devs....
    - TheFlow
    -

pathway27 (Sree) - ME! - This port to VITA

## License

Uses the same license as bookr, GNU GPLv2+, see LICENSE.
Original sources at respective orphan branches of this repository.