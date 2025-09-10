@echo off
setlocal enableextensions enabledelayedexpansion

REM %1 = repo root, %2 = output header
set "REPO=%~1"
set "OUTH=%~2"
set "TMPH=%OUTH%.tmp"

pushd "%REPO%" >NUL 2>&1

for /f "usebackq tokens=*" %%i in (`git describe --tags --always --dirty --long 2^>NUL`) do set "GVER=%%i"
if not defined GVER set "GVER=unknown"

for /f "usebackq tokens=*" %%i in (`git rev-parse --abbrev-ref HEAD 2^>NUL`) do set "GBRANCH=%%i"
if not defined GBRANCH set "GBRANCH=unknown"
if /i "!GBRANCH!"=="HEAD" (
  for /f "usebackq tokens=*" %%k in (`git rev-parse --short=8 HEAD 2^>NUL`) do set "GBRANCH=%%k"
)

> "%TMPH%" (
  echo // Auto-generated. Do NOT edit.
  echo #pragma once
  echo #define GIT_VERSION "!GVER!"
  echo #define GIT_BRANCH  "!GBRANCH!"
)

fc /b "%TMPH%" "%OUTH%" >NUL 2>&1
if errorlevel 1 copy /y "%TMPH%" "%OUTH%" >NUL
del /f /q "%TMPH%" >NUL 2>&1

popd >NUL 2>&1
exit /b 0
