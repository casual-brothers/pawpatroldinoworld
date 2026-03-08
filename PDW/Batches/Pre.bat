echo %1>..\LockFile.txt
call %~dp0..\Batches\EOSDisable.bat
rem call %~dp0..\Batches\GOGDisable.bat

goto CONTINUA

@echo off
setlocal ENABLEDELAYEDEXPANSION

:START
rem Prende ora attuale
for /f "tokens=1-2 delims=:" %%a in ("%time%") do (
    set hour=%%a
    set min=%%b
)

rem Rimuove spazi iniziali (es. " 1" -> "1")
set hour=%hour: =%

rem Debug: mostra l’ora attuale
echo Ora attuale: %hour%:%min%

rem Controlla se l’orario è tra le 23 e le 1:59
if %hour% GEQ 23 (
    goto WAIT
) else (
    if %hour% LSS 2 (
        goto WAIT
    )
)

goto CONTINUA

:WAIT
echo Siamo tra le 23:00 e le 01:59. Attendo 60 secondi...
timeout /t 60 >nul
goto START

:CONTINUA
echo Ora fuori dall'intervallo. Procedo con il resto dello script...
