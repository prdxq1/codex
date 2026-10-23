@echo off
setlocal
set "LAB_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%LAB_VSWHERE%" goto notfound
for /f "usebackq delims=" %%I in (`"%LAB_VSWHERE%" -latest -products * -version "[18.0,19.0)" -find Common7\IDE\devenv.exe`) do (
    start "" "%%I" "%~dp0CodexLab.slnx"
    exit /b 0
)
:notfound
echo Visual Studio 2026 was not found through Visual Studio Installer.
echo Open Visual Studio 2026, then File - Open - Project/Solution.
echo Select CodexLab.slnx from this extracted folder.
echo Install the Desktop development with C++ workload if C++ is unavailable.
pause
exit /b 1
