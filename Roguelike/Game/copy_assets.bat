mkdir ..\x64\Debug\Assets >nul 2>nul
mkdir ..\x64\Release\Assets >nul 2>nul
xcopy /s /Y ..\Assets ..\x64\Debug\Assets >nul 2>nul
xcopy /s /Y ..\Assets ..\x64\Release\Assets >nul 2>nul
echo "Asset files copied"
