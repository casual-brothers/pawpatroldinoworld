cd /D %~dp0
mkdir \\192.168.51.4\Builds\%4_%1_%2_STAGEZIP
ZipDirectory.bat %~dp0..\Saved\StagedBuilds\%2 \\192.168.51.4\Builds\%4_%1_%2_STAGEZIP\StageBuild.zip