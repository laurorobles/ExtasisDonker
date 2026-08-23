@echo off
setlocal
echo ============================================================
echo  EXTASIS DONKER - Windows Automated Installer
echo ============================================================

set "INSTALL_DIR=%~dp0"
set "PROJECT_DIR=%INSTALL_DIR%.."

echo [1/1] Installing VST3 Plugin to C:\Program Files\Common Files\VST3...
if not exist "C:\Program Files\Common Files\VST3" mkdir "C:\Program Files\Common Files\VST3"
if exist "%PROJECT_DIR%\build\ExtasisDonker_artefacts\VST3\ExtasisDonker.vst3" (
    xcopy /E /I /Y /Q "%PROJECT_DIR%\build\ExtasisDonker_artefacts\VST3\ExtasisDonker.vst3" "C:\Program Files\Common Files\VST3\ExtasisDonker.vst3"
) else if exist "%INSTALL_DIR%ExtasisDonker.vst3" (
    xcopy /E /I /Y /Q "%INSTALL_DIR%ExtasisDonker.vst3" "C:\Program Files\Common Files\VST3\ExtasisDonker.vst3"
)

echo.
echo ============================================================
echo  Installation Complete! Open your DAW and rescan plugins.
echo ============================================================
pause
