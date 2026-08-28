# -*- coding: utf-8 -*-
"""Runs the CosmicEngine benchmark matrix and collects one CSV per run.

Usage:  python runner.py [results_dir] [--quick]
Each run is a fresh process (cold managers, fresh memory), driven by env vars
understood by Bench::BenchScene.
"""
import os
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
EXE2D = os.path.join(ROOT, "bin", "Bench2D", "windows", "Release", "Bench2D.exe")
EXE3D = os.path.join(ROOT, "bin", "Bench3D", "windows", "Release", "Bench3D.exe")

results = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith("--") else os.path.join(os.path.dirname(__file__), "results"))
quick = "--quick" in sys.argv
os.makedirs(results, exist_ok=True)

REPS = 2 if quick else 3
WARMUP, MEASURE = ("1", "2") if quick else ("2", "5")

runs = []  # (exe, name, env)


def add(exe, name, **env):
    e = {k: str(v) for k, v in env.items()}
    e.setdefault("BENCH_WARMUP", WARMUP)
    e.setdefault("BENCH_MEASURE", MEASURE)
    runs.append((exe, name, e))


# 1. startup: process creation -> first frame
for r in range(5 if quick else 10):
    add(EXE2D, "startup_r%d" % r, BENCH_MODE="startup")

# 2. resource loading (12 textures + font), repeated
for r in range(5 if quick else 10):
    add(EXE2D, "load_r%d" % r, BENCH_MODE="load")

# 3. rendering scaling: N static sprites, no bodies
for n in ([500, 2000, 10000] if quick else [100, 500, 1000, 2000, 5000, 10000, 20000]):
    for r in range(REPS):
        add(EXE2D, "render_n%d_r%d" % (n, r), BENCH_MODE="render", BENCH_N=n, BENCH_SEED=42 + r)

# 4. collision phase scaling, 2D and 3D, grid vs quadtree
N_COL = [500, 2000, 8000] if quick else [100, 250, 500, 1000, 2000, 4000, 8000]
for exe, tag in ((EXE2D, "2d"), (EXE3D, "3d")):
    for struct in ("grid", "quadtree"):
        for n in N_COL:
            for r in range(REPS):
                add(exe, "collision_%s_%s_n%d_r%d" % (tag, struct, n, r),
                    BENCH_MODE="collision", BENCH_N=n, BENCH_STRUCT=struct, BENCH_SEED=42 + r)

# 5. structure parameters at fixed N=2000 (2D)
if not quick:
    for cell in (32, 64, 128, 256):
        for r in range(REPS):
            add(EXE2D, "param_grid_cell%d_r%d" % (cell, r), BENCH_MODE="collision", BENCH_N=2000,
                BENCH_STRUCT="grid", BENCH_CELL=cell, BENCH_SEED=42 + r)
    for depth in (4, 5, 6, 8):
        for r in range(REPS):
            add(EXE2D, "param_quadtree_depth%d_r%d" % (depth, r), BENCH_MODE="collision", BENCH_N=2000,
                BENCH_STRUCT="quadtree", BENCH_DEPTH=depth, BENCH_SEED=42 + r)

# 6. full scene: moving sprites with bodies (render + collisions), grid
for n in ([500, 2000, 5000] if quick else [100, 500, 1000, 2000, 5000, 10000]):
    for r in range(REPS):
        add(EXE2D, "scene_n%d_r%d" % (n, r), BENCH_MODE="scene", BENCH_N=n, BENCH_STRUCT="grid", BENCH_SEED=42 + r)

# 7. memory footprint vs N (sprites + bodies)
for n in ([0, 5000] if quick else [0, 1000, 5000, 10000, 20000]):
    for r in range(REPS):
        add(EXE2D, "memory_n%d_r%d" % (n, r), BENCH_MODE="memory", BENCH_N=n, BENCH_STRUCT="grid", BENCH_SEED=42 + r)

# 8. integrated-GPU variant (same binary without the dedicated-GPU hint): render and scene only
EXE2D_IGPU = os.path.join(os.path.dirname(EXE2D), "Bench2D_igpu.exe")
if os.path.exists(EXE2D_IGPU) and not quick:
    for n in [100, 500, 1000, 2000, 5000, 10000, 20000]:
        for r in range(REPS):
            add(EXE2D_IGPU, "igpu_render_n%d_r%d" % (n, r), BENCH_MODE="render", BENCH_N=n, BENCH_SEED=42 + r)
    for n in [100, 500, 1000, 2000, 5000, 10000]:
        for r in range(REPS):
            add(EXE2D_IGPU, "igpu_scene_n%d_r%d" % (n, r), BENCH_MODE="scene", BENCH_N=n, BENCH_STRUCT="grid", BENCH_SEED=42 + r)

print("runs:", len(runs), "-> results in", results)
t_all = time.time()
failed = []
for i, (exe, name, env) in enumerate(runs, 1):
    out = os.path.join(results, name + ".csv")
    if os.path.exists(out):
        continue  # resumable
    full_env = dict(os.environ)
    full_env.update(env)
    full_env["BENCH_OUT"] = out
    t0 = time.time()
    try:
        p = subprocess.run([exe], cwd=os.path.dirname(exe), env=full_env,
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=120)
        ok = p.returncode == 0 and os.path.exists(out)
    except subprocess.TimeoutExpired:
        ok = False
    print("[%3d/%3d] %-40s %5.1fs %s" % (i, len(runs), name, time.time() - t0, "ok" if ok else "FAILED"), flush=True)
    if not ok:
        failed.append(name)
    time.sleep(0.5)  # let the window close and the GPU settle

print("done in %.1f min, failed: %s" % ((time.time() - t_all) / 60.0, failed or "none"))
