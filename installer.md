# Prompt for Claude: Build a Windows Installer for Quantum

You are working in the `Quantum` repository. Analyze the repo first, then create a proper Windows installer so users can download one file from GitHub Releases, open it, install the app, and run the Quantum IDE without manually downloading DLLs.

## Repository Context

Quantum is a small Qt/C++ desktop IDE for `.qtm` quant scripts.

Important runtime files:

- `build/main.exe` is the compiled Windows executable.
- `Qt6Core.dll`, `Qt6Gui.dll`, and `Qt6Widgets.dll` are required beside the executable.
- `platforms/qwindows.dll` is required under a `platforms/` directory beside the executable.
- `scripts/fetchPrice.py`, `scripts/watchPrice.py`, and `scripts/montecarlo.py` are called by the app using `python`.
- `resources/` must be shipped because the app loads icons and writes generated chart PNGs there.
- `test_script.qtm` is useful as an example file but should not be required to launch.

The app currently uses relative paths like `scripts/fetchPrice.py` and `resources/...`, so the installer must install the app into a folder layout where those paths work when the executable starts.

Python dependencies used by the scripts:

- `yfinance`
- `matplotlib`
- `pandas`
- `numpy`
- `scipy`

## Goal

Create a release-ready Windows installer that can be uploaded as a single `.exe` asset to the GitHub Releases tab. A user should download that installer, run it, and get a Start Menu/Desktop shortcut for Quantum. They should not need to manually download Qt DLLs or copy folders.

## Preferred Installer Technology

Use Inno Setup unless there is a strong reason not to. Generate an `.iss` script and any supporting build/release scripts needed.

Name the installer output something like:

```text
QuantumSetup.exe
```

## Required Work

1. Inspect the repo and confirm the actual executable name and runtime layout.
2. Add an Inno Setup installer script, preferably at `installer/Quantum.iss`.
3. The installer must include:
   - `main.exe`
   - the required Qt DLLs
   - `platforms/qwindows.dll`
   - the full `scripts/` directory
   - the full `resources/` directory, including `resources/buttons/`
   - `README.md`
   - `docs/`
   - optionally `test_script.qtm`
4. Install into a normal Windows app directory, such as:

```text
{autopf}\Quantum
```

5. Create shortcuts:
   - Start Menu shortcut named `Quantum`
   - optional Desktop shortcut controlled by an installer checkbox
6. Set the shortcut working directory to the installed app directory so the current relative paths continue to work.
7. Add uninstall support through Windows Apps & Features.
8. Add a clear installer icon if a suitable icon exists. If not, leave a TODO and do not invent a fake icon.
9. Add a release build script, for example `build-installer.ps1`, that:
   - builds or verifies `build/main.exe`
   - copies/collects release files into a clean staging directory
   - invokes Inno Setup Compiler
   - writes the final installer to a predictable folder like `dist/`
10. Update docs with short release instructions explaining how to build `QuantumSetup.exe` and upload it to GitHub Releases.

## Python Dependency Decision

The current app shells out to `python`, so decide and implement one of these paths:

### Option A: Require Installed Python

Use this only if you want the quickest simple installer.

- During install, check whether `python` is available on PATH.
- If Python is missing, show a clear message telling the user to install Python 3.
- Add a post-install or first-run helper script that installs required packages with:

```powershell
python -m pip install yfinance matplotlib pandas numpy scipy
```

- Document this limitation clearly.

### Option B: Bundle Python Runtime

Use this if you want the best user experience.

- Bundle a portable Python runtime or convert the Python scripts to standalone executables.
- Ensure Quantum can run charts and simulations on a clean Windows machine without preinstalled Python.
- If changing C++ code is needed, keep the change minimal and explain it.

If you are unsure, implement Option A first because it is simpler, then leave a clear TODO for Option B.

## Important Technical Details

- Do not assume the working directory will automatically be the install directory. Set it explicitly in shortcuts.
- Do not require users to download DLLs separately.
- Preserve the `platforms/qwindows.dll` path exactly as `platforms/qwindows.dll` under the installed app folder.
- Make sure generated chart images can still be written under `resources/`. If installing under Program Files causes write permission problems, either:
  - change the app to write generated charts under `%LOCALAPPDATA%\Quantum`, or
  - install the app somewhere user-writable.

Prefer fixing the write-location problem properly in the app instead of relying on Program Files being writable.

## GitHub Release Result

The final release flow should be:

1. Run the release build script.
2. Get `dist/QuantumSetup.exe`.
3. Upload `QuantumSetup.exe` to a GitHub Release.
4. Users download and run only `QuantumSetup.exe`.
5. Quantum launches from the shortcut.

## Verification Checklist

After implementing, test or explain how to test:

- Fresh install on Windows.
- Launch from Start Menu shortcut.
- Launch from Desktop shortcut if selected.
- `load(AMZN)` works.
- `display()` generates and displays a chart.
- `compare(AAPL)` works.
- `monte(AMZN+AAPL)` works.
- Uninstall removes the app and shortcuts.

## Deliverables

Return a concise summary of:

- Files added or changed.
- How to build the installer.
- Where the installer output is created.
- Any remaining limitations, especially around Python dependencies.
