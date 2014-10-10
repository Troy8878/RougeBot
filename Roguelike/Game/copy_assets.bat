mkdir ..\x64\Debug\Assets >nul 2>nul
mkdir ..\x64\Release\Assets >nul 2>nul
xcopy /s /Y ..\Assets ..\x64\Debug\Assets >nul 2>nul
xcopy /s /Y ..\Assets ..\x64\Release\Assets >nul 2>nul

copy ..\lib\fmod64.dll ..\x64\Debug\ >nul 2>nul
copy ..\lib\fmod64.dll ..\x64\Release\ >nul 2>nul

echo "Asset files copied"
