#######################
# GameLog.rb
# Connor Hilarides
# Created 2014/11/09
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

module GameLog
  LOG_DIR = Dir.appdata["DigiPen"]["Roguebot"]["GameLogs"].path
  LOG_FILE = File.new

  def self.log(data)
    LOG_FILE.open LOG_DIR + "/game_log.txt", :out, :append
    LOG_FILE.write data.inspect
    LOG_FILE.write ','
    LOG_FILE.close
  end
end
