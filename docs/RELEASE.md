# Building and Publishing a Quantum Release

## Prerequisites (Windows only)

| Tool | Where to get it |
|---|---|
| Qt 6 MinGW 64-bit | https://www.qt.io/download |
| Inno Setup 6 | https://jrsoftware.org/isinfo.php |
| Internet access | needed once to download the bundled Python package |

No Python installation is required on the build machine — the build script
downloads and stages a portable Python automatically.

## Steps

### 1. Build the executable

```powershell
.\build.ps1
```

Compiles `src/` → `build\main.exe` and copies Qt DLLs.

### 2. Build the installer

```powershell
.\installer\build-installer.ps1
```

This will:
1. Verify `build\main.exe` and all Qt DLLs are present.
2. Download the Python embeddable package (`python-3.12.x-embed-amd64.zip`).
3. Install `yfinance matplotlib pandas numpy scipy` into `installer\python_stage\`.
4. Compile `installer\Quantum.iss` with Inno Setup.
5. Write the final installer to `dist\QuantumSetup.exe`.

The Python staging step is cached — subsequent builds skip the download unless
you pass `-RebuildPython`.

Custom Qt or Inno Setup paths:
```powershell
.\installer\build-installer.ps1 -QtDir "C:\Qt\6.8.0\mingw_64" -InnoSetupDir "C:\InnoSetup6"
```

### 3. Test the installer

On a clean Windows machine (no Qt, no Python):

1. Run `dist\QuantumSetup.exe`.
2. Launch Quantum from the Start Menu shortcut.
3. Verify:
   - `load(AMZN)` — fetches data without error.
   - `display()` — renders and shows a price chart.
   - `compare(AAPL)` — renders a comparison chart.
   - `monte(AMZN+AAPL)` — completes a Monte Carlo simulation.
4. Uninstall via Windows Settings → Apps & Features; confirm shortcuts are gone.

### 4. Upload to GitHub Releases

1. Create a new Release on GitHub.
2. Attach `dist\QuantumSetup.exe` as the sole release asset.
3. Users download and run only `QuantumSetup.exe` — nothing else needed.

---

## What the installer bundles

| What | Where in install dir |
|---|---|
| `main.exe` | `\` |
| Qt6Core/Gui/Widgets DLLs | `\` |
| `platforms\qwindows.dll` | `platforms\` |
| Python 3.12 + all packages | `python\` |
| `scripts\*.py` | `scripts\` |
| `resources\` (icons + chart output) | `resources\` |
| `docs\`, `README.md` | `docs\`, `\` |
| `test_script.qtm` | `\` |

Install location: `%LOCALAPPDATA%\Quantum` (user-writable, no UAC required).
The app writes generated chart PNGs into `resources\`, which works because the
install is under `%LOCALAPPDATA%` rather than Program Files.

## Notes

- `main.exe` will use `python\python.exe` (the bundled runtime) automatically.
  It falls back to the system `python` only if the bundled one is absent.
- The installer icon (`SetupIconFile`) is currently disabled. Convert
  `resources\qst.png` to `resources\qst.ico` and uncomment the line in
  `installer\Quantum.iss` to enable it.
