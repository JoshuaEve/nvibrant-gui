# Building from source

Building nVibrant GUI requires Qt 6.4 or newer, CMake, Ninja, and a compiler
with C++20 support. You will also need
[nvibrant](https://github.com/Tremeschin/nvibrant) to actually change your
display settings.

## Arch Linux

Install the build tools and Qt:

```sh
sudo pacman -S --needed base-devel cmake ninja qt6-base
```

If you do not already have `nvibrant`, it is available from the AUR as
`nvibrant-bin`.

## Build

From the project directory, run:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The finished program will be at `build/nvibrant-gui`. You can run it without
installing it:

```sh
./build/nvibrant-gui
```

If CMake cannot find Qt, make sure the Qt 6 development package for your Linux
distribution is installed.
