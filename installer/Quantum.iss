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

; Install under %LOCALAPPDATA% so the app can write chart PNGs without UAC.
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
; main.exe + all Qt DLLs + plugins + MinGW runtime (collected by windeployqt)
Source: "deploy_stage\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; Bundled portable Python runtime with all packages pre-installed
Source: "python_stage\*"; DestDir: "{app}\python"; Flags: ignoreversion recursesubdirs createallsubdirs

; Python scripts called at runtime
Source: "..\scripts\*"; DestDir: "{app}\scripts"; Flags: ignoreversion recursesubdirs createallsubdirs

; Resources: static icons + writable destination for generated chart PNGs
Source: "..\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

; Documentation
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\docs\*"; DestDir: "{app}\docs"; Flags: ignoreversion recursesubdirs createallsubdirs

; Example script
Source: "..\test_script.qtm"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; \
  WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
