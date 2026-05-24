@echo off
setlocal

set "CMAKE_2022=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "CMAKE_18=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "VS2022_COMMUNITY=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
set "VS18_COMMUNITY=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"

if exist "%VS18_COMMUNITY%" (
    call "%VS18_COMMUNITY%" -arch=x64
) else if exist "%VS2022_COMMUNITY%" (
    call "%VS2022_COMMUNITY%" -arch=x64
) else (
    echo Could not find VsDevCmd.bat for Visual Studio Community.
    exit /b 1
)

if errorlevel 1 exit /b 1

if exist "%CMAKE_18%" (
    set "CMAKE_EXE=%CMAKE_18%"
) else if exist "%CMAKE_2022%" (
    set "CMAKE_EXE=%CMAKE_2022%"
) else (
    echo Could not find cmake.exe in Visual Studio.
    exit /b 1
)

"%CMAKE_EXE%" -S . -B build-msvc -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

"%CMAKE_EXE%" --build build-msvc --config Release
