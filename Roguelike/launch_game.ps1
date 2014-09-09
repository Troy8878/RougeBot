param([string] $mode = 'debug')

if ($mode -eq 'release')
{
  .\x64\Release\Game.exe
}
else
{
  .\x64\Debug\Game.exe
}
