##############################
# PlayerControllerComponent.rb
# Jake Robsahm
# Created 2014/09/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
##############################

#########################Class Details##########################
# Component makes the object the player.
#######################Property Details#########################
# health - the health of the player. May be changed if we use
#the same health system for enemies.
# inventory - an inventory object containing all of the player's
#items.
################################################################

PLAYER_INVENTORY = Inventory.new

class PlayerControllerComponent < ComponentBase
  include Actor
  dependency "TransformComponent", "PositionComponent"

  attr_reader :pos

  MIN_MOVE_TIME = 0.2

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @pos = self.owner.position_component.position

    @logic_initialized = false
    @logic_cooldown = 0

    self.register_event :move, :on_move
    self.register_event :fire, :fire
    self.register_event :update, :first_update
    self.register_event :attack, :attack

    # Double-click should do it in either case
    # Double-click is the only way in touch mode
    self.register_event :double_click, :mouse_down
    self.register_event :mouse_down, :mouse_down

    self.register_event :mouse_move, :mouse_move
    self.register_event :on_pause, :on_pause
  end

  def on_pause(val)
    if val
      @paused = true
    else
      @paused = false
    end
  end

  def move(x, y)
    return if @paused

    unless can_move? x, y
      return if @blocked_reason != BLOCKED_BY_ACTOR
    end

    if @blocked_reason == BLOCKED_BY_ACTOR
      self.owner.attack_component.do_attack @move_tile.actor
      @logic_cooldown = 0.5
    else
      @pos.x += x
      @pos.y += y
      @logic_cooldown = 0.2

      actor_moved
    end

    yield_to_enemies

    if search_entities('').select(&:enemy_logic_component).empty?
      seq = owner.action_sequence :victory!
      seq.delay 5
      seq.once do
        # TODO: Win Condition
        Game.switch_level 'MainMenu'
      end
    end
  end

  def yield_to_enemies
    seq = self.owner.action_sequence :delay_logic
    seq.delay(0.05)
    seq.delay(0) # ensure at _minimum_ 2 frames go by
    seq.once do
      if @logic_initialized
        Event.raise_event :logic_update, self.owner
      end
    end
  end

  def on_move(e)
    set_kb_mode
    move *e
  end

  ################################
  # Stuffs Troy added for combat #
  ################################

  def attack(e)
    
    return if @paused
    return if @logic_cooldown > 0

    x = e[0]
    y = e[1]
    
    # For now, don't attack empty tiles.  Once AoE classes are in effect we won't need to worry

    cant_attack = can_move? x, y
    return if @blocked_reason != BLOCKED_BY_ACTOR
    return if cant_attack

    # The actual attack logic.  Will be replaced with the AoE class
    self.owner.attack_component.do_attack @move_tile.actor
    @logic_cooldown = 0.5
    
    actor_moved
    yield_to_enemies
  end

  def fire(e)

    return if @paused
    return if @logic_cooldown > 0

    x = e[0]
    y = e[1]

    unless can_fire? x, y
      return if @blocked_reason != BLOCKED_BY_ACTOR # KEEP THIS!!! We always want to be able to shoot (enemy) actors!
    end

    # We actually don't just want to deal damage if an actor is hit.  Fireable weapons can have affects other than base damage

    find_entity(0).create_child(
      archetype: "PlayerProjectiles/Bomb",    # We'll need to make this the wielded ranged weapon once wielding is implemented
      components: {
        "PositionComponent" => {
          "position" => [@pos.x, @pos.y]
        },
        "PlayerProjectileLogicComponent" => {
          "direc" => [x, y]
        }
      }
    )
    
    @logic_cooldown = 0.5
    seq = self.owner.action_sequence :wait_after_firing
    seq.delay(0.1)

    actor_moved
    yield_to_enemies
  end
  
  #################
  # End of stuffs #
  #################
  
  def mouse_down(e)
    @cursor ||= find_entity("TileCursor")
    @curpos ||= @cursor.transform_component.position

    dx = Math.round(@curpos.x - @pos.x)
    dy = Math.round(@curpos.y - @pos.y)

    move dx, dy
  end

  def first_update(e)
    shape = MapItem::ELLIPSE
    actor_init(shape, "Yellow")

    move 0, 0

    @camera = find_entity("CameraRoot")
    @camera.parent = self.owner
    @camz = @camera.transform_component.position.z

    @logic_initialized = true

    set_kb_mode

    register_event :update, :on_update
  end

  def on_update(e)
    pos = @transform.position

    xbounce = Math.sin((pos.x % 1) * Math::PI) / 6
    ybounce = Math.sin((pos.y % 1) * Math::PI) / 6

    pos.z = -(0.25 + xbounce + ybounce)
    @camera.transform_component.position.z = 
      @camz + (xbounce + ybounce) / @transform.scale.z

    update_cursor_color
  end

  def update_cursor_color
    @cursor ||= find_entity("TileCursor")

    curpos = @cursor.transform_component.position
    dx = Math.round(curpos.x - @pos.x)
    dy = Math.round(curpos.y - @pos.y)

    if !can_move?(dx, dy)
      @cursor.children.first.sprite_component.texture_index = 1
    else
      @cursor.children.first.sprite_component.texture_index = 0
    end
  end

  def mouse_move(e)
    set_mouse_mode
  end

  def set_kb_mode
    return if Config[:touch_mode]
    @cursor ||= find_entity("TileCursor")
    @cursor.children[0].sprite_component.visible = false
  end

  def set_mouse_mode
    return if Config[:touch_mode]
    @cursor ||= find_entity("TileCursor")
    @cursor.children[0].sprite_component.visible = true
  end

  def on_zombification(e)
    # TODO: Lose Condition
    Game.switch_level 'MainMenu'
  end

  register_component "PlayerControllerComponent"
end
