#######################
# ItemGenerate.rb
# Leonardo Saikali
# Created 2014/10/28
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

module ItemGenerate

  def self.generate_weapon(data, itemLevel)
    result = Random.die_roll 100

    if(result > 90)
      itemLevel = itemLevel + 1
    end

    data["damage"] ||= [5, 10]
    data["durability"] ||= 20
    data["value"] ||= 10

    data["name"] = set_weapon_type(data)
    data["name"] = set_weapon_stats(data, itemLevel)

    if (data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0]
    end

    data["value"] = data["value"] * itemLevel

    data["damage"][0] = data["damage"][0] * itemLevel
    data["damage"][1] = data["damage"][1] * itemLevel

    wep = Weapon.new data
    wep.item_level = itemLevel
    return wep
  end

  def self.set_weapon_type(data)
    name = ""

    result = Random.die_roll 100

    if result > 80
      data["damage"][0] = 6
      data["damage"][1] = 9
      name = "Spear"
    elsif result > 50
      data["damage"][0] = 5
      data["damage"][1] = 10
      name = "Saber"
    elsif result > 20
      data["damage"][0] = 4
      data["damage"][1] = 11
      data["durability"] -= 5
      name = "Axe"
    else
      data["damage"][0] = 7
      data["damage"][1] = 11
      name = "Dagger"
    end

    return name
  end

  def self.set_weapon_stats(data, itemLevel)

    name = data["name"]

    #Get Material
    result = Random.die_roll 100
    #result += (itemLevel * 5)

    if result > 99
      name = "Diamond " + name
      data["durability"] = data["durability"] + 20
      data["damage"][0] = data["damage"][0] + 4
      data["damage"][1] = data["damage"][1] + 4
      data["value"] = data["value"] + 20
    elsif result > 80
      name = "Steel " + name
      data["durability"] = data["durability"] + 5
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] + 5
    elsif result > 70
      name = "Obsidian " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
    elsif result > 60
      name = "Lead " + name
      data["durability"] = data["durability"] + 5
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] + 2
    elsif result > 50
      name = "Asbestos " + name
      data["damage"][1] = data["damage"][1] + 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 1
    elsif result > 24
      name = "Iron " + name
    elsif result > 11
      name = "Plastic " + name
      data["durability"] = data["durability"] + 20
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 2
      # Should be of the Physical element
    elsif result > 10
      name = "Mahogany " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] + 10
      # Should be of the Physical element
    elsif result > 4
      name = "Wooden " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 3
      # Should be of the Zapping element
    else
      name = "The Alleged " + name
      data["durability"] = 0
      data["damage"][0] = 0
      data["damage"][1] = 0
      data["value"] = 0
      return name
    end


    #Get Attribute
    result = Random.die_roll 100
    #result += (itemLevel * 5)
    
    if result > 99
      name = "Great " + name
      data["durability"] = data["durability"] + 15
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] + 2
    elsif result > 88
      name = "Sturdy " + name
      data["durability"] = data["durability"] + 10
      data["value"] = data["value"] + 2
    elsif result > 77
      name = "Shoddy " + name
      data["durability"] = data["durability"] - 10
      data["value"] = data["value"] - 2
    elsif result > 66
      name = "Shiny " + name
      data["value"] = data["value"] * 2
    elsif result > 55
      name = "Rusty " + name
      data["damage"][0] = data["damage"][0] - 2
      data["value"] = data["value"] - 2
    elsif result > 44
      name = "Sharp " + name
      data["damage"][0] = data["damage"][0] + 2
      data["value"] = data["value"] + 2
    elsif result > 33
      name = "Dull " + name
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] -2
    elsif result > 22
      name = "Laser-Guided " + name
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
      data["value"] = data["value"] + 3
    else
      name = name
    end


    data["damage"][0] = data["damage"][0] + 1
    data["damage"][1] = data["damage"][1] + 1


    #Get Element
    result = Random.die_roll 100
    #result += (itemLevel * 5)

    if result > 95
      name = name + " of Radioactivity"
      data["value"] = data["value"] + 10
      data["damage_type"] = :radioactive
    elsif result > 85
      name = name + " of Exploding"
      data["value"] = data["value"] + 8
      data["damage_type"] = :exploding
    elsif result > 73
      name = name + " of Freezing"
      data["value"] = data["value"] + 6
      data["damage_type"] = :freezing
    elsif result > 60
      name = name + " of Zapping"
      data["value"] = data["value"] + 4
      data["damage_type"] = :zapping
    elsif result > 45
      name = name + " of Flames"
      data["value"] = data["value"] + 2
      data["damage_type"] = :flame
    else
      name = name
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
    end

    return name
	end
end
