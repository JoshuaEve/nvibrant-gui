# nVibrant GUI

I wanted a simple way to adjust NVIDIA Digital Vibrance on Wayland without
reaching for the command line every time, so I built a Qt frontend for
[nvibrant](https://github.com/Tremeschin/nvibrant). It is primarily intended
for KDE Plasma and GNOME users with NVIDIA GPUs.

![nVibrant GUI showing per-monitor vibrance controls](docs/nVibrant-GUI-Screenshot.png)

## What it can do

- Show connected monitors as a visual layout instead of hiding them in a menu.
- Keep a separate vibrance setting for each monitor.
- Let you adjust several monitors before applying all pending changes together.
- Save and load presets.
- Reset every connected monitor on the selected GPU back to zero.
- Notice when monitors are connected or removed while the app is running.
- Run from the system tray and start automatically when you log in.
- Apply presets when configured processes start.
- Scale with larger fonts and high-DPI displays.

Select a monitor, choose a value, and press **Apply**. You can move between
monitors before applying; the app keeps those edits pending for you. Loading a
preset applies it immediately.

## A note about current values

`nvibrant` does not provide a way to read the current vibrance value from the
driver. Because of that, the value shown by the GUI is the last value it applied
during the current session. It starts at zero when the app opens.

The GUI sends a complete set of remembered port values whenever it applies a
change. This avoids changing one monitor and accidentally resetting another.
The supported range in the GUI is 0–1023. Although upstream also supports
negative values, this app currently leaves those out to keep the controls
focused on vibrance rather than desaturation.

## Requirements

- Linux with an NVIDIA GPU
- [nvibrant](https://github.com/Tremeschin/nvibrant) 1.2.x
- Qt 6.4 or newer

See [BUILD.md](BUILD.md) for build instructions and [INSTALL.md](INSTALL.md)
for installation and packaging notes.

Configuration is stored in `~/.config/nvibrant-gui/config.json`. Presets are
stored in `~/.config/nvibrant-gui/presets.json`.

## License

MIT License. See [LICENSE](LICENSE).
