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

