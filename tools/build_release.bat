@echo off
setlocal

rem поднять x64 окружение MSVC/SDK (чтобы линкер/SDK были доступны)
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set VSINSTALL=%%i
call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul

cmake --build --preset Release || exit /b 1

endlocal
