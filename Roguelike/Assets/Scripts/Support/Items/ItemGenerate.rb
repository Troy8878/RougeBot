#######################
# ItemGenerate.rb
# Leonardo Saikali
# Created 2014/10/28
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

module ItemGenerate

  def self.generate_weapon(data, itemLevel)
    
    if GAME_STATE[:act2] == true
      itemLevel = itemLevel + 5
    elsif GAME_STATE[:endless] == true
      itemLevel = Random.die_roll 10
    end

    result = Random.die_roll 100

    if(result > 90)
      itemLevel = itemLevel + 1
    end

    data["damage"] ||= [5, 10]
    data["durability"] ||= 20
    data["value"] ||= 10

    data["name"] = set_weapon_type(data)
    newLevel = set_weapon_stats(data, itemLevel)

    if (data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0]
    end

    wep = Weapon.new data
    wep.item_level = newLevel

    return wep
  end

  def self.generate_mundane_weapon(data, itemLevel)

    if GAME_STATE[:act2] == true
      itemLevel = itemLevel + 5
    elsif GAME_STATE[:endless] == true
      itemLevel = Random.die_roll 10
    end

    data["damage"] ||= [5, 10]
    data["durability"] ||= 20
    data["value"] ||= 10

    data["attributes"] ||= []

    data["name"] = set_weapon_type(data)

    self.add_material(data, itemLevel)

    if (data["damage"][1] < data["damage"][0])
      data["damage"][1] = data["damage"][0]
    end

    wep = Weapon.new data
    wep.item_level = itemLevel

    return wep
  end    

  def self.modify_weapon_stats(data, pass_name, min_dam, max_dam, value, past)
    if past == false
      data["name"] = pass_name + data["name"]
    else
      data["name"] =  data["name"] + pass_name
    end

    data["damage"][0] = data["damage"][0] + min_dam
    data["damage"][1] = data["damage"][1] + max_dam
    data["value"] = data["value"] + value
  end

  def self.set_weapon_type(data)
    name = ""

    result = Random.die_roll 100
	
	  if result > 75
      data["damage"][0] = 6
      data["damage"][1] = 12
      data["weaponType"] = Weapon::SPEAR_TYPE

      name = "Spear"
    elsif result > 50
      data["damage"][0] = 8
      data["damage"][1] = 10
      data["weaponType"] = Weapon::SABER_TYPE

      name = "Saber"
    elsif result > 20
      data["damage"][0] = 4
      data["damage"][1] = 10
      data["weaponType"] = Weapon::AXE_TYPE

      name = "Axe"
    else
      data["damage"][0] = 10
      data["damage"][1] = 12
      data["weaponType"] = Weapon::DAGGER_TYPE

      name = "Dagger"
    end

    return name
  end

  def self.add_material(data, result)
    if result == 1
      self.modify_weapon_stats(data, "Wooden ", 0, 0, 0, false)

      data["attributes"] << ItemAttribute.new(:material, "Wooden")
    elsif result == 2
      self.modify_weapon_stats(data, "Plastic ", 1, 1, 5, false)

      data["attributes"] << ItemAttribute.new(:material, "Plastic")

    elsif result == 3
      self.modify_weapon_stats(data, "Lead ", 2, 4, 5, false)

      data["attributes"] << ItemAttribute.new(:material, "Lead")

    elsif result == 4
      self.modify_weapon_stats(data, "Asbestos ", 4, 5, 10, false)

      data["attributes"] << ItemAttribute.new(:material, "Asbestos")

    elsif result == 5
      self.modify_weapon_stats(data, "Iron ", 6, 6, 10, false)

      data["attributes"] << ItemAttribute.new(:material, "Iron")

    elsif result == 6
      self.modify_weapon_stats(data, "Obsidian ", 8, 10, 15, false)

      data["attributes"] << ItemAttribute.new(:material, "Obsidian")

    elsif result == 7
      self.modify_weapon_stats(data, "Steel ", 10, 12, 20, false)

      data["attributes"] << ItemAttribute.new(:material, "Steel")

    elsif result == 8
      self.modify_weapon_stats(data, "Mahogany ", 0, 0, 40, false)

      data["attributes"] << ItemAttribute.new(:material, "Mahogany")

    elsif result == 9
      self.modify_weapon_stats(data, "Diamond ", 14, 18, 30, false)

      data["attributes"] << ItemAttribute.new(:material, "Diamond")

    elsif result >= 10
      self.modify_weapon_stats(data, "Anti-Matter ", 16, 22, 50, false)

      data["attributes"] << ItemAttribute.new(:material, "Anti-Matter")
    end
  end

  def self.add_attribute(data, result)
    weaponMod = Random.die_roll(10 + result)
    levelMod = 0;

    if weaponMod < 3
      levelMod = -1;

      result = Random.die_roll(3)
      if result == 1
        self.modify_weapon_stats(data, "Dull ", -1, -1, 0, false)

        data["attributes"] << ItemAttribute.new(:augment, "Dull")
      elsif result == 2
        self.modify_weapon_stats(data, "Shoddy ", -2, -2, -2, false)

        data["attributes"] << ItemAttribute.new(:augment, "Shoddy")
      elsif result == 3
        self.modify_weapon_stats(data, "Rusty ", -3, -1, -5, false)

        data["attributes"] << ItemAttribute.new(:augment, "Rusty")
      end
    elsif weaponMod < 7
      levelMod = 0;
    elsif weaponMod < 14
      levelMod = 1;

      result = Random.die_roll(4)
      if result == 1
        self.modify_weapon_stats(data, "Shiny ", 2, 2, data["value"], false)

        data["attributes"] << ItemAttribute.new(:augment, "Shiny")
      elsif result == 2
        self.modify_weapon_stats(data, "Sturdy ", 3, 3, 5, false)

        data["attributes"] << ItemAttribute.new(:augment, "Sturdy")
      elsif result == 3
        self.modify_weapon_stats(data, "Sharp ", 3, 4, 5, false)

        data["attributes"] << ItemAttribute.new(:augment, "Sharp")
      elsif result == 4
        self.modify_weapon_stats(data, "Laser-Guided ", 4, 4, 5, false)

        data["attributes"] << ItemAttribute.new(:augment, "Laser")
      end
    elsif weaponMod < 18
      levelMod = 2;

      self.modify_weapon_stats(data, "Great ", 6, 6, 10, false)

      data["attributes"] << ItemAttribute.new(:augment, "Great")
    end

    return levelMod
  end

  def self.add_element(data, result)

    roll = Random.bell_curve((2), 2)

    weaponMod = roll.to_i
    if weaponMod < 1
      weaponMod = 1;
    end

    levelMod = 0;

    if weaponMod <= 1
      levelMod = 0
      data["element"] = "physical"

      data["attributes"] << ItemAttribute.new(:element, "Physical")
    elsif weaponMod == 2
      levelMod = 1

      self.modify_weapon_stats(data, " of Flames", 2, 2, 5, true)
      data["element"] = "flame"

      data["attributes"] << ItemAttribute.new(:element, "Flame")
    elsif weaponMod == 3
      levelMod = 1

      self.modify_weapon_stats(data, " of Freezing", 2, 2, 5, true)
      data["element"] = "freezing"

      data["attributes"] << ItemAttribute.new(:element, "Freezing")
    elsif weaponMod == 4
      levelMod = 2

      self.modify_weapon_stats(data, " of Zapping", 4, 4, 10, true)
      data["element"] = "zapping"

      data["attributes"] << ItemAttribute.new(:element, "Zapping")
    elsif weaponMod == 5
      levelMod = 2

      self.modify_weapon_stats(data, " of Radiactivity", 4, 4, 10, true)
      data["element"] = "radioactive"

      data["attributes"] << ItemAttribute.new(:element, "Radioactive")
    else
      levelMod = 3

      self.modify_weapon_stats(data, " of Exploding", 6, 6, 15, true)
      data["element"] = "exploding"

      data["attributes"] << ItemAttribute.new(:element, "Exploding")
    end

    return levelMod
  end

  def self.set_weapon_stats(data, itemLevel)
    data["attributes"] ||= []

    roll = Random.bell_curve((itemLevel), 0.5)

    result = roll.to_i
    if result < 1
      result = 1;
    elsif result > 10
      result = 10;
    end

    asp1 = result

    self.add_material(data, result)

    asp2 = 0

    if result <= itemLevel
      asp2 = self.add_attribute(data, result)
    end

    asp3 = 0

    if result + asp2 <= itemLevel
      asp3 = self.add_element(data, result)
    end

    level = asp1 + asp2 + asp3

    return level
	end
end
