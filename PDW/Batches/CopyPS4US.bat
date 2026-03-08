if "%1"=="" goto noparam


attrib -r %~dp0..\Platforms\PS4\Config\PS4Engine.ini 
attrib -r %~dp0..\Platforms\PS4\Config\PS4Game.ini 
copy /Y %~dp0..\Platforms\PS4\Config\PS4Engine_US.ini %~dp0..\Platforms\PS4\Config\PS4Engine.ini 
copy /Y %~dp0..\Platforms\PS4\Config\PS4Game_US.ini %~dp0..\Platforms\PS4\Config\PS4Game.ini 

attrib -r %~dp0..\Platforms\PS4\Build\sce_sys\nptitle.dat
attrib -r %~dp0..\Platforms\PS4\Build\sce_sys\param.sfo
attrib -r %~dp0..\Platforms\PS4\Build\TitleConfiguration.json
copy /Y %~dp0..\Platforms\PS4\Build\sce_sys\%1_00\nptitle.dat %~dp0..\Platforms\PS4\Build\sce_sys\nptitle.dat
copy /Y %~dp0..\Platforms\PS4\Build\sce_sys\%1_00\param.sfo %~dp0..\Platforms\PS4\Build\sce_sys\param.sfo
copy /Y %~dp0..\Platforms\PS4\Build\sce_sys\%1_00\TitleConfiguration.json %~dp0..\Platforms\PS4\Build\TitleConfiguration.json
goto done


:noparam
echo Missing titleID param

:done
