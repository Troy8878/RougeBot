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

    data["name"] = set_weapon_type(data, itemLevel)
    data["name"] = set_weapon_stats(data, itemLevel)

    Weapon.new data
  end

  def self.set_weapon_type(data, itemLevel)
    name = ""

    result = Random.die_roll 100

    if result > 80
      data["damage"][0] = 10
      data["damage"][1] = 15
      name = "Halberd"
    elsif result > 60
      data["damage"][0] = 8
      data["damage"][1] = 10
      name = "Spear"
    elsif result > 40
      data["damage"][0] = 6
      data["damage"][1] = 12
      name = "Sword"
    else
      data["damage"][0] = 5
      data["damage"][1] = 8
      name = "Dagger"
    end

    if (data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0]
    end

    data["value"] = data["value"] * itemLevel
    data["damage"][0] = data["damage"][0] * (itemLevel)
    data["damage"][1] = data["damage"][1] * (itemLevel)

    return name
  end

  def self.set_weapon_stats(data, itemLevel)

    name = data["name"]

    #Get Material
    result = Random.die_roll 100

    if result > 99
      name = "Diamond " + name
      data["durability"] = data["durability"] + 20
      data["damage"][0] = data["damage"][0] + 4
      data["damage"][1] = data["damage"][1] + 4
      data["value"] = data["value"] + 10
    elsif result > 90
      name = "Steel " + name
      data["durability"] = data["durability"] + 5
      data["damage"][0] = data["damage"][0] + 2
      data["damage"][1] = data["damage"][1] + 2
      data["value"] = data["value"] + 5
    elsif result > 70
      name = "Iron " + name
    elsif result > 50
      name = "Lead " + name
      data["damage"][0] = data["damage"][0] - 3
      data["damage"][1] = data["damage"][1] + 4
    elsif result > 30
      name = "Asbestos " + name
      data["durability"] = data["durability"] - 5
      data["damage"][0] = data["damage"][0] + 1
      data["damage"][1] = data["damage"][1] - 1
      data["value"] = data["value"] - 5
    elsif result > 20
      name = "Plastic " + name
      data["durability"] = data["durability"] + 10
      data["damage"][0] = data["damage"][0] - 3
      data["damage"][1] = data["damage"][1] - 4
    elsif result > 10
      name = "Wooden " + name
      data["durability"] = data["durability"] - 10
      data["damage"][0] = data["damage"][0] - 2
      data["damage"][1] = data["damage"][1] - 3
      data["value"] = data["value"] - 10
    #elsif result > 10
    #  name = "Brick " + name
    #  data["durability"] = data["durability"] - 0
    #  data["damage"][0] = data["damage"][0] + 0
    #  data["damage"][1] = data["damage"][1] - 0
    #  data["value"] = data["value"] - 20
    #elsif result > 5
    #  name = "Gelatin " + name
    # data["durability"] = data["durability"] - 20
    #  data["damage"][0] = data["damage"][0] - 10
    #  data["damage"][1] = data["damage"][1] - 10
    #  data["value"] = data["value"] - 30
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
    
    if result > 90
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
    else
      name = name
    end


    #Get Element
    result = Random.die_roll 100

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
