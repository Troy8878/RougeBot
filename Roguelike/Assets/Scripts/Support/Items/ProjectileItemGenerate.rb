#######################
# ProjectileItemGenerate.rb
# Troy
# Created 2014/12/02
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

module ProjectileItemGenerate

  def self.generate_projectile_item(data, itemLevel)
    result = Random.die_roll 100
    
    # Still in favour of having item levels seperate from stats.
    # Inventory management will be a mess on the player's end like this.
    
    if(result > 90)
      itemLevel = itemLevel + 1
    end
    
    # Set default Projectile stats (see Projectile.rb)

    data["damage"] ||= [5, 10]
    data["durability"] ||= 20
    data["value"] ||= 5
    data["speed"] ||= 2
    data["element1"] ||= 0
    
    # The actual randomization happens here
    
    data["name"] = set_projectile_type(data)
    data["name"] = set_projectile_stats(data, itemLevel)

    # Don't let projectiles heal enemies!
    
    if (data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0]
    end

    # Place where level affects things
    
    data["value"] = data["value"] * itemLevel

    data["damage"][0] = data["damage"][0] * itemLevel
    data["damage"][1] = data["damage"][1] * itemLevel
    
    # And here's the actual projectile item creation

    prji = Projectile.new data
    prji.item_level = itemLevel
    return prji
  end

  def self.set_projectile_type(data)
    name = ""

    # This system is temporary; a more detailed/balanced one will be made later
    
    result = Random.die_roll 100

    if result > 98
      data["damage"][0] = 13
      data["damage"][1] = 23
      data["durability"] -= 10
      data["speed"] = 3
      data["value"] += 10
      data["element1"] = Projectile::EXPLOSIVE_ELEM
      name = "Missile"
      
    elsif result > 95
      data["damage"][0] = 13
      data["damage"][1] = 19
      data["durability"] -= 5
      data["speed"] = 2
      data["value"] += 4
      data["element1"] = Projectile::EXPLOSIVE_ELEM
      name = "Mine"
      
    elsif result > 90
      data["damage"][0] = 9
      data["damage"][1] = 21
      data["durability"] -= 5
      data["speed"] = 2
      data["value"] += 7
      data["element1"] = Projectile::EXPLOSIVE_ELEM
      name = "Bomb"
      
    elsif result > 85
      data["damage"][0] = 12
      data["damage"][1] = 16
      data["durability"] += 0
      data["speed"] = 4
      data["value"] += 3
      data["element1"] = Projectile::RADIOACTIVE_ELEM
      name = "Macro Pulsewave"
      
    elsif result > 80
      data["damage"][0] = 8
      data["damage"][1] = 18
      data["durability"] -= 0
      data["speed"] = 2
      data["value"] += 3
      data["element1"] = Projectile::FIRE_ELEM
      name = "Fireball"
      
    elsif result > 75
      data["damage"][0] = 9
      data["damage"][1] = 16
      data["durability"] -= 5
      data["speed"] = 5
      data["value"] += 3
      data["element1"] = Projectile::ELECTRIC_ELEM
      name = "Ball Lightning"
      
    elsif result > 70
      data["damage"][0] = 7
      data["damage"][1] = 17
      data["durability"] -= 0
      data["speed"] = 2
      data["value"] += 2
      data["element1"] = Projectile::ICE_ELEM
      name = "Iceball"
      
    elsif result > 60
      data["damage"][0] = 7
      data["damage"][1] = 13
      data["durability"] -= 5
      data["speed"] = 5
      data["value"] += 1
      data["element1"] = Projectile::ELECTRIC_ELEM
      name = "Electric Bolt"
      
    elsif result > 55
      data["damage"][0] = 7
      data["damage"][1] = 11
      data["durability"] += 5
      data["speed"] = 4
      data["value"] += 1
      data["element1"] = Projectile::RADIOACTIVE_ELEM
      name = "Micro Pulsewave"
      
    elsif result > 50
      data["damage"][0] = 5
      data["damage"][1] = 13
      data["durability"] += 10
      data["speed"] = 2
      data["value"] += 0
      data["element1"] = Projectile::RADIOACTIVE_ELEM
      name = "Glowing Blob"
      
    elsif result > 40
      data["damage"][0] = 5
      data["damage"][1] = 17
      data["durability"] -= 5
      data["speed"] = 4
      data["value"] += 2
      data["element1"] = Projectile::FIRE_ELEM
      name = "Fire Arrow"
      
    elsif result > 35
      data["damage"][0] = 6
      data["damage"][1] = 16
      data["durability"] -= 5
      data["speed"] = 3
      data["value"] += 1
      data["element1"] = Projectile::PHYSICAL_ELEM
      name = "Shiruken"
      
    elsif result > 30
      data["damage"][0] = 6
      data["damage"][1] = 15
      data["durability"] -= 5
      data["speed"] = 4
      data["value"] += 0
      data["element1"] = Projectile::ICE_ELEM
      name = "Ice Arrow"
      
    elsif result > 20
      data["damage"][0] = 5
      data["damage"][1] = 16
      data["durability"] -= 0
      data["speed"] = 4
      data["value"] -= 0
      data["element1"] = Projectile::PHYSICAL_ELEM
      name = "Arrow"
      
    else
      data["damage"][0] = 4
      data["damage"][1] = 12
      data["durability"] += 10
      data["value"] -= 2
      data["speed"] = 2
      data["element1"] = Projectile::PHYSICAL_ELEM
      name = "Rock"
      
    end

    return name
  end

  def self.set_projectile_stats(data, itemLevel)

    name = data["name"]
    
    # The minimum speed of all projectiles
    MIN_SPEED = 2
    
    # Get Material
    result = Random.die_roll 100
    #result += (itemLevel * 5)

    if result > 99
      name = "Atomic " + name
      data["durability"] = data["durability"] - 12
      data["damage"][0] = data["damage"][0] + 4
      data["damage"][1] = data["damage"][1] + 4
      data["value"] = data["value"] + 20
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::RADIOACTIVE_ELEM
    elsif result > 95
      name = "Explosive " + name
      data["durability"] = data["durability"] - 9
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 3
      data["value"] = data["value"] + 6
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::EXPLOSIVE_ELEM
    elsif result > 90
      name = "Thermite " + name
      data["durability"] = data["durability"] - 7
      data["damage"][0] = data["damage"][0] + 0
      data["damage"][1] = data["damage"][1] + 4
      data["value"] = data["value"] + 3
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::FIRE_ELEM
    elsif result > 85
      name = "Cryogenic " + name
      data["durability"] = data["durability"] - 2
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] + 3
      data["value"] = data["value"] + 3
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::ICE_ELEM
    elsif result > 80
      name = "Electrified " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] + 0
      data["damage"][1] = data["damage"][1] + 3
      data["value"] = data["value"] + 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 1
      end
      data["element2"] = Projectile::ELECTRIC_ELEM
    elsif result > 75
      name = "Steel " + name
      data["durability"] = data["durability"] + 5
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
      data["value"] = data["value"] + 3
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
    elsif result > 70
      name = "Lead " + name
      data["durability"] = data["durability"] + 0
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] - 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] - 1
      end
      data["element2"] = Projectile::PHYSICAL_ELEM
    elsif result > 65
      name = "Flaming " + name
      data["durability"] = data["durability"] - 4
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] + 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::FIRE_ELEM
    elsif result > 60
      name = "Asbestos " + name
      data["durability"] = data["durability"] - 1
      data["damage"][1] = data["damage"][1] + 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::NONE
    elsif result > 55
      name = "Tin " + name
      data["durability"] = data["durability"] - 0
      data["damage"][0] = data["damage"][0] + 0
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] - 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::NONE
    elsif result > 50
      name = "Uranium " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 0
      data["value"] = data["value"] + 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::RADIOACTIVE_ELEM
    elsif result > 45
      name = "Volatile " + name
      data["durability"] = data["durability"] - 7
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] - 1
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::EXPLOSIVE_ELEM
    elsif result > 33
      name = "Iron " + name
      # These & all zeros are here in case we need to quickly change values
      data["durability"] = data["durability"] + 0
      data["damage"][0] = data["damage"][0] - 0
      data["damage"][1] = data["damage"][1] - 0
      data["value"] = data["value"] - 0
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      data["element2"] = Projectile::PHYSICAL_ELEM
      end
    elsif result > 30
      name = "Gold " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] + 10
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::NONE
    elsif result > 20
      name = "Brick " + name
      data["durability"] = data["durability"] + 4
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 3
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
    elsif result > 10
      name = "Plastic " + name
      data["durability"] = data["durability"] + 8
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] - 2
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 1
      end
      data["element2"] = Projectile::NONE
    elsif result > 5
      name = "Mahogany " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] + 4
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::NONE
    elsif result > 4
      name = "Wooden " + name
      data["durability"] = data["durability"] - 3
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 2
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 0
      end
      data["element2"] = Projectile::NONE
    else
      name = "The Alleged " + name
      data["durability"] = 0
      data["damage"][0] = 0
      data["damage"][1] = 0
      data["value"] = 0
      if(data["speed"] > MIN_SPEED)
        data["speed"] = 2
      end
      data["element2"] = Projectile::NONE
      return name
    end

    # Get Attribute
    result = Random.die_roll 100
    
    # An example of the type of level system I'd like to implement:
    # result += (itemLevel * 5)
    
    if result > 99
      name = "Great " + name
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 3
      data["value"] = data["value"] + 3
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] + 1
      end
    elsif result > 88
      #name = "Sturdy " + name # Just get more of what there is
      data["durability"] = data["durability"] + 10
    elsif result > 77
      name = "Crude " + name
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] - 2
      if(data["speed"] > MIN_SPEED)
        data["speed"] = data["speed"] - 1
      end
    elsif result > 66
      name = "Shiny " + name
      data["value"] = data["value"] * 2
    elsif result > 55
      name = "Rusty " + name
      data["damage"][0] = data["damage"][0] - 2
      data["value"] = data["value"] - 2
      if(data["speed"] > 0)
        data["speed"] = data["speed"] + 0
      end
    elsif result > 44
      name = "Sharp " + name
      data["damage"][0] = data["damage"][0] + 2
      data["value"] = data["value"] + 2
      if(data["speed"] > 0)
        data["speed"] = data["speed"] + 2
      end
    elsif result > 33
      name = "Dull " + name
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] - 2
      if(data["speed"] > 0)
        data["speed"] = data["speed"] + 0
      end
    elsif result > 22
      name = "Laser-Guided " + name
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
      data["value"] = data["value"] + 3
      if(data["speed"] > 0)
        data["speed"] = data["speed"] + 2
      end
    else
      name = name
    end
    
    # Insure that no stat is below 0
    
    if(data["damage"][0] < 1)
      data["damage"][0] = 1
    end
    
    if(data["damage"][1] < 1)
      data["damage"][1] = 1
    end
    
    if(data["durability"] < 1)
      data["durability"] = 1
    end
    
    if(data["value"] < 1)
      data["value"] = 1
    end
    
    if(data["speed"] < 0)
      data["speed"] = 0 # In other words, a mine
    end
    
    # There should also be at least one element
    if(data["element1"] == NONE && data["element2"] == NONE)
      data["element1"] = PHYSICAL_ELEM
    end

    #Don't let max damage be below min damage
    if(data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0] + 1
    end

    #Unlike Weapons, Projectiles set element1 in their type

    return name
	end
end