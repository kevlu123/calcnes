# CalcNES

A Casio fx9860gii calculator port of the NES emulator from https://github.com/ObaraEmmanuel/NES/.

![demo](readme/demo.png)

# Input

| Calc | NES |
| --- | --- |
| Up | Up |
| Down | Down |
| Left | Left |
| Right | Right |
| F1 | B |
| F2 | A |
| F3 | Select |
| F4 | Start |

| Calc | Function |
| --- | --- |
| F6 | Toggle Colour |
| Exe | Exit |

# Building

```bash
python build.py --cpp
```

# Select a ROM (max ~45kb) to Embed

```bash
python select_rom.py <.nes file>
python build.py --cpp
```
