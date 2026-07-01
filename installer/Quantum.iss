#define MyAppName "Quantum"
#define MyAppVersion "1.0"
#define MyAppPublisher "veefs"
#define MyAppURL "https://github.com/veefs/quantum"
#define MyAppExeName "main.exe"

[Setup]
AppId={{A3F7B2C1-9E4D-4F8A-B6C3-2D1E0F5A7B9C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/issues
AppUpdatesURL={#MyAppURL}/releases

; Install under %LOCALAPPDATA% — user-writable, so the app can write
; generated chart PNGs to resources/ without UAC or elevated permissions.
DefaultDirName={localappdata}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
PrivilegesRequired=lowest

; TODO: Convert resources/qst.png to qst.ico and uncomment:
; SetupIconFile=..\resources\qst.ico

OutputDir=..\dist
OutputBaseFilename=QuantumSetup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "..\build\main.exe"; DestDir: "{app}"; Flags: ignoreversion

; Qt runtime DLLs (must sit beside main.exe)
Source: "..\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion

; MinGW runtime DLLs (required by any exe compiled with MinGW g++)
Source: "..\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion

; Qt Windows platform plugin (must live at platforms\qwindows.dll relative to main.exe)
Source: "..\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion

; Python scripts called at runtime
Source: "..\scripts\*"; DestDir: "{app}\scripts"; Flags: ignoreversion recursesubdirs createallsubdirs

; Resources: static icons + writable destination for generated chart PNGs
Source: "..\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

; Bundled portable Python runtime (built by build-installer.ps1 into python_stage/)
; main.exe calls python_stage/python.exe directly — no system Python needed.
Source: "python_stage\*"; DestDir: "{app}\python"; Flags: ignoreversion recursesubdirs createallsubdirs

; Documentation
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\docs\*"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs createallsubdirs

; Example script
Source: "..\test_script.qtm"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Start Menu shortcut — WorkingDir keeps relative paths (scripts/, resources/) working
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

; Optional Desktop shortcut (opt-in, unchecked by default)
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; \
  WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
