; ============================================================
;  宵月良宵 · 中秋交互庆典  安装包脚本 (Inno Setup 6)
; ============================================================

#define AppName        "宵月良宵 · 中秋交互庆典"
#define AppShortName   "宵月良宵"
#define AppVersion     "1.0.0"
#define AppPublisher   "SherlockYzz"
#define AppExeName     "FestivalLauncher.exe"

[Setup]
AppId={{8F3C2B14-7A5E-4D9C-9E21-6C4B8A0F2D77}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher={#AppPublisher}
VersionInfoVersion=1.0.0.0
VersionInfoCompany={#AppPublisher}
VersionInfoDescription={#AppName} 安装程序
VersionInfoProductName={#AppName}
DefaultDirName={autopf}\FestivalCelebration
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
UninstallDisplayName={#AppName}
UninstallDisplayIcon={app}\{#AppExeName}
OutputDir=output
OutputBaseFilename=宵月良宵_中秋庆典_v1.0.0_安装包
SetupIconFile=app.ico
WizardStyle=modern
WizardImageFile=wizard.bmp
WizardSmallImageFile=wizard_small.bmp
Compression=lzma2/max
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
AllowNoIcons=yes
DisableWelcomePage=no
LicenseFile=使用许可与说明.txt

[Languages]
Name: "chinese"; MessagesFile: "ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式(&D)"; GroupDescription: "附加任务："; Flags: checkedonce

[Files]
Source: "dist\festival_app.exe";    DestDir: "{app}"; Flags: ignoreversion
Source: "dist\FestivalLauncher.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README.md";             DestDir: "{app}"; Flags: ignoreversion
Source: "使用说明.txt";              DestDir: "{app}"; Flags: ignoreversion isreadme

[Icons]
Name: "{group}\{#AppShortName}";                 Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Comment: "{#AppName}"
Name: "{group}\使用说明";                         Filename: "{app}\使用说明.txt"
Name: "{autodesktop}\{#AppShortName}";           Filename: "{app}\{#AppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{group}\卸载 {#AppShortName}";             Filename: "{uninstallexe}"

[Run]
Filename: "{app}\{#AppExeName}"; Description: "立即启动 {#AppShortName}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"