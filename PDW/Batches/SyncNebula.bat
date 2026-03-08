p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=%1
p4 set P4CLIENT=%2

cd /D %~dp0..\..

@echo Synching Retail
p4 sync NebulaEngine_Retail\...

p4 set P4CLIENT=%3

@echo Synching Source
p4 sync NebulaEngine_Source\...
