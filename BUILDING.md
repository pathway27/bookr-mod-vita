### For Windows

You have to compile mupdf with Visual Studio and add the link directories first.

#### Visual Studio Code

Requirements:
 - Visual Studio since it has the compiler.
 - Cmake and Cmake Tools extensions.

1. Git clone this.
2. Select the `x86` kit after it scans it.
3. Ctrl + Shift + P -> Build Target (bookr)

#### Visual Studio 2019

### For Mac

```sh
# This is just for devel.
# Currently on OSX El Capitan 10.11.5
git clone --recursive https://github.com/pathway27/bookr-mod-vita
# install ext/SOIL (there's a readme)
brew install glm glew glfw freetype
mkdir -p desktop/Debug && cd $_
cmake ../..
make -j8
./bookr-mod-vita
```
