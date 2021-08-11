; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "SDFBuilder"
#define MyAppVersion "0.1"
#define MyAppPublisher "Michael Sohnen"
#define MyAppURL "https://github.com/mikeandike523"
#define MyAppExeName "runprogram.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{37F28584-78D8-4181-971D-6A10A63CF3F3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
ChangesAssociations=yes
DisableProgramGroupPage=yes
LicenseFile=D:\Projects\SDFBuilder\LICENSE
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputBaseFilename=SDFBuilderInstaller
OutputDir=D:\Projects\SDFBuilder\
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\Projects\SDFBuilder\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
;Source: "D:\Projects\SDFBuilder\Debug\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "D:\Projects\SDFBuilder\Debug\*"; DestDir: "{app}\Debug"; Flags: ignoreversion
Source: "D:\Projects\SDFBuilder\v1.vert"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Projects\SDFBuilder\f1.frag"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Projects\SDFBuilder\triSDFON2.cl"; DestDir: "{app}"; Flags: ignoreversion

[Registry]

Root: HKA; Subkey: "Software\Classes\Applications\{#MyAppExeName}\SupportedTypes"; ValueType: string; ValueName: ".myp"; ValueData: ""

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

