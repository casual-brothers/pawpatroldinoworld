FOR /d /r . %%d IN ("intermediate") DO @IF EXIST "%%d" rd /s /q "%%d"
