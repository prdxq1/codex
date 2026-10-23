@echo off
setlocal
set "LAB_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "LAB_MSBUILD="
if not exist "%LAB_VSWHERE%" goto missing
for /f "usebackq delims=" %%I in (`"%LAB_VSWHERE%" -latest -products * -version "[18.0,19.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find MSBuild\Current\Bin\MSBuild.exe`) do set "LAB_MSBUILD=%%I"
if not defined LAB_MSBUILD goto missing
pushd "%~dp0"
"%LAB_MSBUILD%" "%~dp0CodexLab.vcxproj" /m /p:Configuration=Debug /p:Platform=x64 /verbosity:minimal
if errorlevel 1 goto failed
if not exist "%~dp0bin\vs\CodexLab\x64\Debug\CodexLab.exe" goto failed
"%~dp0bin\vs\CodexLab\x64\Debug\CodexLab.exe"
set "LAB_EXIT=%ERRORLEVEL%"
echo.
echo Images: %~dp0test_output
echo Program exit code: %LAB_EXIT%
popd
pause
exit /b %LAB_EXIT%
:failed
echo.
echo Build failed. The program was not started. Read the error above.
popd
pause
exit /b 1
:missing
echo Visual Studio 2026 C++ build tools were not found.
echo In Visual Studio Installer install Desktop development with C++,
echo including the MSVC v145 x64/x86 build tools and Windows SDK.
pause
exit /b 1
