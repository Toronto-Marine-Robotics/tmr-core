# Build Instructions

## Prerequisites

- **C++20** compiler (GCC 11+, Clang 14+)
- **CMake** 3.10+
- **OpenGL** 4.3+
- **SDL2** development libraries
- **FreeType** development libraries
- **OpenMP** (optional, for multi-threading)

### Installing dependencies (Ubuntu / Debian)

```bash
sudo apt install build-essential cmake libsdl2-dev libfreetype-dev \
     libgl1-mesa-dev libomp-dev
```

### Installing dependencies (macOS)

```bash
brew install cmake sdl2 freetype
```

## Build

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/Toronto-Marine-Robotics/tmr-core.git
cd tmr-core

# Configure
cmake -S tmr-simulator -B build

# Build
cmake --build build -j$(nproc)
```

## Run

From the build directory:

```bash
./build/tmr-simulator
```

## Applying Stonefish patches

The simulator depends on a few local patches to Stonefish (path separator in
`GetFullPath`, resizable window flag). These are applied automatically when
the submodule is first checked out, but after updating the submodule you can
re-apply them:

```bash
git -C tmr-simulator/stonefish checkout 09208f9
git -C tmr-simulator/stonefish apply ../patches/stonefish-local.patch
```
