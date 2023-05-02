OpenFX uses [cmake](https://cmake.org) and [conan](https://conan.io) to build.

_Build Command:_
```sh
mkdir build && cd build
conan install .. --build missing
cmake ..
cmake --build .
```

Some useful parameters you can pass to `cmake` with `-Dparameter=value`:

- `OFX_EXAMPLE_PLUGINS`: set to `ON` or `OFF` to enable/disable building
  of example plugins, defaults to `OFF`

_Linux dependencies:_
```sh
sudo apt install pkg-config libgl-dev
```
