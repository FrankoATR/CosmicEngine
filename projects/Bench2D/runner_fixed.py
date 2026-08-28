# -*- coding: utf-8 -*-
"""Re-runs the scenarios affected by the two engine fixes (symmetric quadtree
notification, catch-up step cap) into results_fixed/. Same protocol as runner.py."""
import os
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
EXE2D = os.path.join(ROOT, "bin", "Bench2D", "windows", "Release", "Bench2D.exe")
EXE2D_IGPU = os.path.join(ROOT, "bin", "Bench2D", "windows", "Release", "Bench2D_igpu.exe")
EXE3D = os.path.join(ROOT, "bin", "Bench3D", "windows", "Release", "Bench3D.exe")

results = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), "results_fixed"))
os.makedirs(results, exist_ok=True)
REPS = 3
runs = []


def add(exe, name, **env):
    e = {k: str(v) for k, v in env.items()}
    e.setdefault("BENCH_WARMUP", "2")
    e.setdefault("BENCH_MEASURE", "5")
    runs.append((exe, name, e))


# pair test, both structures
for struct in ("grid", "quadtree"):
    add(EXE2D, "pairtest_%s" % struct, BENCH_MODE="pairtest", BENCH_STRUCT=struct)

# collision phase with the symmetric quadtree/octree (cost + notifications)
for exe, tag in ((EXE2D, "2d"), (EXE3D, "3d")):
    for n in (1000, 2000, 4000):
        for r in range(REPS):
            add(exe, "collision_%s_quadtree_n%d_r%d" % (tag, n, r), BENCH_MODE="collision", BENCH_N=n,
                BENCH_STRUCT="quadtree", BENCH_SEED=42 + r)

# full scene with the catch-up cap, both GPUs
for exe, tag in ((EXE2D, "scene"), (EXE2D_IGPU, "igpu_scene")):
    for n in (2000, 5000, 10000):
        for r in range(REPS):
            add(exe, "%s_n%d_r%d" % (tag, n, r), BENCH_MODE="scene", BENCH_N=n, BENCH_STRUCT="grid", BENCH_SEED=42 + r)

print("runs:", len(runs), "-> results in", results)
t_all = time.time()
failed = []
for i, (exe, name, env) in enumerate(runs, 1):
    out = os.path.join(results, name + ".csv")
    if os.path.exists(out):
        continue
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
    print("[%3d/%3d] %-36s %5.1fs %s" % (i, len(runs), name, time.time() - t0, "ok" if ok else "FAILED"), flush=True)
    if not ok:
        failed.append(name)
    time.sleep(0.5)
print("done in %.1f min, failed: %s" % ((time.time() - t_all) / 60.0, failed or "none"))
