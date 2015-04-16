##############################
# PlayerControllerComponent.rb
# Claire Robsahm, Leonardo Saikali
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

GAME_STATE[:score] = 0

class PlayerControllerComponent < ComponentBase
  include Actor
  dependency "TransformComponent", "PositionComponent"

  attr_reader :pos

  MIN_MOVE_TIME = 0.2

  serialized_input do |p|
    p.dependency "TransformComponent"
    p.dependency "PositionComponent"
  end

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @pos = self.owner.position_component.position

    @vp_width = 4
    @vp_height = 4

    @logic_initialized = false
    @logic_cooldown = 0

    self.register_event :move, :on_move
    self.register_event :fire, :fire
    self.register_event :update, :first_update
    self.register_event :attack, :attack
    self.register_event :swing_weapon, :swing_weapon

    self.register_event :double_click, :mouse_down
    self.register_event :mouse_down, :mouse_down

    self.register_event :mouse_move, :mouse_move
    self.register_event :on_pause, :on_pause
    self.register_event :player_use, :player_use
    self.register_event :player_drop, :player_drop
    self.register_event :player_eat, :player_eat

    #self.register_event :skip_floor, :skip_floor
    #self.register_event :skip_to_win, :skip_to_win

    self.register_event :ai_complete, :ai_complete
  end

  def on_pause(val)
    if val
      @paused = true
    else
      @paused = false
    end
  end

  def player_use(e)
    # Check if the player is on the stairs
    if @pos.near? STAIR_POSITION, 0.5
      # Make sure we aren't on the last floor already.
      if GAME_STATE[:tutorial].is_a? Fixnum
        if GAME_STATE[:tutorial] == -5
          Game.switch_level "Credits"
        else
          GAME_STATE[:floor] += 1
          GAME_STATE[:tutorial] += 1
          Game.reload_level
        end
      elsif GAME_STATE[:floor] == $DungeonLength && GAME_STATE[:endless] != true
        Config[:dungeon_completed] = true
        if GAME_STATE[:act2] == true
          Config[:act2_completed] = true
        end
        Config.save

        Game.switch_level "Victory"
      else
        GAME_STATE[:floor] += 1
        Game.reload_level
      end
      return
    end

    tile = current_tile

    if tile.door
      tut = tile.door.tutnum
      if tut == -2
        play_dungeon!(1, nil)
      else
        play_dungeon!(1, tut)
      end
    end

    if tile.item?
      if PLAYER_INVENTORY.room_in_inventory == true
        item = tile.pickup_item
        PLAYER_INVENTORY.pickup item
        Event.raise_event :combat_log, "Picked up #{item.name}"
      else
        Event.raise_event :combat_log, "INVENTORY FULL"
      end
    end
  end

  def player_drop(e)
    find_entity("Hotbar").raise_event :drop_selected, current_tile
  end

  def player_eat(e)
    hsh = {}
    find_entity("Hotbar").raise_event :extract_selected, hsh

    item = hsh[:item]

    if item == nil
      return
    end

    self.owner.defense_component.heal item.value
    GAME_STATE[:score] += item.value
    find_entity(0).raise_event :score_change, nil
  end

  def skip_floor(e)
  end

  def skip_to_win(e)
    Game.switch_level "Victory"
  end

  MOVE_ORIENTATIONS = {
    left: [-1, 0],
    right: [1, 0],
    up: [0, 1],
    down: [0, -1]
  }

  def move(x, y)
    return if @paused

    unless can_move_more_than_one? x, y
      return unless x == 0 && y == 0
    end

    if x == 0 && y == 0 && @logic_cooldown > 0
      return
    end

    if @blocked_reason == BLOCKED_BY_ACTOR
      @logic_cooldown = 0.5
    else
      @pos.x += x
      @pos.y += y
      @logic_cooldown = 0.2

      actor_moved
    end

    yield_to_enemies

    if @pos.near? STAIR_POSITION, 0.5
      display_down
    else
      hide_down
    end
  end

  def display_down
    #TODO Show that the player can go down stairs
  end

  def hide_down
    #TODO Show that the player can't go down stairs
  end

  def enemies_be_ded?
    search_entities('').select(&:enemy_logic_component).empty?
  end

  def yield_to_enemies
    seq = self.owner.action_sequence :delay_logic

    if enemies_nearby?
      @logic_cooldown += 0.1
      seq.delay(0.1)
    else
      return
    end

    seq.delay(0) # ensure at _minimum_ 2 frames go by
    seq.once do
      if @logic_initialized
        AISystem.instance.update_snapshot
        Event.raise_event :logic_update, self.owner
      end
    end
  end

  def on_move(e)
    move *e
  end

  # For autoplay
  def ai_complete(e)
    yield_to_enemies
  end

  def enemies_nearby?
    enemies = find_entity(0).children.select(&:enemy_logic_component)
    enemies = enemies.select do |enemy|
      !enemy.enemy_logic_component.stationary
    end

    enemies.each do |enemy|
      epos = enemy.transform_component.position
      if Math.abs(@pos.x - epos.x) <= @vp_width && Math.abs(@pos.y - epos.y) <= @vp_height
        return true
      end
    end

    return false
  end

  ####################################
  # Stuffs Troy/Leo added for combat #
  ####################################

  ATTACK_ANIM_ROTATIONS = {
    left: -Math::PI / 2,
    up: Math::PI,
    right: Math::PI / 2,
    down: 0
  }

  ELEMENT_TINT = {
    "flame"       => Vector.new(1.0, 0.6,  0.6, 1.0),
    "freezing"    => Vector.new(1.0, 1.0,  1.0, 1.0),
    "radioactive" => Vector.new(0.6, 1.0, 0.6, 1.0),
    "exploding"   => Vector.new(1.0, 1.0,  0.6, 1.0),
    "zapping"     => Vector.new(0.6, 1.0,  1.0, 1.0),
    "physical"    => Vector.new(0.8, 0.8,  0.8, 1.0)
  }

  def swing_weapon(e)

    return if @paused
    return if @logic_cooldown > 0

    orientation = e

    weapon = self.owner.inventory_component.inventory.equipment[:weapon]

    element = weapon ? weapon.element : "physical"
    weaponType = weapon ? weapon.weaponType : Weapon::DAGGER_TYPE

    element ||= "physical"

    weaponType ||= Weapon::DAGGER_TYPE

    attacks = Weapon::ATTACK_SQUARES[weaponType][orientation]

    attacks.each do |atk|
      attack atk
    end

    anim_arch = "WeaponAnimations/#{Weapon::ANIM_NAMES[weaponType]}"
    anim_entity = owner.create_child(
      archetype: anim_arch
    )
    anim_entity.transform_component.rotation.z = ATTACK_ANIM_ROTATIONS[orientation]
    anim_entity.transform_component.position += Vector.new(*MOVE_ORIENTATIONS[orientation])
    anim_entity.sprite_component.tint = ELEMENT_TINT["#{element}"]

    #Sound for attack
    SFX::SLASH.play

    @logic_cooldown = 0.5
    
    actor_moved
    yield_to_enemies
  end

  def attack(e)

    x = e[0]
    y = e[1]
    
    # For now, don't attack empty tiles.
    # Once AoE classes are in effect we won't need to worry

    cant_attack = can_hit? x, y
    return if @blocked_reason != BLOCKED_BY_ACTOR
    return if cant_attack

    # The actual attack logic.  Will be replaced with the AoE class
    self.owner.attack_component.do_attack @move_tile.actor


  end

  def fire(e)

    return if @paused
    return if @logic_cooldown > 0

    x = e[0]
    y = e[1]

    unless can_fire? x, y
      return if @blocked_reason != BLOCKED_BY_ACTOR
      # KEEP THIS!!! We always want to be able to shoot (enemy) actors!
    end

    # We actually don't just want to deal damage if an actor is hit.
    # Fireable weapons can have affects other than base damage

    find_entity(0).create_child(
      archetype: "PlayerProjectiles/Arrow",
      # We'll need to make this the wielded ranged weapon
      # once wielding is implemented
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
    dir = Vector.new(dx, dy)

    if e.button == MouseState::LBUTTON
      move dx, dy
    elsif e.button == MouseState::RBUTTON
      if dir.near? Vector.up, 0.1
        swing_weapon :up
      elsif dir.near? Vector.down, 0.1
        swing_weapon :down
      elsif dir.near? Vector.left, 0.1
        swing_weapon :left
      elsif dir.near? Vector.right, 0.1
        swing_weapon :right
      elsif dir.near? Vector.zero, 0.1
        player_use nil
      end
    end
  end

  def first_update(e)
    shape = MapItem::ELLIPSE
    actor_init(shape, "Yellow")

    move 0, 0

    @camera = find_entity("CameraRoot")
    @camera.parent = self.owner
    @camz = @camera.transform_component.position.z

    @logic_initialized = true

    if GAME_STATE[:tutorial] == -1
      owner.local_find("HealthBar").zombify!
      owner.local_find("HealthDisplay").zombify!
    end

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
  end

  def on_zombification(e)
    find_entity(0).raise_event :ai_is_kill, nil

    find_entity("Minimap").zombify!
    find_entity("TileCursor").zombify!

    overlay = owner.local_find("DefeatOverlay")
    overlay.add_component "ButtonComponent", {
      "render_target" => "HUDLayer"
    }

    transient = owner.parent.create_child
    transient.add_component "TransformComponent", {
      "position" => owner.transform_component.position.to_a,
      "scale" => owner.transform_component.scale.to_a
    }
    transient.add_child owner.local_find("CameraRoot")

    tint = overlay.sprite_component.tint
    tint.w = 0.49

    ntint = tint.dup
    ntint.w = 1

    MUSIC.play(nil)
    SFX::FAILURE.play

    seq = transient.action_sequence :display_defeat
    seq.interpolate tint, to: ntint, over: 5.seconds
    seq.once do
      MUSIC.play(MUSIC::TUT)
      Game.switch_level "GameOver"
    end
  end

  register_component
end

def expunge_inventory
  PLAYER_INVENTORY.inventory.each_with_index do |item, index|
    roll = Random.die_roll 9
    if roll > 2
      PLAYER_INVENTORY.inventory[index] = nil
    end
  end
end
