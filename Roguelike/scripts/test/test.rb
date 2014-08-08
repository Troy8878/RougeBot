class Array
  def sum
    sum = 0
    self.each {|i| sum += i }
    sum
  end
end

class Game
  def coin_flip
    (Game.rand :int, 1) == 1
  end

  def game_title
    "I'm the title :D"
  end

  def game_text
    flip_count = 9

    flips = []
    flip_count.times { flips << coin_flip }
    won_flips = flips.map {|flip| flip ? 1 : 0 }.sum
    display_flips = flips.map {|flip| flip ? :win : :lose }
    win = won_flips > flip_count / 2

    win_text = "You #{win ? 'won' : 'lost'} the game! #{win ? '' : '>'}:D"
    summary_text = "#{won_flips} flips won out of #{flip_count}"
    "#{win_text}\n#{summary_text}\n#{display_flips}"
  end

  def run_game
    Game.message_box "#{game_title}", "#{game_text}", 
                      MessageBox::Buttons::YESNOCANCEL | 
                      MessageBox::Icon::INFORMATION
  end
end
