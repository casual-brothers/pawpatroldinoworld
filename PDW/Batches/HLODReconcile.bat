p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=%1
p4 set P4CLIENT=%2

cd /D %~dp0../..
p4 reconcile Main/Content/__ExternalActors__/...