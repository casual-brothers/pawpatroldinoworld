p4 set P4PORT=192.168.51.5:1666
p4 set P4USER=marco.c
p4 set P4CLIENT=marco.c_KS2_4

rem -q :Quiet mode, which suppresses informational messages and reports only warnings or errors.

rem check out
p4 -q edit E:\PRJ\KS2_4\NebulaEngine_Retail\...

rem revert unchanged
p4 -q revert -a E:\PRJ\KS2_4\NebulaEngine_Retail\...

rem submit and revert
p4 -q submit -f revertunchanged -d "[KS2_5.4] Retail update" E:\PRJ\KS2_4\NebulaEngine_Retail\...