# Installing nVibrant GUI

The easiest option on Arch Linux is to install the AUR package once it is
available. If you have downloaded the source instead, follow the instructions
in [BUILD.md](BUILD.md) first.

## Install a local build

After building the project, install it system-wide with:

```sh
sudo cmake --install build
```

This installs the application, desktop entry, icon, and license under `/usr/local`.
You can then launch **nVibrant GUI** from your application menu or run:

```sh
nvibrant-gui
```

`nvibrant` must also be installed and available in your `PATH`.

## Start automatically

Open **File → Settings** in nVibrant GUI and enable **Start on login**. The app
will create an autostart entry in your user configuration directory.

## Uninstall

From the same source and build directory, run:

```sh
sudo cmake --build build --target uninstall
```

Your settings and presets are kept in `~/.config/nvibrant-gui`. Remove that
directory manually if you also want to delete your personal configuration.
