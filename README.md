# Camera_01 — Wire-Frame Jack Camera Calibration

An interactive OpenCV tool for manually calibrating a camera by aligning a 3D wire-frame jack model to a photograph. This is a direct, visual approach to estimating the camera perspective matrix without a checkerboard target.

## Project Goal

This tool is the first step in a shoe last measurement and feedback system. The broader goal is to:

1. **Calibrate the camera** against a physical wire-frame jack placed near the shoe last.
2. **Capture last cross-sections** — using the calibrated camera to backproject image points to world coordinates and extract the last profile at any slice.
3. **Close the feedback loop** — compare measured cross-sections to a target last design and guide a shaping workbench to cut or adjust the last until it matches.

The end result is a camera-guided process for tuning a 3D-printed or machined shoe last to a precise fit specification.

## Concept

A 3D wire-frame jack (an L-shaped bracket with known real-world dimensions) is projected onto a photograph using a perspective matrix. The user interactively adjusts the camera parameters — pan, rotate, zoom — until the projected jack overlay matches the physical jack visible in the image. Once aligned, **P = Pint * Pext is the solution** — the full camera matrix is known directly from the alignment. No least-squares solve is required.

The `6` key / DLT code builds the A-matrix from the jack vertices and their image projections, but this is a verification tool only, not part of the calibration solution.

## Controls

| Key | Action |
|-----|--------|
| `p` | Pan — middle-click to set start, drag, release, `s` to save / `space` to cancel |
| `r` | Rotate — middle-click center, middle-click radius, drag to rotate, `s` to save / `space` to cancel |
| `2` | Arcball rotate — drag with middle button, `s` to save / `space` to cancel |
| `z` | Zoom (translate along Z) — mouse gesture, `s` to save / `space` to cancel |
| `f` | Zoom (scale focal length) — mouse gesture, `s` to save / `space` to cancel |
| `w` | Write perspective matrices to `pMats.xml` |
| `4` | Read perspective matrices from `pMats.xml` |
| `6` | Read jack points / compute DLT equations |
| `q` | Quit |

`space` cancels any active sub-mode and returns to the main menu without saving.

## Build

Requires OpenCV 4.

```bash
make camera_cal_init.exe
```

## Run

```bash
./camera_cal_init.exe
```

The image path is set in `image_data_s::load()` in `camera_cal_init.cpp`. By default it looks for `IMG_20251119_105708393.jpg` in the working directory.

## Output

- `pMats.xml` — saved camera intrinsic and extrinsic matrices
- `Camera_outFile.m` — matrices exported in Octave/MATLAB format
- `cal_points.xml` / `cal_data.txt` — collected image/object point correspondences for DLT

## Files

| File | Description |
|------|-------------|
| `camera_cal_init.cpp` | Main program, all state machines, image/jack/pan/rotate/zoom logic |
| `camera_cal.h` | Struct definitions for all subsystems |
| `camera_cal.cpp` | Additional calibration code |
| `terminal_server.c/h` | ANSI terminal positioning utilities (`rc()`, `clr()`) |
| `Camera_Wire_Frame_Calibration_Design.odg` | Design drawing of the wire-frame jack |
| `dlt.m` / `DLT.m` | Octave/MATLAB scripts for DLT solution |
