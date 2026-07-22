# Installation and packaging

Install with `sudo cmake --install build`. Remove installed files with `sudo cmake --build build --target uninstall`.

For a local Arch package, first create or download the versioned source archive referenced by `PKGBUILD`, then run `makepkg -si`. Before publishing to the AUR, publish the matching `v1.0.0` GitHub release, update its SHA-256 checksum, and generate `.SRCINFO` with `makepkg --printsrcinfo > .SRCINFO`.

The runtime dependency is named `nvibrant`; the current AUR `nvibrant-bin` package provides it. Settings can generate `~/.config/autostart/nvibrant-gui.desktop`.
