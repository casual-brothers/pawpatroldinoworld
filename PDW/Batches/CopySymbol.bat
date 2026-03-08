cd /D %~dp0
mkdir \\192.168.51.4\Builds\%3_%1_%2_SYMBOL
xcopy %~dp0..\Binaries\%2\*%1.* \\192.168.51.4\Builds\%3_%1_%2_SYMBOL
