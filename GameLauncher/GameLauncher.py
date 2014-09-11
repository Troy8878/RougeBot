import sublime, sublime_plugin
import io, os
import subprocess
from threading import Thread

def file_dir(file):
  parts = file.split('/')
  del parts[-1]
  return '/'.join(parts)

def dir_has_run_script(dir):
  files = os.listdir(dir)
  for file in files:
    if file == 'launch_game.ps1':
      return True
  return False

def get_script_dir(file):
  while True:
    file = file_dir(file)

    if file == '':
      return None

    if dir_has_run_script(file):
      return file

class GameLauncher:
  def __init__(self, dir, mode):
    self.dir = dir
    self.mode = mode

  def run(self):
    subprocess.call(
      [
        '/'.join([self.dir, 'x64', self.mode, 'Game.exe'])
      ], 
      cwd = self.dir
    )

class LaunchGameCommand(sublime_plugin.WindowCommand):
  def run(self, mode):
    current_file = self.window.active_view()
    file_name = current_file.file_name().replace('\\', '/')
    script_dir = get_script_dir(file_name)
    print(script_dir)

    if script_dir == None:
      sublime.status_message("Run this command from a game file")
      return

    launcher = GameLauncher(script_dir, mode)
    thread = Thread(target = launcher.run)
    thread.start()
