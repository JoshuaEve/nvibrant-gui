# Building

```sh
sudo pacman -S qt6-base qt6-tools cmake ninja gcc
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Qt 6.4 or newer and a C++20 compiler are required. Install `nvibrant` separately before running the GUI.

For a clean release check:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
DESTDIR="$PWD/stage" cmake --install build
desktop-file-validate packaging/nvibrant-gui.desktop
```
