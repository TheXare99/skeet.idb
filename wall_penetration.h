#pragma once

class penetration : public singleton<penetration>
{
public:
	bool is_breakable_entity(IClientEntity* entity);
	void scale_damage(C_CSPlayer* ent, CGameTrace& enterTrace, CCSWeaponData* weaponData, float& currentDamage, bool local = false);
	bool trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction, bool local = false);
	bool handle_bullet_penetration(CCSWeaponData* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, bool local = false, float dist = 0.f, Vector start = { 0.f,0.f,0.f });
	bool can_hit(C_CSPlayer* player, Vector& point, float& damage, const Vector pos = g_pLocalPlayer->get_eye_pos(), bool full = false);
	bool fire_bullet(C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, C_CSPlayer* enemy = nullptr, float length = 0.f, Vector pos = { 0.f,0.f,0.f });
};
