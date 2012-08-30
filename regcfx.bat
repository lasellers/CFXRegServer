copy Release\CFXRegServer.exe ..\CFXRegServer.exe
copy Release\CFXRegServer.exe ..\CFX_AudioInfo
copy Release\CFXRegServer.exe ..\CFX_ImageInfo
copy Release\CFXRegServer.exe ..\CFX_ConsoleCommand
copy Release\CFXRegServer.exe ..\CFX_Execute
copy Release\CFXRegServer.exe ..\IHTK

cd ..\CFX_AudioInfo
CFXRegServer CFX_AudioInfo.jar

cd ..\CFX_ImageInfo
CFXRegServer CFX_ImageInfo.jar

cd ..\CFX_ConsoleCommand
CFXRegServer CFX_ConsoleCommand.jar

cd ..\CFX_Execute
CFXRegServer CFX_Execute.jar

cd ..\CFXRegServer
