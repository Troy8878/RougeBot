###########################
# GameConsoleComponent.rb
# Connor Hilarides
# Created 2014/10/24
###########################

class GameConsoleComponent < ComponentBase
  dependency "TransformComponent"

  OPEN_Y = 0.2
  CLOSE_Y = 3

  OPEN_SPEED = 8
  CLOSE_SPEED = 8

  def initialize(data)
    super data

    @open = false
    @pos = owner.transform_component.position
    @textbuf = String.new

    register_event :update, :on_update
    register_event :key_down, :on_key
    register_event :key_char, :on_char
    register_event :append_line, :append_line
  end

  def on_key(e)
    if e.vkey == Keys::TILDE
      self.toggle
      return
    end

    return unless @open

    if e.vkey == Keys::RETURN
      append_line({text: ("> " + @textbuf), color: 10})

      begin
        result = eval(@textbuf).inspect
        until result.nil? || result.empty?
          append_line({text: result.slice(0, 100), color: 11})
          result = result.slice(99, result.length)
        end
      rescue Exception => e
        append_line({text: e.inspect, color: 12})
      end
      @textbuf = String.new
    elsif e.vkey == Keys::BACK
      @textbuf = @textbuf.slice(0, @textbuf.length - 1)
    end

    update_textbuf
  end

  IGNORE_KEYS = [
    96, # `
    8,  # \b
    13  # \n
  ]

  def on_char(e)
    return unless @open
    return if IGNORE_KEYS.include? e.char_code
    @textbuf += e
    update_textbuf
  end

  def update_textbuf
    box = owner.local_find "Textbox"
    box.raise_event :send, [:set_text_at, [0, "> " + @textbuf]]
  end

  def on_update(e)
    pos = @pos

    if @open && pos.y > OPEN_Y
      pos.y -= e.dt * OPEN_SPEED
      if pos.y < OPEN_Y
        pos.y = OPEN_Y
      end
    elsif !@open && pos.y < CLOSE_Y
      pos.y += e.dt * CLOSE_SPEED
      if pos.y > CLOSE_Y
        pos.y = CLOSE_Y
      end
    end

    ary = AryStreamBuffer.game_console.peek
    unless ary.empty?
      append_line(ary.first)
      ary.delete_at 0
    end
  end

  FG_ALPHA = Vector.new(0, 0, 0, 1)
  FG_RED = Vector.new(1, 0, 0)
  FG_GRN = Vector.new(0, 1, 0)
  FG_BLU = Vector.new(0, 0, 1)
  FG_DRK = Vector.new(0.5, 0.5, 0.5)

  CONSOLE_COLORS = {
    0  => FG_DRK * 0.5               + FG_ALPHA,
    1  => FG_DRK * (FG_BLU)          + FG_ALPHA,
    2  => FG_DRK * (FG_GRN)          + FG_ALPHA,
    3  => FG_DRK * (FG_GRN + FG_BLU) + FG_ALPHA,
    4  => FG_DRK * (FG_RED)          + FG_ALPHA,
    5  => FG_DRK * (FG_RED + FG_BLU) + FG_ALPHA,
    6  => FG_DRK * (FG_RED + FG_GRN) + FG_ALPHA,
    7  => FG_DRK                     + FG_ALPHA,
    8  => FG_DRK * 1.5               + FG_ALPHA,
    9  => FG_BLU                     + FG_ALPHA,
    10 => FG_GRN                     + FG_ALPHA,
    11 => FG_GRN + FG_BLU            + FG_ALPHA,
    12 => FG_RED                     + FG_ALPHA,
    13 => FG_RED + FG_BLU            + FG_ALPHA,
    14 => FG_RED + FG_GRN            + FG_ALPHA,
    15 => FG_RED + FG_GRN + FG_BLU   + FG_ALPHA
  }

  def append_line(msg)
    root = owner.local_find("TextRoot")
    root.raise_event :shift_up, nil

    item = root.create_child(
      archetype: "HUD/Console/TextItem"
    )

    item.raise_event :send, [:set_text_at, [0, msg[:text]]]
    item.raise_event :send, [:tint=, [CONSOLE_COLORS[msg[:color]]]]
  end

  def toggle
    if @open
      @open = false
      KeybindingComponent.unlock!
    else
      @open = true
      KeybindingComponent.lock!
    end
  end

  register_component "GameConsoleComponent"
end
