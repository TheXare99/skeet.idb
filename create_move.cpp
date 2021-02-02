#include "../include_cheat.h"
#include <intrin.h>

bool __stdcall hooks::create_move(float input_sample_time, CUserCmd* cmd)
{
	if (!cmd || !cmd->command_number)
		return orig_create_move(input_sample_time, cmd);

	g_cmd = cmd;
	g_max_lag = (*g_ppGameRules)->m_bIsValveDS() ? 6 : 14;

	misc::get().clan_tag();
	misc::get().chat_spam();
	misc::get().reveal_ranks();
	misc::get().update_keybinds();

	if (g_unload)
		return orig_create_move(input_sample_time, g_cmd);

	prediction::get().unpred_viewangles = g_cmd->viewangles;
	prediction::get().strafe_angles = g_cmd->viewangles;

	misc::get().radar();
	misc::get().bhop();
	autostrafer::strafe();
	antiaim::get().slow_walk();

	prediction::get().start();
	{
		antiaim::get().fake_lags();
		antiaim::get().run();
		ragebot::get().run();
		ragebot::get().no_recoil();
	}
	prediction::get().finish();

	if (cmd->buttons & IN_ATTACK && !ragebot::get().get_ragebot_state() && ragebot::get().can_shoot(g_pGlobals->curtime, true))
		resolver::get().add_shot(g_pLocalPlayer->get_eye_pos(), nullptr, -1);

	// клемпим нахуй сендпакет.
	if (g_pClientState->m_nChokedCommands >= g_max_lag)
		g_send_packet = true;

	// сторим углы дл€ онемфикса.
	if (g_send_packet)
		thirdperson::get().set_networked_angle(cmd->viewangles);
	else
		thirdperson::get().set_clientside_angle(cmd->viewangles);

	cmd->viewangles.y = math::get().normalize_float(cmd->viewangles.y);
	cmd->viewangles.Clamp();

	// ломаем мувмент фикс если билд кс не трушный.
	if (!g_csgobuild_invalid)
		antiaim::get().fix_movement();

	// клемпим всю хуйню по значени€м сервака.
	__(cl_sidespeed_s, "cl_sidespeed");
	__(cl_forwardspeed_s, "cl_forwardspeed");
	__(cl_upspeed_s, "cl_upspeed");
	static ConVar* cl_sidespeed = g_pCVar->FindVar(cl_sidespeed_s);
	static ConVar* cl_forwardspeed = g_pCVar->FindVar(cl_forwardspeed_s);
	static ConVar* cl_upspeed = g_pCVar->FindVar(cl_upspeed_s);

	cmd->sidemove = clamp(cmd->sidemove, -cl_sidespeed->get_float(), cl_sidespeed->get_float());
	cmd->forwardmove = clamp(cmd->forwardmove, -cl_forwardspeed->get_float(), cl_forwardspeed->get_float());
	cmd->upmove = clamp(cmd->upmove, -cl_upspeed->get_float(), cl_upspeed->get_float());

	uintptr_t *frame_ptr;
	__asm mov frame_ptr, ebp;
	*(bool *)(*frame_ptr - 0x1c) = g_send_packet;

	// оффаем к ху€м все стопы, дабы в некст тике не стопатс€ как долбоЄб.
	g_stop = false;
	g_slowwalk = false;

	// чтобы анимки апдейтились каждый тик.
	g_should_animate = true;

	return false;
}