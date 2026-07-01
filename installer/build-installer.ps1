#Requires -Version 5.1
<#
.SYNOPSIS
    Builds dist\QuantumSetup.exe - a self-contained installer that bundles
    Python, all required packages, and Qt DLLs. Users need nothing pre-installed.

.DESCRIPTION
    Steps:
      1. Verify build\main.exe exists.
      2. Ensure Qt DLLs and platforms\qwindows.dll are in place.
      3. Download the Python embeddable package and install yfinance, matplotlib,
         pandas, numpy, and scipy into installer\python_stage\.
      4. Compile installer\Quantum.iss with Inno Setup.
      5. Write dist\QuantumSetup.exe.

.PARAMETER QtDir
    Qt MinGW 64-bit root. Only needed if platforms\qwindows.dll is missing.
    Default: C:\Qt\6.11.1\mingw_64

.PARAMETER InnoSetupDir
    Directory containing ISCC.exe.
    Default: C:\Program Files (x86)\Inno Setup 6

.PARAMETER PythonVersion
    Version of the Python embeddable package to download.
    Default: 3.12.10

.PARAMETER RebuildPython
    Force re-download and re-install of the bundled Python even if
    installer\python_stage\ already exists.

.EXAMPLE
    .\installer\build-installer.ps1

.EXAMPLE
    .\installer\build-installer.ps1 -RebuildPython -PythonVersion "3.12.10"
#>

param(
    [string]$QtDir         = "C:\Qt\6.11.1\mingw_64",
    [string]$InnoSetupDir  = "C:\Program Files (x86)\Inno Setup 6",
    [string]$PythonVersion = "3.12.10",
    [switch]$RebuildPython
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot  = Split-Path -Parent $ScriptDir
$StageDir  = Join-Path $ScriptDir "python_stage"

function Require-File([string]$Path, [string]$Hint) {
    if (-not (Test-Path $Path)) { Write-Error "$Path not found. $Hint" }
}

# --- 1. Verify compiled executable -------------------------------------------
$ExePath = Join-Path $RepoRoot "build\main.exe"
Require-File $ExePath "Run build.ps1 in the repo root first."
Write-Host "OK  build\main.exe" -ForegroundColor Green

# --- 2. Ensure Qt DLLs are at repo root --------------------------------------
foreach ($Dll in @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll")) {
    $Dest = Join-Path $RepoRoot $Dll
    if (-not (Test-Path $Dest)) {
        $Src = Join-Path $RepoRoot "dlls\$Dll"
        if (Test-Path $Src) {
            Copy-Item $Src $Dest -Force
            Write-Host "Copied $Dll from dlls/" -ForegroundColor Yellow
        } else {
            Write-Error "$Dll missing. Copy Qt DLLs to repo root or run build.ps1."
        }
    } else {
        Write-Host "OK  $Dll" -ForegroundColor Green
    }
}

# --- 3. Ensure platforms\qwindows.dll is present -----------------------------
$PlatformDest = Join-Path $RepoRoot "platforms\qwindows.dll"
if (-not (Test-Path $PlatformDest)) {
    $PlatformSrc = Join-Path $QtDir "plugins\platforms\qwindows.dll"
    if (Test-Path $PlatformSrc) {
        New-Item -ItemType Directory -Force -Path (Join-Path $RepoRoot "platforms") | Out-Null
        Copy-Item $PlatformSrc $PlatformDest -Force
        Write-Host "Copied qwindows.dll from Qt" -ForegroundColor Yellow
    } else {
        Write-Error "platforms\qwindows.dll not found. Pass -QtDir with your Qt path."
    }
} else {
    Write-Host "OK  platforms\qwindows.dll" -ForegroundColor Green
}

# --- 4. Stage bundled Python --------------------------------------------------
if ($RebuildPython -and (Test-Path $StageDir)) {
    Write-Host "Removing existing python_stage/ for rebuild..." -ForegroundColor Yellow
    Remove-Item $StageDir -Recurse -Force
}

if (Test-Path (Join-Path $StageDir "python.exe")) {
    Write-Host "OK  python_stage\ (already built; pass -RebuildPython to refresh)" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "Staging bundled Python $PythonVersion..." -ForegroundColor Cyan

    $ZipName    = "python-$PythonVersion-embed-amd64.zip"
    $ZipUrl     = "https://www.python.org/ftp/python/$PythonVersion/$ZipName"
    $ZipPath    = Join-Path $env:TEMP $ZipName
    $GetPipPath = Join-Path $env:TEMP "get-pip.py"

    Write-Host "  Downloading $ZipName..."
    Invoke-WebRequest -Uri $ZipUrl -OutFile $ZipPath -UseBasicParsing

    Write-Host "  Extracting..."
    New-Item -ItemType Directory -Force -Path $StageDir | Out-Null
    Expand-Archive -Path $ZipPath -DestinationPath $StageDir -Force

    # Enable pip: the embeddable package ships with 'import site' commented out.
    $PthFile = Get-ChildItem $StageDir -Filter "python*._pth" | Select-Object -First 1
    if (-not $PthFile) { Write-Error "Could not find python*._pth in $StageDir" }
    (Get-Content $PthFile.FullName) -replace '#import site', 'import site' | Set-Content $PthFile.FullName
    Write-Host "  Enabled site-packages in $($PthFile.Name)"

    Write-Host "  Downloading get-pip.py..."
    Invoke-WebRequest -Uri "https://bootstrap.pypa.io/get-pip.py" -OutFile $GetPipPath -UseBasicParsing

    $PythonExe = Join-Path $StageDir "python.exe"

    Write-Host "  Installing pip..."
    & $PythonExe $GetPipPath --no-warn-script-location
    if ($LASTEXITCODE -ne 0) { Write-Error "pip installation failed." }

    $PipExe = Join-Path $StageDir "Scripts\pip.exe"
    Require-File $PipExe "pip.exe not found after installation. Something went wrong."

    Write-Host "  Installing Python packages (this may take a few minutes)..."
    & $PipExe install yfinance matplotlib pandas numpy scipy --no-warn-script-location
    if ($LASTEXITCODE -ne 0) { Write-Error "Package installation failed." }

    Write-Host "  Python stage complete." -ForegroundColor Green
}

# --- 5. Create dist/ ----------------------------------------------------------
New-Item -ItemType Directory -Force -Path (Join-Path $RepoRoot "dist") | Out-Null

# --- 6. Ensure Inno Setup is installed ---------------------------------------
$IsccPath = Join-Path $InnoSetupDir "ISCC.exe"
if (-not (Test-Path $IsccPath)) {
    Write-Host ""
    Write-Host "Inno Setup not found - downloading latest release..." -ForegroundColor Cyan

    $release     = Invoke-RestMethod "https://api.github.com/repos/jrsoftware/issrc/releases/latest" -UseBasicParsing
    $asset       = $release.assets | Where-Object { $_.name -match "^innosetup-.*\.exe$" } | Select-Object -First 1
    if (-not $asset) { Write-Error "Could not find Inno Setup installer in GitHub release assets." }

    $InnoInstaller = Join-Path $env:TEMP $asset.name
    Write-Host "  Downloading $($asset.name)..."
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $InnoInstaller -UseBasicParsing

    Write-Host "  Installing (a UAC prompt may appear)..."
    Start-Process -FilePath $InnoInstaller `
        -ArgumentList "/VERYSILENT", "/SUPPRESSMSGBOXES", "/NORESTART" `
        -Wait
    Remove-Item $InnoInstaller -Force

    if (-not (Test-Path $IsccPath)) {
        Write-Error "Inno Setup installed but ISCC.exe not found at default path. Pass -InnoSetupDir if you used a custom install location."
    }
    Write-Host "  Inno Setup installed." -ForegroundColor Green
} else {
    Write-Host "OK  ISCC.exe" -ForegroundColor Green
}

$IssPath = Join-Path $ScriptDir "Quantum.iss"
Require-File $IssPath "installer\Quantum.iss is missing."

Write-Host ""
Write-Host "Compiling installer..." -ForegroundColor Cyan
& $IsccPath $IssPath
if ($LASTEXITCODE -ne 0) { Write-Error "Inno Setup compilation failed (exit $LASTEXITCODE)." }

# --- 7. Report ----------------------------------------------------------------
$OutputExe = Join-Path $RepoRoot "dist\QuantumSetup.exe"
if (Test-Path $OutputExe) {
    $SizeMB = [math]::Round((Get-Item $OutputExe).Length / 1MB, 1)
    Write-Host ""
    Write-Host "Done!  dist\QuantumSetup.exe  ($SizeMB MB)" -ForegroundColor Green
    Write-Host "Upload this file to a GitHub Release."
} else {
    Write-Error "Expected dist\QuantumSetup.exe was not produced."
}
