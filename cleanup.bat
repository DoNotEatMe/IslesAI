@echo off

REM Define ANSI color codes
set COLOR_RED=[31m
set COLOR_GREEN=[32m
set COLOR_YELLOW=[33m
set COLOR_RESET=[0m

:: List of directories to preserve
set "KEEP_DIRS=Config Content Source Plugins .git"

:: List of file extensions to preserve
set "KEEP_EXTENSIONS=.uproject .gitattributes .gitignore .md .bat"

:: Loop through all directories in the current folder
for /d %%D in (*) do (
    echo %%D | findstr /i "\<%KEEP_DIRS%\>" >nul
    if errorlevel 1 (
        echo ---%COLOR_RED%Deleting directory %%D%COLOR_RESET%
        rmdir /s /q "%%D"
    )else (
        echo +++%COLOR_GREEN%Skipping directory %%D%COLOR_RESET%
    )
)

:: Loop through all files in the current folder
for %%F in (*) do (
    setlocal enabledelayedexpansion
    set "SKIP_FILE=0"
    for %%E in (%KEEP_EXTENSIONS%) do (
        if /i "%%~xF"=="%%E" (
            set "SKIP_FILE=1"
        )
    )
    if "!SKIP_FILE!"=="0" (
        echo ---%COLOR_RED%Deleting file %%F%COLOR_RESET%
        del "%%F"
    ) else (
        echo +++%COLOR_GREEN%Skipping file %%F%COLOR_RESET%
    )
    endlocal
)

echo Cleanup complete!
pause