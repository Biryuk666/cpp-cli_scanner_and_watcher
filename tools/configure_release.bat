@echo off
setlocal

set CONAN=C:\Programs\Tools\Conan\conan\conan.exe

rem 1) поднять x64 окружение MSVC/SDK
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set VSINSTALL=%%i
call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul

rem 2) conan install (сгенерит conan_toolchain.cmake в build\Release)
"%CONAN%" install . ^
  -s build_type=Release ^
  -s compiler.cppstd=23 ^
  --build=missing ^
  -c tools.cmake.cmaketoolchain:generator=Ninja || exit /b 1

rem 3) configure preset
cmake --preset Release || exit /b 1

endlocal
