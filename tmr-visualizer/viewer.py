#!/usr/bin/env python3
"""
tmr-visualizer — Rerun viewer + Zenoh subscriber for tmr-simulator telemetry.

Usage:
    python viewer.py [--headless] [--save recording.rrd]

The viewer will:
1. Spawn a Rerun viewer window (or run headless)
2. Subscribe to Zenoh topics published by tmr-simulator
3. Display DVL data, depth images, and color camera feed
"""

import rerun as rr
import rerun.blueprint as rrb
import zenoh
import struct
import time
import argparse
import traceback
import numpy as np

ZENOH_DVL_TOPIC = "auv/dvl"
ZENOH_DEPTH_TOPIC = "auv/depth"
ZENOH_COLOR_TOPIC = "auv/color"
ZENOH_IMU_TOPIC = "auv/imu"

DEPTH_SCALE = 1000.0

DVL_FORMAT = "<d4fB3x"
DVL_SIZE = struct.calcsize(DVL_FORMAT)

IMU_FORMAT = "<d9f"
IMU_SIZE = struct.calcsize(IMU_FORMAT)

CAM_WIDTH = 1280
CAM_HEIGHT = 800
CAM_FOCAL = 500.0


def setup_blueprint():
    pinhole = rr.archetypes.Pinhole(
        resolution=[CAM_WIDTH, CAM_HEIGHT],
        focal_length=CAM_FOCAL,
    )
    rr.log("world/camera", pinhole)

    blueprint = rrb.Blueprint(
        rrb.Vertical(
            rrb.Horizontal(
                rrb.Spatial2DView(name="Color", origin="world/camera"),
                rrb.Spatial2DView(name="Depth", origin="world/camera/depth"),
                column_shares=[1, 1],
            ),
            rrb.Horizontal(
                rrb.TimeSeriesView(name="DVL", origin="auv/dvl"),
                rrb.TimeSeriesView(name="IMU", origin="auv/imu"),
                column_shares=[1, 1],
            ),
            row_shares=[3, 1],
        ),
    )
    rr.send_blueprint(blueprint)


def on_dvl(sample):
    try:
        print(f"[tmr-visualizer] Received DVL sample {len(sample.payload)} bytes")
        payload = bytes(sample.payload)
        if len(payload) < DVL_SIZE:
            return
        unpacked = struct.unpack(DVL_FORMAT, payload[:DVL_SIZE])
        ts, vx, vy, vz, alt, status = unpacked
        rr.set_time("sim_time", duration=ts)
        rr.log("auv/dvl/velocity_x", rr.Scalars(vx))
        rr.log("auv/dvl/velocity_y", rr.Scalars(vy))
        rr.log("auv/dvl/velocity_z", rr.Scalars(vz))
        rr.log("auv/dvl/altitude", rr.Scalars(alt))
        rr.log("auv/dvl/status", rr.Scalars(float(status)))
    except Exception:
        traceback.print_exc()


def on_depth(sample):
    try:
        payload = bytes(sample.payload)
        if len(payload) < 16:
            return
        ts, w, h = struct.unpack("<dII", payload[:16])
        num_pixels = w * h
        depth_bytes = payload[16:]
        expected = num_pixels * 2
        if len(depth_bytes) < expected:
            return
        depth = np.frombuffer(depth_bytes[:expected], dtype=np.int16).reshape((h, w))
        depth_meters = depth.astype(np.float32) / DEPTH_SCALE
        depth_meters[depth == 0] = 0.0
        rr.set_time("sim_time", duration=ts)
        rr.log(
            "world/camera/depth",
            rr.DepthImage(depth_meters, meter=1.0),
        )
    except Exception:
        traceback.print_exc()


def on_color(sample):
    try:
        payload = bytes(sample.payload)
        if len(payload) < 16:
            return
        ts, w, h = struct.unpack("<dII", payload[:16])
        num_pixels = w * h
        rgba_bytes = payload[16:]
        expected = num_pixels * 4
        if len(rgba_bytes) < expected:
            return
        image = np.frombuffer(rgba_bytes[:expected], dtype=np.uint8).reshape((h, w, 4))
        rr.set_time("sim_time", duration=ts)
        rr.log(
            "world/camera/image",
            rr.Image(image),
        )
    except Exception:
        traceback.print_exc()


def on_imu(sample):
    try:
        print(f"[tmr-visualizer] Received IMU sample {len(sample.payload)} bytes")
        payload = bytes(sample.payload)
        if len(payload) < IMU_SIZE:
            return
        unpacked = struct.unpack(IMU_FORMAT, payload[:IMU_SIZE])
        ts, roll, pitch, yaw, ang_vel_x, ang_vel_y, ang_vel_z, lin_acc_x, lin_acc_y, lin_acc_z = unpacked
        rr.set_time("sim_time", duration=ts)
        rr.log("auv/imu/roll", rr.Scalars(roll))
        rr.log("auv/imu/pitch", rr.Scalars(pitch))
        rr.log("auv/imu/yaw", rr.Scalars(yaw))
        rr.log("auv/imu/angular_velocity_x", rr.Scalars(ang_vel_x))
        rr.log("auv/imu/angular_velocity_y", rr.Scalars(ang_vel_y))
        rr.log("auv/imu/angular_velocity_z", rr.Scalars(ang_vel_z))
        rr.log("auv/imu/linear_acceleration_x", rr.Scalars(lin_acc_x))
        rr.log("auv/imu/linear_acceleration_y", rr.Scalars(lin_acc_y))
        rr.log("auv/imu/linear_acceleration_z", rr.Scalars(lin_acc_z))
    except Exception:
        traceback.print_exc()


def main():
    parser = argparse.ArgumentParser(description="TMR Visualizer")
    parser.add_argument("--headless", action="store_true", help="Run without GUI")
    parser.add_argument("--save", type=str, help="Save recording to .rrd file")
    args = parser.parse_args()

    rr.init("tmr-visualizer")

    if args.save:
        rr.save(args.save)
        print(f"[tmr-visualizer] Saving recording to {args.save}")
    elif args.headless:
        print("[tmr-visualizer] Running headless (no viewer window)")
    else:
        rr.spawn()

    setup_blueprint()

    z_session = zenoh.open(zenoh.Config())
    print("[tmr-visualizer] Zenoh session open")

    z_session.declare_subscriber(ZENOH_DVL_TOPIC, on_dvl)
    z_session.declare_subscriber(ZENOH_DEPTH_TOPIC, on_depth)
    z_session.declare_subscriber(ZENOH_COLOR_TOPIC, on_color)
    z_session.declare_subscriber(ZENOH_IMU_TOPIC, on_imu)

    print(f"[tmr-visualizer] Subscribed to:")
    print(f"  {ZENOH_DVL_TOPIC}")
    print(f"  {ZENOH_DEPTH_TOPIC}")
    print(f"  {ZENOH_COLOR_TOPIC}")
    print(f"  {ZENOH_IMU_TOPIC}")
    print("\nWaiting for data... Press Ctrl+C to exit.")

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[tmr-visualizer] Shutting down...")
    finally:
        z_session.close()


if __name__ == "__main__":
    main()
