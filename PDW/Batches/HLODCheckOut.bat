p4 set P4PORT=192.168.51.5:1666 
p4 set P4USER=%1
p4 set P4CLIENT=%2

cd /D %~dp0..
p4 edit Content\__ExternalActors__\...

if not %ERRORLEVEL% ==0 (
   call RevertMain.bat %1 %2 
   exit /b %ERRORLEVEL%
) 

p4 edit Content\__ExternalObjects__\...

if not %ERRORLEVEL% == 0 (
   call RevertMain.bat %1 %2 
   exit /b %ERRORLEVEL%
) 
