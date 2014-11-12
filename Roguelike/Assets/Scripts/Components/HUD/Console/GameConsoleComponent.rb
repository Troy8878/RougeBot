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

  HISTORY = []
  INPUT_HISTORY = []

  def initialize(data)
    super data

    @open = false
    @pos = owner.transform_component.position
    @textbuf = String.new
    @history_index = -1
    @suffix = ""
    @suffix_sequence = self.owner.action_sequence :text_suffix
    add_suffix

    register_event :update, :first_update
    register_event :key_down, :on_key
    register_event :key_held, :on_key_repeat
    register_event :key_char, :on_char
    register_event :append_line, :append_line
    register_event :quit, :quit
  end

  def add_suffix
    @suffix = "|"
    update_textbuf

    @suffix_sequence.delay 0.5
    @suffix_sequence.once do
      self.remove_suffix
    end
  end

  def remove_suffix
    @suffix = ""
    update_textbuf

    @suffix_sequence.delay 0.5
    @suffix_sequence.once do
      self.add_suffix
    end
  end

  def on_key(e)
    if e.vkey == Keys::TILDE
      self.toggle
      return
    end

    return unless @open

    case e.vkey
    when Keys::RETURN
      return if @textbuf.empty?

      append_text ("> " + @textbuf), 10

      begin
        INPUT_HISTORY.insert 0, @textbuf

        result = eval(@textbuf).inspect

        process_output # if there's any puts messages in there

        append_text result, 11
      rescue Exception => e
        append_text e.inspect, 12
      end
      @textbuf = String.new
      @history_index = -1
    when Keys::BACK
      @backspace_cooldown = nil
    when Keys::UP
      if INPUT_HISTORY.length > 0
        @history_index += 1
        if @history_index >= INPUT_HISTORY.length
          @history_index = INPUT_HISTORY.length - 1
        end

        @textbuf = INPUT_HISTORY[@history_index]
      end
    when Keys::DOWN
      if INPUT_HISTORY.length > 0
        @history_index -= 1
        if @history_index < 0
          @history_index = -1
          @textbuf = String.new
        else
          @textbuf = INPUT_HISTORY[@history_index]
        end
      end
    end
    update_textbuf
  end

  def on_key_repeat(e)
    return unless e.vkey == Keys::BACK
    return if @textbuf.empty?

    if @backspace_cooldown.nil?
      @backspace_cooldown = 0.2
    elsif @backspace_cooldown > 0
      return
    end

    @textbuf = @textbuf.slice(0, @textbuf.length - 1)
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
    box.raise_event :send, [:set_text_at, [0, "> " + @textbuf + @suffix]]
  end

  def first_update(e)
    register_event :update, :on_update

    ary = AryStreamBuffer.game_console.peek
    ary.replace(HISTORY + ary)
  end

  def process_output
    ary = AryStreamBuffer.game_console.flush
    ary.each do |msg|
      append_line(msg)
    end
  end

  def on_update(e)
    # Backspace should cool down after the first stroke
    @backspace_cooldown ||= 0
    @backspace_cooldown -= e.dt if @backspace_cooldown > 0

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

    process_output
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

  def append_text(msg, color)
    if msg.include? "\n"
      msg.split("\n").each do |line|
        append_text(line, color)
      end
      return
    end

    until msg.nil? || msg.empty?
      append_line({text: msg.slice(0, 100), color: color})
      msg = msg.slice(100, msg.length)
    end
  end

  def append_line(msg)
    HISTORY << msg
    HISTORY.delete_at 0 while HISTORY.length > 50

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

  def quit(e)
    return unless @open

    e[:quit] = false
    self.toggle
  end

  register_component "GameConsoleComponent"
end
