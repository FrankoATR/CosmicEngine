# -*- coding: utf-8 -*-
"""Generate synthetic benchmark assets (pure Python PNG writer, no PIL)."""
import os
import shutil
import struct
import zlib

ROOT = r"c:\PERSONAL\CosmicEngine"
OUT = os.path.join(ROOT, "projects", "Bench2D", "assets")


def png_chunk(tag, data):
    c = struct.pack(">I", len(data)) + tag + data
    return c + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)


def write_png(path, w, h, rgba_rows):
    raw = b"".join(b"\x00" + r for r in rgba_rows)
    png = (b"\x89PNG\r\n\x1a\n"
           + png_chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0))
           + png_chunk(b"IDAT", zlib.compress(raw, 6))
           + png_chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(png)


def noise_rows(w, h):
    return [os.urandom(w * 4) for _ in range(h)]


def sprite_rows(w, h):
    rows = []
    cx, cy, r = w / 2.0, h / 2.0, w / 2.0 - 2
    for y in range(h):
        row = bytearray()
        for x in range(w):
            d = ((x + 0.5 - cx) ** 2 + (y + 0.5 - cy) ** 2) ** 0.5
            if d <= r:
                row += bytes((60 + int(180 * x / w), 140, 220 - int(150 * y / h), 255))
            else:
                row += b"\x00\x00\x00\x00"
        rows.append(bytes(row))
    return rows


os.makedirs(os.path.join(OUT, "textures"), exist_ok=True)
os.makedirs(os.path.join(OUT, "fonts"), exist_ok=True)

write_png(os.path.join(OUT, "textures", "sprite.png"), 64, 64, sprite_rows(64, 64))
for s in (256, 512, 1024, 2048):
    for i in range(3):
        write_png(os.path.join(OUT, "textures", "noise_%d_%d.png" % (s, i)), s, s, noise_rows(s, s))
shutil.copy(os.path.join(ROOT, "default", "font", "ThaleahFat.ttf"), os.path.join(OUT, "fonts", "ThaleahFat.ttf"))

for root, _, files in os.walk(OUT):
    for f in sorted(files):
        p = os.path.join(root, f)
        print("%8.1f KB  %s" % (os.path.getsize(p) / 1024.0, os.path.relpath(p, OUT)))
