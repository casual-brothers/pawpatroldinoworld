FOR /d /r . %%d IN ("intermediate") DO (
    IF "%%d" NEQ "saved" (
        IF "%%d" NEQ ".vs" (
            IF EXIST "%%d" (
                rd /s /q "%%d"
            )
        )
    )
)