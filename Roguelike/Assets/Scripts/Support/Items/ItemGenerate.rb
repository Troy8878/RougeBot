#######################
# ItemGenerate.rb
# Leonardo Saikali
# Created 2014/10/28
#######################

module ItemGenerate

  def self.generate_weapon(data, itemLevel)
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

    Weapon.new data
  end

  def self.set_weapon_type(data)
    name = ""

    result = Random.die_roll 100

    if result > 90
      data["damage"][0] = 8
      data["damage"][1] = 13
      name = "Halberd"
    elsif result > 80
      data["damage"][0] = 7
      data["damage"][1] = 10
      name = "Spear"
    elsif result > 70
      data["damage"][0] = 5
      data["damage"][1] = 8
      name = "Pitchfork"
    elsif result > 60
      data["damage"][0] = 4
      data["damage"][1] = 7
      data["durability"] += 10
      name = "Quaterstaff"
    elsif result > 50
      data["damage"][0] = 5
      data["damage"][1] = 10
      name = "Saber"
    elsif result > 40
      data["damage"][0] = 5
      data["damage"][1] = 9
      data["durability"] += 5
      name = "Hammer"
    elsif result > 30
      data["damage"][0] = 6
      data["damage"][1] = 11
      data["durability"] -= 5
      name = "Axe"
    elsif result > 20
      data["damage"][0] = 9
      data["damage"][1] = 14
      data["durability"] -= 5
      name = "Chainsaw"
    elsif result > 10
      data["damage"][0] = 3
      data["damage"][1] = 4
      data["durability"] -= 10
      name = "Manual Drill"
    #elsif result > 40
    #  data["damage"][0] = 6
    #  data["damage"][1] = 12
    #  data["durability"] -= 10
    #  name = "Diesel Drill"
    #elsif result > 40
    #  data["damage"][0] = 6
    #  data["damage"][1] = 12
    #  data["durability"] -= 10
    #  name = "Steam Drill"
    #elsif result > 40
    #  data["damage"][0] = 6
    #  data["damage"][1] = 12
    #  data["durability"] -= 10
    #  name = "Electric Drill"
    #elsif result > 40
    #  data["damage"][0] = 6
    #  data["damage"][1] = 12
    #  data["durability"] -= 10
    #  name = "Atomic Drill"
    #elsif result > 40
    #  data["damage"][0] = 33
    #  data["damage"][1] = 39
    #  data["durability"] -= 30
    #  name = "Plasma Drill"
    else
      data["damage"][0] = 4
      data["damage"][1] = 9
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
      name = "Antimatter " + name
      data["durability"] = data["durability"] - 15
      data["damage"][0] = data["damage"][0] + 6
      data["damage"][1] = data["damage"][1] + 6
      data["value"] = data["value"] + 15
      # Should be of the Explosive element
    elsif result > 96
      name = "Diamond " + name
      data["durability"] = data["durability"] + 20
      data["damage"][0] = data["damage"][0] + 4
      data["damage"][1] = data["damage"][1] + 4
      data["value"] = data["value"] + 20
      # Should be of the Physical element
    elsif result > 90
      name = "Plasmic " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] + 3
      data["damage"][1] = data["damage"][1] + 6
      # Should be of the Flaming element
    elsif result > 80
      name = "Steel " + name
      data["durability"] = data["durability"] + 5
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] + 5
      # Should be of the Physical element
    elsif result > 75
      name = "Magma " + name
      data["durability"] = data["durability"] - 10
      data["damage"][1] = data["damage"][1] + 4
      # Should be of the Flaming element
    elsif result > 70
      name = "Lead " + name
      data["damage"][0] = data["damage"][0] - 3
      data["damage"][1] = data["damage"][1] + 4
      # Should be of the Physical element
    elsif result > 65
      name = "Embroidered " + name
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
      data["value"] = data["value"] + 10
      # Should be of the Physical element
    elsif result > 60
      name = "Asbestos " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] - 5
      # Should be of the Physical element
    elsif result > 55
      name = "Anbaricum " + name
      data["damage"][1] = data["damage"][1] + 2
      # Should be of the Zapping element
    elsif result > 53
      name = "Gold " + name
      data["durability"] = data["durability"] - 10
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 3
      data["value"] = data["value"] + 20
      # Should be of the Physical element
    elsif result > 48
      name = "Obsidian " + name
      data["durability"] = data["durability"] + 10
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 2
      # Should be of the Physical element
    elsif result > 44
      name = "Frozen " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] - 1
      # Should be of the Freezing & Physical elements
    elsif result > 40
      name = "Iron " + name
      # Should be of the Physical element
    elsif result > 35
      name = "Uranium " + name
      data["durability"] = data["durability"] - 10
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] + 1
      # Should be of the Radioactive & Physical elements
    elsif result > 30
      name = "Plastic " + name
      data["durability"] = data["durability"] + 20
      data["damage"][0] = data["damage"][0] - 3
      data["damage"][1] = data["damage"][1] - 4
      # Should be of the Physical element
    elsif result > 29
      name = "Plastic Explosive " + name
      data["damage"][1] = data["damage"][1] - 1
      # Should be of the Explosive element
    elsif result > 26
      name = "Mahogany " + name
      data["durability"] = data["durability"] - 10
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 3
      data["value"] = data["value"] + 10
    elsif result > 20
      name = "Wooden " + name
      data["durability"] = data["durability"] - 10
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 3
      data["value"] = data["value"] - 10
      # Should be of the Physical element
    elsif result > 10
      name = "Brick " + name
      data["durability"] = data["durability"] - 0
      data["damage"][0] = data["damage"][0] + 0
      data["damage"][1] = data["damage"][1] - 0
      data["value"] = data["value"] - 20
      # Should be of the Physical element
    elsif result > 5
      name = "Gelatin " + name
      data["durability"] = data["durability"] - 20
      data["damage"][0] = data["damage"][0] - 10
      data["damage"][1] = data["damage"][1] - 10
      data["value"] = data["value"] - 30
      # Should be of the Radioactive element
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
    elsif result > 90
      name = "Sturdy " + name
      data["durability"] = data["durability"] + 10
      data["value"] = data["value"] + 2
    elsif result > 80
      name = "Shoddy " + name
      data["durability"] = data["durability"] - 10
      data["value"] = data["value"] - 2
    elsif result > 70
      name = "Shiny " + name
      data["value"] = data["value"] * 2
    elsif result > 60
      name = "Rusty " + name
      data["value"] = data["value"] / 2
    elsif result > 50
      name = "Sharp " + name
      data["damage"][0] = data["damage"][0] + 2
      data["value"] = data["value"] + 2
    elsif result > 40
      name = "Dull " + name
      data["damage"][1] = data["damage"][1] - 2
      data["value"] = data["value"] -2
    elsif result > 35
      name = "Laser-Guided " + name
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] + 1
      data["value"] = data["value"] + 3
    elsif result > 30
      name = "Torrented " + name
      data["damage"][0] = data["damage"][0] - 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] = 0
    else
      name = name
    end


    #Get Element
    result = Random.die_roll 100
    #result += (itemLevel * 5)

    if result > 95
      name = name + " of Radioactivity"
      data["value"] = data["value"] + 10
    elsif result > 85
      name = name + " of Exploding"
      data["value"] = data["value"] + 8
    elsif result > 73
      name = name + " of Freezing"
      data["value"] = data["value"] + 6
    elsif result > 60
      name = name + " of Zapping"
      data["value"] = data["value"] + 4
    elsif result > 45
      name = name + " of Flames"
      data["value"] = data["value"] + 2
    else
      name = name
    end

    return name
	end
end
