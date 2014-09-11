Game200-Project
===============

Building
--------

Before you can build and run this project, make sure you
set the Default Startup Project to "Game", and edit the
working directory in the Game debugging settings to be
    $(SolutionDir)


Sublime Plugin
--------------

Copy the "GameLauncher" folder into this folder:
~\AppData\Roaming\Sublime Text 3\Packages

If you want the keyboard shortcuts, add these to your
Preferences -> Key Bindings - User

    { "keys": ["f5"], "command": "launch_game", "args": {"mode": "Debug"} },
    { "keys": ["ctrl+f5"], "command": "launch_game", "args": {"mode": "Release"} }


