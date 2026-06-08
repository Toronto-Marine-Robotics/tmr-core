# Architecture

## Class overview

```
GraphicalSimulationApp (Stonefish)
        |
    MyApp
        |-- ProcessInputs()    ← called every step
        |-- DoHUD()            ← render GUI overlays
        |
SimulationManager (Stonefish)
        |
    MySimulationManager
        |-- BuildScenario()    ← parse scenario file, load robot
```

### MyApp

- Extends `sf::GraphicalSimulationApp`
- Handles keyboard input (SDL2) and GUI sliders (ImGUI via Stonefish)
- Computes thruster allocation via `ProcessInputs()`
- Provides `getThruster(name)` helper that looks up thrusters by prefixed name
  (e.g. `"VectoredAUV/thr_flb"`)

### MySimulationManager

- Extends `sf::SimulationManager`
- Loads the scenario via `ScenarioParser`
- Delegates diagnostic output from the parser to Stonefish's logging system

## Data flow

```
Input (keyboard / GUI sliders)
    |
    v
ProcessInputs()
    |-- Read commands (surge, sway, heave, yaw)
    |-- If all zero: set all thruster setpoints to 0
    |-- Else: compute allocation matrix → per-thruster setpoints
    |
    v
Thruster::setSetpoint(value)
    |
    v
Stonefish physics step (fluid dynamics, buoyancy, drag)
    |
    v
Updated AUV pose (rendered via OpenGL)
```

## Thruster naming

Actuators (thrusters) are stored with a robot name prefix. The `Robot::getActuator(name)`
method compares against the full stored name, so lookups must use the prefixed form:

```
Robot stores:  "VectoredAUV/thr_flb"
Lookup with:   "VectoredAUV/thr_flb"    ✓
Lookup with:   "thr_flb"                ✗
```
