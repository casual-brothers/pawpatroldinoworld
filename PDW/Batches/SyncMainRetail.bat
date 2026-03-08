p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=%1
p4 set P4CLIENT=%2
p4 set P4IGNORE=.p4ignore

cd /D %~dp0..\..

@echo Synching Main
p4 sync Main\...

@echo Synching Retail
p4 sync NebulaEngine_Retail\...
