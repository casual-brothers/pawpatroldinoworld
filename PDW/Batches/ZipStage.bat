cd /D %~dp0
mkdir \\192.168.51.4\Builds\%2
ZipDirectory.bat %~dp0..\Saved\StagedBuilds\%1 \\192.168.51.4\Builds\%2\StageBuild.zip