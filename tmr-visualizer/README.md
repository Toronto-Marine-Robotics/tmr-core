# tmr-visualizer

Visualization tool for the TMR AUV simulator. Subscribes to telemetry data via Zenoh and displays it in Rerun.

## Setup

```bash
pip3 install -r requirements.txt
```

## Usage

### Start the viewer

```bash
python viewer.py
```

This will:
1. Open a Rerun viewer window
2. Subscribe to Zenoh topics (`auv/dvl`, `auv/depth`)
3. Display DVL scalar plots and depth camera feed

### Save recording to file

```bash
python viewer.py --save recording.rrd
```

### Headless mode (no GUI)

```bash
python viewer.py --headless --save recording.rrd
```

## Architecture

```
tmr-simulator (C++)  --Zenoh-->  tmr-visualizer (Python)  --gRPC-->  Rerun Viewer
   publishes:                       subscribes & logs:
   - auv/dvl (binary)               - DVL scalars (Scalar)
   - auv/depth (binary)             - Depth image (DepthImage)
```

## Zenoh Topics

| Topic | Format | Description |
|-------|--------|-------------|
| `auv/dvl` | `<d4fB` (24 bytes) | timestamp + velocity(x,y,z) + altitude + status |
| `auv/depth` | `<dII + int16[]` | timestamp + width + height + depth in mm (int16) |
