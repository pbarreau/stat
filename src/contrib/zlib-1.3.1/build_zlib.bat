@echo off
REM Etape a rajouter dans creator pour compilation..
REM Dans la partie Generale
REM Repertoire de compilation : %{ActiveProject:NativePath}\build\%{Project:Name}_%{Kit:FileSystemName}-%{BuildConfig:Name}
REM
REM A mettre apres Qmake et avant Make
REM
REM Commande: cmd
REM Arguments :/c %{ActiveProject:NativePath}\ext\zlib-1.3.1\build_zlib.bat %{CMake:Executable:FilePath}
REM Repertoire de travail : %{ActiveProject:NativePath}\ext\zlib-1.3.1
REM

if exist build\libzlib.dll (
    echo [INFO] zlib already built.
    exit /b 0
)
echo [INFO] Building zlib...
mkdir build
cd build
REM %1 correspond au chemin complet de l'executable cmake
%1 .. -G "MinGW Makefiles"
mingw32-make
