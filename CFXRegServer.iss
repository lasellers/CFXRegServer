; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=CFXRegServer
AppVerName=CFXRegServer 1.6h
AppPublisher=Intrafoundation Software
AppPublisherURL=http://www.intrafoundation.com
AppSupportURL=http://www.intrafoundation.com/CFXRegServer.asp
AppUpdatesURL=http://www.intrafoundation.com/CFXRegServer.asp
DefaultDirName={pf}\CFXRegServer
DefaultGroupName=CFXRegServer
AllowNoIcons=yes
LicenseFile=C:\projects\CFXRegServer\License.txt
Compression=lzma
SolidCompression=yes
OutputBaseFilename=CFXRegServer_v1.6
OutputDir=c:\projects\intrafoundationcom\html\freeware\
VersionInfoCompany=Intrafoundation Software
VersionInfoDescription=CFXRegServer 1.6h
VersionInfoTextVersion=v1.6
VersionInfoVersion=1.6.7.0
WizardImageFile=c:\projects\installer.bmp
WizardSmallImageFile=c:\projects\installer.small.bmp

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
;Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\projects\CFXRegServer\CFXRegServer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\projects\CFXRegServer\default.htm"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\projects\CFXRegServer\index.html"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\projects\CFXRegServer\style.css"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\projects\CFXRegServer\License.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\projects\CFXRegServer\Command Prompt.bat"; DestDir: "{app}"; Flags: ignoreversion

;Source: "C:\projects\CFXRegServer\CFXRegServer.cpp"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\CFXRegServer.dsp"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\CFXRegServer.dsw"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\CFXRegServer.ncb"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\CFXRegServer.opt"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\CFXRegServer.plg"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\StdAfx.cpp"; DestDir: "{app}"; Flags: ignoreversion
;Source: "C:\projects\CFXRegServer\StdAfx.h"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[INI]
Filename: "{app}\CFXRegServer.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://www.intrafoundation.com/CFXRegServer.asp"

[Icons]
Name: "{group}\CFXRegServer"; Filename: "{app}\ReadMe.html"
Name: "{group}\{cm:ProgramOnTheWeb,CFXRegServer}"; Filename: "{app}\CFXRegServer.url"
Name: "{group}\{cm:UninstallProgram,CFXRegServer}"; Filename: "{uninstallexe}"
Name: "{group}\Explore CFXRegServer folder"; Filename: "{app}"
Name: "{userdesktop}\CFXRegServer"; Filename: "{app}\index.html"; Tasks: desktopicon
;Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\CFXRegServer"; Filename: "{app}\index.html"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\index.html"; Description: "View README documentation"; Flags: shellexec postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\CFXRegServer.url"

