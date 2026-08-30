#include "creature.h"
#include "misc.h"

#include "external/csv.hpp"

struct creature_type_t
{
	std::string name;
	std::string species;
	int weighting = -1;
	int min = -1;
	int max = -1;
	int move = 100;
	int physical = 100;
	damage_t element = damage_t::Physical;
	int elemental = 0;
	int dot_duration = 100;
	int attack_speed = 100;
	int range = 10;
	int ammo = -1;
	int health = 100;
	int energy = 100;
	int stamina = 100;
	int block_speed = 100;
	int reload = 100;
	int max_level = -1;
	int min_level = -1;
	std::string tags; // TBD list
	std::vector<creature_role_t> roles; // indexed by role, default = undefined
	uint8_t role_mask = 0; // bit set if the role is defined
};

static std::vector<creature_type_t> creatures;
static std::vector<int> creature_weights;

static constexpr int role_count = (int)role_type::boss + 1;

static role_type to_role_type(const std::string& str)
{
	if (str == "Leader") return role_type::leader;
	else if (str == "Damage") return role_type::damage;
	else if (str == "Tank") return role_type::tank;
	else if (str == "Support") return role_type::support;
	else if (str == "Boss") return role_type::boss;
	assert(false);
	return role_type::invalid;
}

const char* role_to_string(role_type t)
{
	if (t == role_type::leader) return "Leader";
	else if (t == role_type::damage) return "Damage";
	else if (t == role_type::tank) return "Tank";
	else if (t == role_type::support) return "Support";
	else if (t == role_type::boss) return "Boss";
	assert(false);
	return "Internal error";
}

static int find_creature_type(const std::string& name)
{
	for (unsigned i = 0; i < creatures.size(); i++)
	{
		if (creatures.at(i).name == name) return (int)i;
	}
	return -1;
}

const std::string& get_creature_name(int index)
{
	return creatures.at(index).name;
}

struct creature_cache
{
	filtered_const_roll_table* creatures = nullptr;

	~creature_cache()
	{
		delete creatures;
	}
};

bool init_creature_cache(creature_context_t& ctx)
{
	assert(!creatures.empty());
	free_creature_cache(ctx);
	ctx.cache = new creature_cache;
	std::vector<bool> mask(creatures.size());
	bool has_any = false;
	for (unsigned i = 0; i < creatures.size(); i++)
	{
		const creature_type_t& c = creatures.at(i);
		mask[i] = ctx.depth >= c.min_level && (c.max_level < 0 || ctx.depth <= c.max_level);
		has_any |= mask[i];
	}
	assert(has_any);
	if (has_any) ctx.cache->creatures = new filtered_const_roll_table(creature_weights, mask);
	return has_any;
}

void free_creature_cache(creature_context_t& ctx)
{
	if (ctx.cache)
	{
		delete ctx.cache;
		ctx.cache = nullptr;
	}
}

bool read_creatures(const char* path)
{
	creatures.clear();
	creature_weights.clear();
	csv::CSVReader reader(path);
	for (auto& row : reader)
	{
		creature_type_t v;
		v.name = row["Type"].get<>();
		if (v.name.empty()) continue; // skip sum rows
		v.species = row["Species"].get<>();
		if (row["Weighting"].is_int()) v.weighting = row["Weighting"].get<int>();
		if (row["Min"].is_int()) v.min = row["Min"].get<int>(); // number of creatures spawning together
		if (row["Max"].is_int()) v.max = row["Max"].get<int>();
		if (row["Move"].is_int()) v.move = row["Move"].get<int>();
		if (row["Phys"].is_int()) v.physical = row["Phys"].get<int>();
		std::string elem = row["Element"].get<>();
		if (!elem.empty()) v.element = to_damage_type(elem);
		if (row["Res+Dam"].is_int()) v.elemental = row["Res+Dam"].get<int>(); // damage + resist
		if (row["DoT secs"].is_int()) v.dot_duration = row["DoT secs"].get<int>();
		if (row["Attack speed"].is_int()) v.attack_speed = row["Attack speed"].get<int>();
		if (row["Range"].is_int()) v.range = row["Range"].get<int>();
		if (row["Ammo"].is_int()) v.ammo = row["Ammo"].get<int>();
		if (row["Health"].is_int()) v.health = row["Health"].get<int>();
		if (row["Energy"].is_int()) v.energy = row["Energy"].get<int>();
		if (row["Stamina"].is_int()) v.stamina = row["Stamina"].get<int>();
		if (row["Block speed"].is_int()) v.block_speed = row["Block speed"].get<int>();
		if (row["Reload"].is_int()) v.reload = row["Reload"].get<int>();
		if (row["Min level"].is_int()) v.min_level = row["Min level"].get<int>();
		if (row["Max level"].is_int()) v.max_level = row["Max level"].get<int>();
		v.tags = row["Tags"].get<>(); // TBD decompose into a list
		v.roles.assign(role_count, creature_role_t{});
		creatures.push_back(v);
		creature_weights.push_back(v.weighting > 0 ? v.weighting : 100);
	}
	printf("Added %d creatures\n", (int)creatures.size());
	return creatures.size() > 0;
}

bool read_creature_roles(const char* path)
{
	assert(!creatures.empty()); // read_creatures() first
	for (auto& c : creatures)
	{
		c.roles.assign(role_count, creature_role_t{});
		c.role_mask = 0;
	}
	csv::CSVReader reader(path);
	int added = 0;
	for (auto& row : reader)
	{
		const std::string type_name = row["Type"].get<>();
		if (type_name.empty()) continue; // skip sum rows
		const role_t role = to_role_type(row["Subtype"].get<>());
		if (role == role_type::invalid) continue; // skip unknown roles
		const int type = find_creature_type(type_name);
		if (type < 0)
		{
			printf("Warning: unknown creature type '%s' in creature roles\n", type_name.c_str());
			continue;
		}
		creature_role_t r;
		const std::string filled_by = row["Filled by"].get<>();
		if (!filled_by.empty())
		{
			const int filler = find_creature_type(filled_by);
			if (filler < 0)
			{
				// The filling creature is not in monsters.csv yet, fall back to the tribe's own type
				printf("Warning: creature '%s' filling '%s' is not defined, using '%s'\n", filled_by.c_str(), type_name.c_str(), type_name.c_str());
			}
			r.creature_type = (uint16_t)(filler >= 0 ? filler : type);
		}
		else
		{
			r.creature_type = (uint16_t)type;
		}
		if (row["Powers %"].is_int()) r.scaling = perten_from_percent(row["Powers %"].get<int>());
		creatures.at(type).roles.at((int)role) = r;
		creatures.at(type).role_mask |= (uint8_t)(1u << (int)role);
		added++;
	}
	printf("Added %d creature roles\n", added);
	return added > 0;
}

const creature_role_t* get_creature_role(int index, role_t role)
{
	const int r = (int)role;
	if (index < 0 || index >= (int)creatures.size() || r < 0 || r >= role_count) return nullptr;
	if (!(creatures.at(index).role_mask & (1u << r))) return nullptr;
	return &creatures.at(index).roles.at(r);
}

int get_creature_count()
{
	return (int)creatures.size();
}

int get_creature_index(const std::string& name)
{
	return find_creature_type(name);
}

/// The role of the member at 'index' in a tribe of 'count' creatures of the given type.
/// Damage is the default filler role and is always present. Leader, tank and support are
/// included (at most one each, in that priority order) as long as at least one damage creature
/// remains. A leader is left out of a one-creature tribe, because it needs followers.
/// Bosses are handled separately and never appear here.
static role_t tribe_role_at(const creature_type_t& c, int count, int index)
{
	if (index < 0 || index >= count) return role_type::damage;
	int place = 0;
	for (role_t spec : { role_type::leader, role_type::tank, role_type::support })
	{
		if ((c.role_mask & (1u << (int)spec)) && place + 1 < count)
		{
			if (index == place) return spec;
			place++;
		}
	}
	return role_type::damage;
}

tribe_t create_tribe(const creature_context_t& context, position pos)
{
	assert(context.cache && context.cache->creatures);
	// Derive the tribe's seed from the context seed and the spawn position, so that tribes are
	// deterministic and independent of each other.
	tribe_t v{ -1, 0, context.rand.derive(pos.x, pos.y, pos.z, pos.w), pos, 0 };
	const int which = context.cache->creatures->roll(v.s);
	v.creature_type = (uint16_t)which;
	const creature_type_t& c = creatures.at(which);
	const int min = c.min > 0 ? c.min : 1;
	const int max = c.max >= min ? c.max : min;
	v.count = (uint16_t)v.s.roll(min, max);
	assert(v.count <= 32); // tribe_short_memory_t only tracks 32 members
	v.leader = (tribe_role_at(c, v.count, 0) == role_type::leader) ? 0 : -1;
	return v;
}

std::vector<creature_t> create_creature(const creature_context_t& context, const tribe_t& tribe)
{
	(void)context; // not needed yet, kept for future depth/ability scaling
	assert(tribe.count > 0 && tribe.count <= 32);
	assert(tribe.creature_type < creatures.size());
	const creature_type_t& c = creatures.at(tribe.creature_type);
	std::vector<creature_t> v(tribe.count);
	for (int i = 0; i < (int)tribe.count; i++)
	{
		const role_t role = tribe_role_at(c, tribe.count, i);
		const creature_role_t* def = get_creature_role(tribe.creature_type, role);
		creature_t& m = v.at(i);
		m.role = role;
		if (def)
		{
			m.creature_type = def->creature_type;
			m.scaling = def->scaling;
		}
		else // default: the tribe's own type at full power
		{
			m.creature_type = tribe.creature_type;
			m.scaling = perten_full;
		}
	}
	return v;
}
