@echo off

call setvc12

if /i "%1"=="run" (
  goto rungame
)
if /i "%1"=="" (
  msbuild /m /property:Configuration=Release
  goto rungame
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

:end
