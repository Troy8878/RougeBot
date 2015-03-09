@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32"

if /i "%1"=="run" (
  goto rungame
)
if /i "%1"=="" (
  msbuild /m /property:Configuration=Release
  goto rungame
)
if /i "%1"=="RAVX" (
  goto runavx
)
if /i "%1"=="AVX" (
  msbuild /m /target:Clean;Rebuild /property:Configuration=AVX
  goto runavx
)
if /i "%1"=="build" (
  msbuild /m /target:Rebuild /property:Configuration=Release
  goto rungame
)
if /i "%1"=="clean" (
  msbuild /m /target:Clean /property:Configuration=Release
  goto end
)
if /i "%1"=="cleanbuild" (
  msbuild /m /target:Clean;Rebuild /property:Configuration=Release
  goto rungame
)

:rungame
start x64\Release\Game.exe
goto end

:runavx
start x64\AVX\Game.exe
goto end

:end
