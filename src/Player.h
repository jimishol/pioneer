// Copyright © 2008-2022 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Ship.h"
#include "ShipCockpit.h"

class PlayerShipController;

namespace Graphics {
	class Renderer;
}

class Player : public Ship {
public:
	OBJDEF(Player, Ship, PLAYER);
	Player() = delete;
	Player(const Json &jsonObj, Space *space);
	Player(const ShipType::Id &shipId);

	virtual void SetDockedWith(SpaceStation *, int port) override;
	virtual bool DoDamage(float kgDamage) override final; // overloaded to add "crush" audio
	virtual bool OnDamage(Body *attacker, float kgDamage, const CollisionContact &contactData) override;
	virtual bool SetWheelState(bool down) override; // returns success of state change, NOT state itself
	virtual Missile *SpawnMissile(ShipType::Id missile_type, int power = -1) override;
	virtual void SetAlertState(Ship::AlertState as) override;
	virtual void NotifyRemoved(const Body *const removedBody) override;

	virtual void SetShipType(const ShipType::Id &shipId) override;

	PlayerShipController *GetPlayerController() const;
	//XXX temporary things to avoid causing too many changes right now
	Body *GetCombatTarget() const;
	Body *GetNavTarget() const;
	Body *GetSetSpeedTarget() const;
	void SetCombatTarget(Body *const target, bool setSpeedTo = false);
	void SetNavTarget(Body *const target);
	void SetSetSpeedTarget(Body *const target);
	void ChangeSetSpeed(double delta);

	virtual Ship::HyperjumpStatus InitiateHyperjumpTo(const SystemPath &dest, int warmup_time, double duration, const HyperdriveSoundsTable &sounds, LuaRef checks) override;
	virtual void AbortHyperjump() override;

	// XXX cockpit is here for now because it has a physics component
	void InitCockpit();
	ShipCockpit *GetCockpit() const { return m_cockpit.get(); }
	void OnCockpitActivated();

	virtual void StaticUpdate(const float timeStep) override;
	sigc::signal<void> onChangeEquipment;
	virtual vector3d GetManeuverVelocity() const;
	virtual int GetManeuverTime() const;

protected:
	virtual void SaveToJson(Json &jsonObj, Space *space) override;

	virtual void OnEnterSystem() override;
	virtual void OnEnterHyperspace() override;

private:
	std::unique_ptr<ShipCockpit> m_cockpit;
	Sound::Event m_creakSound;
	vector3d m_accel;
	vector3d m_jerk[5] = { vector3d(0.0, 0.0, 0.0) };
};

#endif /* _PLAYER_H */
