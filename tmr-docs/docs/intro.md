# TMR Simulator

A vectored AUV (Autonomous Underwater Vehicle) simulation built on the [Stonefish](https://github.com/patrykcieslak/stonefish) physics engine.

## Overview

The simulator implements an 8-thruster vectored AUV in an underwater environment. The robot
has full 4-DOF control (surge, sway, heave, yaw) via a thruster allocation matrix that maps
body-frame forces to individual thruster setpoints.

### Tech stack

- **Stonefish v1.6** — marine robotics simulator with hydrodynamics, buoyancy, and ocean rendering
- **Bullet Physics** — rigid body dynamics with fluid interaction support
- **OpenGL 4.3** — real-time 3D rendering with ocean optics
- **SDL2** — window and input management

### Repository structure

```
tmr-core/
├── tmr-simulator/           # Simulator source code
│   ├── src/                 # Application code (MyApp, SimulationManager)
│   ├── data/                # Scenario files, meshes, textures
│   ├── stonefish/           # Stonefish physics engine (git submodule)
│   └── patches/             # Local patches for Stonefish
└── tmr-docs/                # Documentation site (Docusaurus)
```
