# Controls

## Keyboard

| Key | Action |
|---|---|
| Arrow Up | Surge forward (+X) |
| Arrow Down | Surge backward (-X) |
| Arrow Left | Sway left (+Y) |
| Arrow Right | Sway right (-Y) |
| R | Heave up (+Z) |
| F | Heave down (-Z) |
| T | Yaw clockwise |
| G | Yaw counter-clockwise |
| Space | Stop (zero all thrusters) |
| H | Toggle HUD overlay |

## GUI sliders

When no keyboard keys are pressed, control falls back to sliders in the HUD:

- **Thrust Scale** — global multiplier (0.0 to 1.0)
- **Surge** — forward/backward (-1 to 1)
- **Sway** — lateral (-1 to 1)
- **Heave** — vertical (-1 to 1)
- **Yaw** — rotation (-1 to 1)

## Allocation matrix

Four body-frame commands (surge, sway, heave, yaw) are mapped to eight thrusters
using a signed allocation matrix. Each thruster's setpoint is:

```
sp[i] = scale * (alloc[i].surge * cmd_surge
               + alloc[i].sway   * cmd_sway
               + alloc[i].heave  * cmd_heave
               + alloc[i].yaw    * cmd_yaw)
```

The allocation signs encode the geometry of the vectored thruster layout:

| Thruster | Surge | Sway | Heave | Yaw |
|---|---|---|---|---|
| thr_flb | -1 | -1 | -1 | -1 |
| thr_frb | -1 | +1 | -1 | +1 |
| thr_blb | +1 | -1 | -1 | +1 |
| thr_brb | +1 | +1 | -1 | -1 |
| thr_flt | -1 | -1 | +1 | -1 |
| thr_frt | -1 | +1 | +1 | +1 |
| thr_blt | +1 | -1 | +1 | +1 |
| thr_brt | +1 | +1 | +1 | -1 |
