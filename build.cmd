@echo off
set "BUILD_TYPE="
set "CLEAN=%~2"

for %%I in (%*) do (
    if /i "%%I"=="debug" (
        set "BUILD_TYPE=Debug"
    ) else if /i "%%I"=="release" (
        set "BUILD_TYPE=Release"
    ) else if /i "%%I"=="clean" (
         if exist "build" rd /s /q "build" 2>nul
         exit /b 0
     ) else (
         echo 不支持的参数：%%I
         exit /b 1
     )
)

if not defined BUILD_TYPE (
    set "BUILD_TYPE=Debug"
)

cmake -S . -B build
if "%CLEAN%"=="" if exist "build" rd /s /q "build" 2>nul
cmake --build build --config %BUILD_TYPE%
