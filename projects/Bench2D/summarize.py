# -*- coding: utf-8 -*-
"""Aggregate the benchmark CSVs into summary.json (one entry per scenario group)."""
import glob
import io
import json
import os
import re
import statistics as st
import sys

results = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), "results")


def parse(path):
    meta, rows = {}, {}
    for line in io.open(path, encoding="utf-8"):
        line = line.strip()
        if line.startswith("#"):
            for kv in line[1:].split(","):
                if "=" in kv:
                    k, v = kv.split("=", 1)
                    meta[k.strip()] = v.strip()
            continue
        if line.startswith("kind,") or not line:
            continue
        kind, value, extra = line.split(",", 2)
        rows.setdefault(kind, []).append((float(value), extra))
    return meta, rows


def pct(vals, p):
    s = sorted(vals)
    if not s:
        return None
    k = (len(s) - 1) * p
    f, c = int(k), min(int(k) + 1, len(s) - 1)
    return s[f] + (s[c] - s[f]) * (k - f)


groups = {}
for path in sorted(glob.glob(os.path.join(results, "*.csv"))):
    name = os.path.basename(path)[:-4]
    group = re.sub(r"_r\d+$", "", name)
    groups.setdefault(group, []).append(parse(path))

summary = {}
for group, runs in sorted(groups.items()):
    meta0 = runs[0][0]
    entry = {"runs": len(runs), "mode": meta0.get("mode"), "n": int(meta0.get("n", 0)),
             "struct": meta0.get("struct"), "config": meta0.get("config"),
             "cell": int(meta0.get("cell", 0)), "depth": int(meta0.get("depth", 0)),
             "renderer": meta0.get("renderer"), "gl": meta0.get("gl")}
    # frame time (draw-to-draw)
    fm = [[v for v, _ in r.get("frame_ms", [])] for _, r in runs]
    fm = [x for x in fm if x]
    if fm:
        entry["frame_ms_median"] = st.median(st.median(x) for x in fm)
        entry["frame_ms_p95"] = st.median(pct(x, 0.95) for x in fm)
        entry["frame_ms_mean"] = st.mean(st.mean(x) for x in fm)
        entry["fps_mean"] = st.mean(len(x) / (sum(x) / 1000.0) for x in fm)
        entry["frames_per_run"] = st.mean(len(x) for x in fm)
    # collision phase
    tk = [r.get("tick_us", []) for _, r in runs]
    tk = [x for x in tk if x]
    if tk:
        entry["tick_us_median"] = st.median(st.median(v for v, _ in x) for x in tk)
        entry["tick_us_p95"] = st.median(pct([v for v, _ in x], 0.95) for x in tk)
        entry["pairs_per_tick"] = st.mean(st.mean(float(e) for _, e in x) for x in tk)
    # memory (steady samples)
    for key in ("mem_ws_bytes", "mem_private_bytes"):
        vals = [v for _, r in runs for v, e in r.get(key, []) if e == "steady"]
        base = [v for _, r in runs for v, e in r.get(key, []) if e == "baseline"]
        if vals:
            entry[key.replace("_bytes", "_mb_steady")] = st.mean(vals) / 1048576.0
        if base:
            entry[key.replace("_bytes", "_mb_baseline")] = st.mean(base) / 1048576.0
    # startup
    su = [v for _, r in runs for v, _e in r.get("startup_ms", []) if v > 0]
    if su and meta0.get("mode") == "startup":
        entry["startup_ms_mean"] = st.mean(su)
        entry["startup_ms_sd"] = st.pstdev(su)
        gl = [v for _, r in runs for v, _e in r.get("glfw_ms", []) if v > 0]
        entry["glfw_to_first_frame_ms_mean"] = st.mean(gl)
    # load
    ld = {}
    for _, r in runs:
        for v, e in r.get("load_ms", []):
            ld.setdefault(e, []).append(v)
    if ld:
        entry["load_ms"] = {k: {"mean": st.mean(v), "sd": st.pstdev(v)} for k, v in ld.items()}
        # by texture size
        bysize = {}
        for k, v in ld.items():
            m = re.match(r"noise_(\d+)_", k)
            if m:
                bysize.setdefault(int(m.group(1)), []).extend(v)
        entry["load_ms_by_size"] = {str(k): {"mean": st.mean(v), "sd": st.pstdev(v), "n": len(v)} for k, v in sorted(bysize.items())}
    # ticks/frames
    entry["ticks"] = st.mean(int(m.get("ticks", 0)) for m, _ in runs)
    summary[group] = entry

out = os.path.join(os.path.dirname(os.path.abspath(results)), "summary" + os.path.basename(os.path.abspath(results)).replace("results", "") + ".json")
io.open(out, "w", encoding="utf-8").write(json.dumps(summary, indent=1))
print("groups:", len(summary), "->", out)

# compact console view
def fmt(v):
    return "-" if v is None else ("%.2f" % v if isinstance(v, float) else str(v))
for g, e in summary.items():
    line = "%-30s" % g
    for k in ("frame_ms_median", "frame_ms_p95", "fps_mean", "tick_us_median", "tick_us_p95", "pairs_per_tick",
              "mem_private_mb_steady", "startup_ms_mean"):
        if k in e:
            line += " %s=%s" % (k.replace("_median", "").replace("_mean", ""), fmt(e[k]))
    print(line)
