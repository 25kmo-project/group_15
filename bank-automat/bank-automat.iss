; Pankkiautomaatti Setup Script for Inno Setup
; Requires Inno Setup 6.0 or later

#define MyAppName "Pankkiautomaatti"
#define MyAppVersion "0.1"
#define MyAppPublisher "Your Name/Company"
#define MyAppExeName "Pankkiautomaatti.exe"
#define MyAppURL "https://yourwebsite.com"

[Setup]
; Perusasetukset
AppId={{E617069E-B8C0-4A82-9DC0-3A5B5A0F3443}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=output
OutputBaseFilename=PankkiautomaattiSetup
SetupIconFile={#SourcePath}\bankicon.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin

; Kieliasetukset
ShowLanguageDialog=auto

[Languages]
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Pääohjelma ja kaikki riippuvuudet
Source: "..\build\Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SourcePath}\bankicon.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\bankicon.ico"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"; IconFilename: "{app}\bankicon.ico"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename: "{app}\bankicon.ico"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#MyAppName}}"; Flags: nowait postinstall skipifsilent

[Code]
// Tarkista että .NET Framework tai muut riippuvuudet on asennettu
function InitializeSetup(): Boolean;
begin
  Result := True;
end;