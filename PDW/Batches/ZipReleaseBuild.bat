cd /D %~dp0
mkdir \\192.168.51.4\Builds\%4_%1_%2_RELEASEZIP
ZipDirectory.bat %~dp0..\Releases\%3\%2 \\192.168.51.4\Builds\%4_%1_%2_RELEASEZIP\ReleaseBuild.zip