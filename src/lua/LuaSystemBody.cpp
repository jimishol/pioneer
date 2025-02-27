// Copyright © 2008-2022 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Body.h"
#include "EnumStrings.h"
#include "Game.h"
#include "LuaObject.h"
#include "LuaUtils.h"
#include "Pi.h"
#include "SectorView.h"
#include "Space.h"
#include "galaxy/Galaxy.h"
#include "galaxy/StarSystem.h"

/*
 * Class: SystemBody
 *
 * Class representing a system body.
 *
 * <SystemBody> differs from <Body> in that it holds the properties that are
 * used to generate the physics <body> that is created when the player enters
 * a system. It exists outside of the current space. That is, scripts can use
 * a <SystemBody> to discover information about a body that exists in another
 * system.
 */

/*
 * Attribute: index
 *
 * The body index of the body in its system
 *
 * Availability:
 *
 *  alpha 10
 *
 * Status:
 *
 *  stable
 */
static int l_sbody_attr_index(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushinteger(l, sbody->GetPath().bodyIndex);
	return 1;
}

/*
 * Attribute: name
 *
 * The name of the body
 *
 * Availability:
 *
 *  alpha 10
 *
 * Status:
 *
 *  stable
 */
static int l_sbody_attr_name(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushstring(l, sbody->GetName().c_str());
	return 1;
}

/*
 * Attribute: type
 *
 * The type of the body, as a <Constants.BodyType> constant
 *
 * Availability:
 *
 *  alpha 10
 *
 * Status:
 *
 *  stable
 */
static int l_sbody_attr_type(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushstring(l, EnumStrings::GetString("BodyType", sbody->GetType()));
	return 1;
}

/*
 * Attribute: superType
 *
 * The supertype of the body, as a <Constants.BodySuperType> constant
 *
 * Availability:
 *
 *  alpha 10
 *
 * Status:
 *
 *  stable
 */
static int l_sbody_attr_super_type(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushstring(l, EnumStrings::GetString("BodySuperType", sbody->GetSuperType()));
	return 1;
}

/*
 * Attribute: seed
 *
 * The random seed used to generate this <SystemBody>. This is guaranteed to
 * be the same for this body across runs of the same build of the game, and
 * should be used to seed a <Rand> object when you want to ensure the same
 * random numbers come out each time.
 *
 * This value is the same is the one available via <Body.seed> once you enter
 * this system.
 *
 * Availability:
 *
 *   alpha 10
 *
 * Status:
 *
 *   stable
 */
static int l_sbody_attr_seed(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushinteger(l, sbody->GetSeed());
	return 1;
}

/*
 * Attribute: parent
 *
 * The parent of the body, as a <SystemBody>. A body orbits its parent.
 *
 * Availability:
 *
 *   alpha 14
 *
 * Status:
 *
 *   stable
 */
static int l_sbody_attr_parent(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);

	// sbody->parent is 0 as it was cleared by the acquirer. we need to go
	// back to the starsystem proper to get what we need.
	RefCountedPtr<StarSystem> s = Pi::game->GetGalaxy()->GetStarSystem(sbody->GetPath());
	SystemBody *live_sbody = s->GetBodyByPath(sbody->GetPath());

	if (!live_sbody->GetParent())
		return 0;

	LuaObject<SystemBody>::PushToLua(live_sbody->GetParent());
	return 1;
}

/*
 * Attribute: population
 *
 * The population of the body, in billions of people.
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_population(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetPopulation());
	return 1;
}

/*
 * Attribute: radius
 *
 * The radius of the body, in metres (m).
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_radius(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetRadius());
	return 1;
}

/*
 * Attribute: mass
 *
 * The mass of the body, in kilograms (kg).
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_mass(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetMass());
	return 1;
}

/*
 * Attribute: gravity
 *
 * The gravity on the surface of the body (m/s).
 *
 * Availability:
 *
 *   alpha 21
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_gravity(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->CalcSurfaceGravity());
	return 1;
}

/*
 * Attribute: periapsis
 *
 * The periapsis of the body's orbit, in metres (m).
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_periapsis(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetOrbMin() * AU);
	return 1;
}

/*
 * Attribute: apoapsis
 *
 * The apoapsis of the body's orbit, in metres (m).
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_apoapsis(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetOrbMax() * AU);
	return 1;
}

/*
 * Attribute: orbitPeriod
 *
 * The orbit of the body, around its parent, in days, as a float
 *
 * Availability:
 *
 *   201708
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_orbital_period(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetOrbit().Period() / float(60 * 60 * 24));
	return 1;
}

/*
 * Attribute: rotationPeriod
 *
 * The rotation period of the body, in days
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_rotation_period(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetRotationPeriodInDays());
	return 1;
}

/*
 * Attribute: semiMajorAxis
 *
 * The semi-major axis of the orbit, in metres (m).
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_semi_major_axis(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetSemiMajorAxis() * AU);
	return 1;
}

/*
 * Attribute: eccentricity
 *
 * The orbital eccentricity of the body
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_eccentricty(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetEccentricity());
	return 1;
}

/*
 * Attribute: axialTilt
 *
 * The axial tilt of the body, in radians
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_axial_tilt(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetAxialTilt());
	return 1;
}

/*
 * Attribute: averageTemp
 *
 * The average surface temperature of the body, in degrees kelvin
 *
 * Availability:
 *
 *   alpha 16
 *
 * Status:
 *
 *   experimental
 */
static int l_sbody_attr_average_temp(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushinteger(l, sbody->GetAverageTemp());
	return 1;
}

/*
* Attribute: metallicity
*
* Returns the measure of metallicity of the body
* (crust) 0.0 = light (Al, SiO2, etc), 1.0 = heavy (Fe, heavy metals)
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_metallicity(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetMetallicity());
	return 1;
}

/*
* Attribute: volatileGas
*
* Returns the measure of volatile gas present in the atmosphere of the body
* 0.0 = no atmosphere, 1.0 = earth atmosphere density, 4.0+ ~= venus
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_volatileGas(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetVolatileGas());
	return 1;
}

/*
* Attribute: atmosOxidizing
*
* Returns the compositional value of any atmospheric gasses in the bodys atmosphere (if any)
* 0.0 = reducing (H2, NH3, etc), 1.0 = oxidising (CO2, O2, etc)
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_atmosOxidizing(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetAtmosOxidizing());
	return 1;
}

/*
* Attribute: volatileLiquid
*
* Returns the measure of volatile liquids present on the body
* 0.0 = none, 1.0 = waterworld (earth = 70%)
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_volatileLiquid(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetVolatileLiquid());
	return 1;
}

/*
* Attribute: volatileIces
*
* Returns the measure of volatile ices present on the body
* 0.0 = none, 1.0 = total ice cover (earth = 3%)
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_volatileIces(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetVolatileIces());
	return 1;
}

/*
* Attribute: volcanicity
*
* Returns the measure of volcanicity of the body
* 0.0 = none, 1.0 = lava planet
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_volcanicity(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetVolcanicity());
	return 1;
}

/*
* Attribute: life
*
* Returns the measure of life present on the body
* 0.0 = dead, 1.0 = teeming (~= pandora)
*
* Availability:
*
*   January 2018
*
* Status:
*
*   experimental
*/
static int l_sbody_attr_life(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushnumber(l, sbody->GetLife());
	return 1;
}

/*
* Attribute: hasRings
*
* Returns true if the body has a ring or rings of debris or ice in orbit around it
*
* Availability:
*
*   January 2018
*
* Status:
*
*  experimental
*/

static int l_sbody_attr_has_rings(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushboolean(l, sbody->HasRings());
	return 1;
}

/*
 * Attribute: hasAtmosphere
 *
 * Returns true if an atmosphere is present, false if not
 *
 * Availability:
 *
 *   alpha 21
 *
 * Status:
 *
 *  experimental
 */

static int l_sbody_attr_has_atmosphere(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushboolean(l, sbody->HasAtmosphere());
	return 1;
}

/*
 * Attribute: isScoopable
 *
 * Returns true if the system body can be scoopable, false if not
 *
 * Availablility:
 *
 *   alpha 21
 *
 * Status:
 *
 *  experimental
 */

static int l_sbody_attr_is_scoopable(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	lua_pushboolean(l, sbody->IsScoopable());
	return 1;
}

static int l_sbody_attr_path(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	LuaObject<SystemPath>::PushToLua(sbody->GetPath());
	return 1;
}

static int l_sbody_attr_astro_description(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	LuaPush(l, sbody->GetAstroDescription());
	return 1;
}

static int l_sbody_attr_body(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	if (Pi::game) {
		Space *space = Pi::game->GetSpace();
		if (space) {
			const SystemPath &path = sbody->GetPath();
			Body *body = space->FindBodyForPath(&path);
			if (body) {
				LuaObject<Body>::PushToLua(body);
			} else {
				lua_pushnil(l);
			}
		}
	} else {
		lua_pushnil(l);
	}
	return 1;
}

static int l_sbody_attr_children(lua_State *l)
{
	SystemBody *sbody = LuaObject<SystemBody>::CheckFromLua(1);
	LuaTable children(l);
	int i = 1;
	for (auto child : sbody->GetChildren()) {
		LuaPush(l, i++);
		LuaObject<SystemBody>::PushToLua(child);
		lua_settable(l, -3);
	}
	return 1;
}

static int l_sbody_attr_nearest_jumpable(lua_State *l)
{
	LuaObject<SystemBody>::PushToLua(LuaObject<SystemBody>::CheckFromLua(1)->GetNearestJumpable());
	return 1;
}

static int l_sbody_attr_is_moon(lua_State *l)
{
	LuaPush<bool>(l, LuaObject<SystemBody>::CheckFromLua(1)->IsMoon());
	return 1;
}

static int l_sbody_attr_is_station(lua_State *l)
{
	LuaPush<bool>(l, LuaObject<SystemBody>::CheckFromLua(1)->GetSuperType() == SystemBody::SUPERTYPE_STARPORT);
	return 1;
}

static int l_sbody_attr_is_ground_station(lua_State *l)
{
	SystemBody *sb = LuaObject<SystemBody>::CheckFromLua(1);
	LuaPush<bool>(l, sb->GetSuperType() == SystemBody::SUPERTYPE_STARPORT && sb->GetType() == SystemBody::TYPE_STARPORT_SURFACE);
	return 1;
}

static int l_sbody_attr_is_space_station(lua_State *l)
{
	SystemBody *sb = LuaObject<SystemBody>::CheckFromLua(1);
	LuaPush<bool>(l, sb->GetSuperType() == SystemBody::SUPERTYPE_STARPORT && sb->GetType() == SystemBody::TYPE_STARPORT_ORBITAL);
	return 1;
}

static int l_sbody_attr_physics_body(lua_State *l)
{
	SystemBody *b = LuaObject<SystemBody>::CheckFromLua(1);
	Body *physbody = nullptr;
	SystemPath headpath = Pi::game->GetSectorView()->GetSelected().SystemOnly();
	SystemPath gamepath = Pi::game->GetSpace()->GetStarSystem()->GetPath();
	if (headpath == gamepath) {
		RefCountedPtr<StarSystem> ss = Pi::game->GetGalaxy()->GetStarSystem(headpath);
		SystemPath path = ss->GetPathOf(b);
		physbody = Pi::game->GetSpace()->FindBodyForPath(&path);
	}
	LuaObject<Body>::PushToLua(physbody);
	return 1;
}

template <>
const char *LuaObject<SystemBody>::s_type = "SystemBody";

template <>
void LuaObject<SystemBody>::RegisterClass()
{
	static const luaL_Reg l_attrs[] = {
		{ "index", l_sbody_attr_index },
		{ "name", l_sbody_attr_name },
		{ "type", l_sbody_attr_type },
		{ "superType", l_sbody_attr_super_type },
		{ "seed", l_sbody_attr_seed },
		{ "parent", l_sbody_attr_parent },
		{ "population", l_sbody_attr_population },
		{ "radius", l_sbody_attr_radius },
		{ "mass", l_sbody_attr_mass },
		{ "gravity", l_sbody_attr_gravity },
		{ "periapsis", l_sbody_attr_periapsis },
		{ "apoapsis", l_sbody_attr_apoapsis },
		{ "orbitPeriod", l_sbody_attr_orbital_period },
		{ "rotationPeriod", l_sbody_attr_rotation_period },
		{ "semiMajorAxis", l_sbody_attr_semi_major_axis },
		{ "eccentricity", l_sbody_attr_eccentricty },
		{ "axialTilt", l_sbody_attr_axial_tilt },
		{ "averageTemp", l_sbody_attr_average_temp },
		{ "metallicity", l_sbody_attr_metallicity },
		{ "volatileGas", l_sbody_attr_volatileGas },
		{ "atmosOxidizing", l_sbody_attr_atmosOxidizing },
		{ "volatileLiquid", l_sbody_attr_volatileLiquid },
		{ "volatileIces", l_sbody_attr_volatileIces },
		{ "volcanicity", l_sbody_attr_volcanicity },
		{ "life", l_sbody_attr_life },
		{ "hasRings", l_sbody_attr_has_rings },
		{ "hasAtmosphere", l_sbody_attr_has_atmosphere },
		{ "isScoopable", l_sbody_attr_is_scoopable },
		{ "astroDescription", l_sbody_attr_astro_description },
		{ "path", l_sbody_attr_path },
		{ "body", l_sbody_attr_body },
		{ "children", l_sbody_attr_children },
		{ "nearestJumpable", l_sbody_attr_nearest_jumpable },
		{ "isMoon", l_sbody_attr_is_moon },
		{ "isStation", l_sbody_attr_is_station },
		{ "isGroundStation", l_sbody_attr_is_ground_station },
		{ "isSpaceStation", l_sbody_attr_is_space_station },
		{ "physicsBody", l_sbody_attr_physics_body },
		{ 0, 0 }
	};

	LuaObjectBase::CreateClass(s_type, 0, 0, l_attrs, 0);
}
