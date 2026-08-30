#!/usr/bin/env bash

cd ..

# raylib/raygui/stb are header-only to us, so meson never auto-fetches them.
meson subprojects download

meson setup build-windows --cross-file windows-x86_64.ini
