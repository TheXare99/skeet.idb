#include "../include_cheat.h"

void __fastcall hooks::run_command(void* ecx_, void* edx_, C_BasePlayer* entity, CUserCmd* ucmd, IMoveHelper* move_helper) {
	if (!entity || entity != g_pLocalPlayer)
		return;

	float m_velocity_modifier = FLT_MAX;
	static int old_tickbase = 0;

	m_velocity_modifier = g_pLocalPlayer->get_velocity_modifier();

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	orig_run_command(ecx_, edx_, entity, ucmd, move_helper);

	if (g_pLocalPlayer && g_pLocalPlayer->get_alive() && entity == g_pLocalPlayer) {
		if (m_velocity_modifier != FLT_MAX)
			g_pLocalPlayer->get_velocity_modifier() = m_velocity_modifier;

		if (weapon) {
			static int old_activity = weapon->get_activity();
			const auto tickbase = entity->get_tickbase() - 1;
			auto activity = weapon->get_activity();
			if (weapon->get_item_definiton_index() == WEAPON_REVOLVER && !ucmd->hasbeenpredicted) {
				if (old_activity != activity && weapon->get_activity() == 208)
					old_tickbase = tickbase + 2;

				if (weapon->get_activity() == 208 && old_tickbase == tickbase)
					weapon->get_postpone_fire_ready_time() = time_to_ticks(tickbase) + 0.2f;
			}
			old_activity = activity;
		}
	}
	if (g_pMoveHelper != move_helper)
		g_pMoveHelper = move_helper;
}