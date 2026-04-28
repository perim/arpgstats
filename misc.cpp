#include "misc.h"

damage_t to_damage_type(const std::string& str)
{
	if (str == "Physical") return damage_t::Physical;
	else if (str == "Fire") return damage_t::Fire;
	else if (str == "Cold") return damage_t::Cold;
	else if (str == "Lightning") return damage_t::Lightning;
	else if (str == "Poison") return damage_t::Poison;
	else { abort(); return damage_t::Poison; }
}
