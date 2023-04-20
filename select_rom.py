import sys
import os
from pathlib import Path

if len(sys.argv) != 2 or "-h" in sys.argv or "--help" in sys.argv:
    print("Usage: python select_rom.py <file>")
    exit(1)
    
os.chdir(Path(__file__).parent)

rom = sys.argv[1]
with open(rom, "rb") as f:
    data = f.read()
print(rom, "\t\t", len(data), "bytes")

chunks = ["extern const char FILE_DATA [] = {"]
for i, b in enumerate(data):
    if i % 64 == 0:
        chunks.append("\n")
    chunks.append(hex(b))
    chunks.append(",")
chunks.append("\n};")

with open("src/data.c", "w") as f:
    f.write("".join(chunks))
