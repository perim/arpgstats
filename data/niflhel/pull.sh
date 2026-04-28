#!/bin/bash

function pull {
	wget -O $1 "https://docs.google.com/spreadsheets/d/1ZRqxQG7gUIBRWpFN0wHsOepKXADBqRTk0ofpcBVnM-c/export?format=csv&gid=$2"
}

# Modifiers
pull item_mods.csv 1900576104
pull skill_mods.csv 1099504492
pull class_mods.csv 0
pull device_mods.csv 1526865506
pull item_implicits.csv 736088889

# Items
pull equipment.csv 1927042379
pull currencies.csv 874376262

# Monsters
pull monsters.csv 797459359
pull monster_roles.csv 1856790211

# Misc
pull types.csv 552895390
