# -*- coding: utf-8 -*-
"""Print figures for the CONIA paper from summary.json (single-column, 300 dpi)."""
import io
import json
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, ScalarFormatter

HERE = os.path.dirname(os.path.abspath(__file__))
S = json.load(io.open(os.path.join(HERE, "summary.json"), encoding="utf-8"))
OUT = os.path.join(HERE, "figures")
os.makedirs(OUT, exist_ok=True)

# validated categorical slots (light surface), chrome ink
C1, C2, C3, C4 = "#2a78d6", "#eb6834", "#1baf7a", "#eda100"
INK, INK2, MUTED, GRID, AXIS = "#0b0b0b", "#52514e", "#898781", "#e1e0d9", "#c3c2b7"

plt.rcParams.update({
    "font.family": "sans-serif", "font.size": 8, "axes.labelsize": 8, "axes.titlesize": 8,
    "xtick.labelsize": 7, "ytick.labelsize": 7, "legend.fontsize": 7,
    "axes.edgecolor": AXIS, "axes.labelcolor": INK2, "xtick.color": INK2, "ytick.color": INK2,
    "axes.grid": True, "grid.color": GRID, "grid.linewidth": 0.6, "axes.axisbelow": True,
    "axes.spines.top": False, "axes.spines.right": False, "legend.frameon": False,
    "figure.dpi": 300, "savefig.dpi": 300, "savefig.bbox": "tight", "savefig.pad_inches": 0.02,
})
W, H = 3.4, 2.35  # inches, one column


def series(prefix, key, ns, suffix=""):
    xs, ys = [], []
    for n in ns:
        e = S.get("%s_n%d%s" % (prefix, n, suffix))
        if e and key in e:
            xs.append(n)
            ys.append(e[key])
    return xs, ys


def plain(ax, axis="both"):
    for a in ([ax.xaxis, ax.yaxis] if axis == "both" else [ax.xaxis if axis == "x" else ax.yaxis]):
        a.set_major_formatter(FuncFormatter(lambda v, _: ("%g" % v)))
        a.set_minor_formatter(FuncFormatter(lambda v, _: ""))


def label_end(ax, xs, ys, text, color, dy=0):
    ax.annotate(text, (xs[-1], ys[-1]), xytext=(4, dy), textcoords="offset points",
                fontsize=7, color=INK, va="center", ha="left")


def refline(ax, y, text):
    ax.axhline(y, color=MUTED, linewidth=0.8, linestyle=(0, (4, 3)))
    ax.annotate(text, (ax.get_xlim()[0], y), xytext=(3, 3), textcoords="offset points",
                fontsize=6.5, color=INK2, ha="left", va="bottom")


LINE = dict(linewidth=1.4, marker="o", markersize=3.2, markeredgewidth=0.8, markeredgecolor="white")

# ---------- Fig. 1: rendering scaling, RTX vs iGPU ---------------------------
NS_R = [100, 500, 1000, 2000, 5000, 10000, 20000]
fig, ax = plt.subplots(figsize=(W, H))
x1, y1 = series("render", "frame_ms_median", NS_R)
x2, y2 = series("igpu_render", "frame_ms_median", NS_R)
ax.plot(x1, y1, color=C1, **LINE)
ax.plot(x2, y2, color=C2, **LINE)
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlim(80, 26000); ax.set_ylim(0.25, 60)
plain(ax)
refline(ax, 16.67, "16,7 ms (60 FPS)")
label_end(ax, x1, y1, "GPU dedicada", C1, dy=4)
label_end(ax, x2, y2, "GPU integrada", C2, dy=-4)
ax.legend(["GPU dedicada (RTX 5060)", "GPU integrada (Intel)"], loc="lower right")
ax.set_xlabel("Sprites por fotograma")
ax.set_ylabel("Tiempo de fotograma (ms, mediana)")
fig.savefig(os.path.join(OUT, "fig_render.png")); plt.close(fig)

# ---------- Fig. 2: collision phase scaling, 2D/3D, grid/quadtree ---------
NS_C = [100, 250, 500, 1000, 2000, 4000, 8000]
fig, ax = plt.subplots(figsize=(W, H))
cfg = [("collision_2d_grid", "Rejilla 2D", C1, "o"), ("collision_2d_quadtree", "Quadtree 2D", C2, "s"),
       ("collision_3d_grid", "Rejilla 3D", C3, "^"), ("collision_3d_quadtree", "Octree 3D", C4, "D")]
handles = []
for prefix, name, color, mk in cfg:
    xs, ys = series(prefix, "tick_us_median", NS_C)
    kw = dict(LINE); kw["marker"] = mk
    h, = ax.plot(xs, [y / 1000.0 for y in ys], color=color, **kw)
    handles.append(h)
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlim(80, 12000); ax.set_ylim(0.01, 60)
plain(ax)
refline(ax, 6.94, "6,94 ms (paso fijo a 144 Hz)")
ax.legend(handles, [c[1] for c in cfg], loc="lower right", ncol=1)
ax.set_xlabel("Cuerpos de colisión (N)")
ax.set_ylabel("Fase de colisiones (ms por paso, mediana)")
fig.savefig(os.path.join(OUT, "fig_collision.png")); plt.close(fig)

# ---------- Fig. 3: structure parameters at N = 2000 ------------------------
fig, (a1, a2) = plt.subplots(1, 2, figsize=(W, H * 0.9), sharey=False)
cells = [32, 64, 128, 256]
yc = [S["param_grid_cell%d" % c]["tick_us_median"] / 1000.0 for c in cells]
a1.plot(range(len(cells)), yc, color=C1, **LINE)
a1.set_xticks(range(len(cells))); a1.set_xticklabels([str(c) for c in cells])
a1.set_xlabel("Celda (px)"); a1.set_ylabel("ms por paso, N = 2000")
a1.set_ylim(0, max(yc) * 1.25)
for i, v in enumerate(yc):
    a1.annotate("%.2f" % v, (i, v), xytext=(0, 5), textcoords="offset points", ha="center", fontsize=6.5, color=INK)
depths = [4, 5, 6, 8]
yd = [S["param_quadtree_depth%d" % d]["tick_us_median"] / 1000.0 for d in depths]
a2.plot(range(len(depths)), yd, color=C2, **LINE)
a2.set_xticks(range(len(depths))); a2.set_xticklabels([str(d) for d in depths])
a2.set_xlabel("Profundidad máx.")
a2.set_ylim(0, max(yc) * 1.25)
for i, v in enumerate(yd):
    a2.annotate("%.2f" % v, (i, v), xytext=(0, 5), textcoords="offset points", ha="center", fontsize=6.5, color=INK)
a1.set_title("Rejilla", color=INK2, loc="left"); a2.set_title("Quadtree", color=INK2, loc="left")
fig.tight_layout(w_pad=1.6)
fig.savefig(os.path.join(OUT, "fig_params.png")); plt.close(fig)

# ---------- Fig. 4: full scene (render + collisions), RTX vs iGPU -----------
NS_S = [100, 500, 1000, 2000, 5000, 10000]
fig, ax = plt.subplots(figsize=(W, H))
x1, y1 = series("scene", "frame_ms_median", NS_S)
x2, y2 = series("igpu_scene", "frame_ms_median", NS_S)
ax.plot(x1, y1, color=C1, **LINE)
ax.plot(x2, y2, color=C2, **LINE)
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlim(80, 13000); ax.set_ylim(0.2, 500)
plain(ax)
refline(ax, 16.67, "16,7 ms (60 FPS)")
ax.axvspan(3500, 13000, color=GRID, alpha=0.5, linewidth=0)
ax.annotate("bucle de paso fijo\nsaturado", (6500, 0.5), fontsize=6.5, color=INK2, ha="center", va="bottom")
ax.legend(["GPU dedicada (RTX 5060)", "GPU integrada (Intel)"], loc="upper left")
ax.set_xlabel("Cuerpos móviles con sprite (N)")
ax.set_ylabel("Tiempo de fotograma (ms, mediana)")
fig.savefig(os.path.join(OUT, "fig_scene.png")); plt.close(fig)

print("figures ->", OUT, sorted(os.listdir(OUT)))
