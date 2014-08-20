Invoke-BatchFile "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"

msbuild Roguelike\Roguelike.sln /v:m /property:Configuration=$env:Configuration
