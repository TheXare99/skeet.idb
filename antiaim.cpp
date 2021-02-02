#include "../include_cheat.h"
void antiaim::fake_lags()
{
	if (!cfg.anti_aimbot.fake_lag.enabled)
	{
		g_send_packet = g_cmd->command_number % 2;
		return;
	}

	auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	int choke = 1;

	if (!(g_pLocalPlayer->get_flags() & FL_ONGROUND))
	{
		choke = static_cast<int>(cfg.anti_aimbot.fake_lag.limit_when_in_air);
	}
	else
	{
		if (!vars::aa.slowwalk.get<bool>() || !GetAsyncKeyState(vars::key.slowwalk.get<int>()))
		{
			if (animstate->m_flSpeed > 5.f)
			{
				choke = static_cast<int>(cfg.anti_aimbot.fake_lag.limit_when_move);
			}
			else
			{
				choke = static_cast<int>(cfg.anti_aimbot.fake_lag.limit_when_stand);
			}
		}
		else
		{
			choke = static_cast<int>(cfg.anti_aimbot.fake_lag.limit_when_slowmotion);
		}
	}

	g_send_packet = g_pClientState->m_nChokedCommands >= choke;
}
void antiaim::slow_walk()
{
	if (!vars::aa.slowwalk.get<bool>() || !GetAsyncKeyState(vars::key.slowwalk.get<int>()))
		return;

	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	// get the max possible speed whilest we are still accurate.
	float flMaxSpeed = g_pLocalPlayer->get_scoped() ? weapon->get_wpn_data()->flMaxSpeedAlt : weapon->get_wpn_data()->flMaxSpeed;
	float flDesiredSpeed = (flMaxSpeed * 0.33000001);

	if (animstate->m_flSpeed > flDesiredSpeed)
	{
		// convert velocity to angular momentum.
		QAngle angle;
		math::get().vector_angles(g_pLocalPlayer->get_velocity(), angle);

		// get our current speed of travel.
		float speed = g_pLocalPlayer->get_velocity().Length();

		// fix direction by factoring in where we are looking.
		angle.y = prediction::get().unpred_viewangles.y - angle.y;

		// convert corrected angle back to a direction.
		Vector direction;
		math::get().angle_vectors(angle, &direction);

		Vector stop = direction * -speed;

		g_cmd->forwardmove = stop.x;
		g_cmd->sidemove = stop.y;
	}
	else
	{
		// thanks onetap.
		g_cmd->buttons |= IN_SPEED;

		float squirt = sqrtf((g_cmd->forwardmove * g_cmd->forwardmove) + (g_cmd->sidemove * g_cmd->sidemove));

		if (squirt > flDesiredSpeed) {
			float squirt2 = sqrtf((g_cmd->forwardmove * g_cmd->forwardmove) + (g_cmd->sidemove * g_cmd->sidemove));

			float cock1 = g_cmd->forwardmove / squirt2;
			float cock2 = g_cmd->sidemove / squirt2;

			if (flDesiredSpeed + 1.0 <= animstate->m_flSpeed) {
				g_cmd->forwardmove = 0;
				g_cmd->sidemove = 0;
			}
			else {
				g_cmd->forwardmove = cock1 * flDesiredSpeed;
				g_cmd->sidemove = cock2 * flDesiredSpeed;
			}
		}
	}
}

void antiaim::run()
{
	if (!cfg.anti_aimbot.anti_aimbot_angles.enabled)
		return;

	if (!g_pLocalPlayer->get_alive() || (*g_ppGameRules)->m_bFreezePeriod())
		return;

	auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;
	
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	if (!weapon || g_cmd->buttons & IN_USE ||
		g_pLocalPlayer->get_move_type() == MOVETYPE_OBSERVER ||
		g_pLocalPlayer->get_move_type() == MOVETYPE_NOCLIP ||
		g_pLocalPlayer->get_move_type() == MOVETYPE_LADDER ||
		g_pLocalPlayer->get_immunity())
	{
		return;
	}
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;
	if (weapon->get_weapon_type() == WEAPONTYPE_C4)
	{
	}
	else if (weapon->is_grenade() && weapon->is_being_thrown())
	{
		g_send_packet = true;
		return;
	}
	else if (is_knife)
	{
		if (g_cmd->buttons & IN_ATTACK || g_cmd->buttons & IN_ATTACK2)
			return;
	}
	else if (weapon->get_item_definiton_index() == WEAPON_REVOLVER)
	{
		if (g_cmd->buttons & IN_ATTACK || g_cmd->buttons & IN_ATTACK2)
			return;
	}
	else if (g_cmd->buttons & IN_ATTACK && weapon->get_next_primary_attack() < g_pGlobals->curtime && g_pLocalPlayer->get_next_attack() < g_pGlobals->curtime)
		return;

	choose_real();
}

void antiaim::lby_prediction()
{
	const auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	if (animstate->m_flSpeed > 0.1f)
	{
		next_lby_update = g_pGlobals->curtime + 0.22f;
	}
	else if (g_pGlobals->curtime > next_lby_update)
	{
		update_lby = true;
		next_lby_update = g_pGlobals->curtime + 1.1f;
	}
}

// thanks aimware.
void antiaim::fix_movement()
{
	Vector view_fwd, view_right, view_up, g_cmd_fwd, g_cmd_right, g_cmd_up;
	math::get().angle_vectors(prediction::get().strafe_angles, view_fwd, view_right, view_up);
	math::get().angle_vectors(g_cmd->viewangles, g_cmd_fwd, g_cmd_right, g_cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((g_cmd_fwd.x * g_cmd_fwd.x) + (g_cmd_fwd.y * g_cmd_fwd.y));
	const auto v16 = sqrtf((g_cmd_right.x * g_cmd_right.x) + (g_cmd_right.y * g_cmd_right.y));
	const auto v18 = sqrtf(g_cmd_up.z * g_cmd_up.z);

	const Vector norm_g_cmd_fwd((1.f / v14) * g_cmd_fwd.x, (1.f / v14) * g_cmd_fwd.y, 0.f);
	const Vector norm_g_cmd_right((1.f / v16) * g_cmd_right.x, (1.f / v16) * g_cmd_right.y, 0.f);
	const Vector norm_g_cmd_up(0.f, 0.f, (1.f / v18) * g_cmd_up.z);

	const auto v22 = norm_view_fwd.x * g_cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * g_cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * g_cmd->forwardmove;
	const auto v24 = norm_view_right.x * g_cmd->sidemove;
	const auto v23 = norm_view_right.y * g_cmd->sidemove;
	const auto v25 = norm_view_right.z * g_cmd->sidemove;
	const auto v30 = norm_view_up.x * g_cmd->upmove;
	const auto v27 = norm_view_up.z * g_cmd->upmove;
	const auto v29 = norm_view_up.y * g_cmd->upmove;

	g_cmd->forwardmove = ((((norm_g_cmd_fwd.x * v24) + (norm_g_cmd_fwd.y * v23)) + (norm_g_cmd_fwd.z * v25))
		+ (((norm_g_cmd_fwd.x * v22) + (norm_g_cmd_fwd.y * v26)) + (norm_g_cmd_fwd.z * v28)))
		+ (((norm_g_cmd_fwd.y * v30) + (norm_g_cmd_fwd.x * v29)) + (norm_g_cmd_fwd.z * v27));
	g_cmd->sidemove = ((((norm_g_cmd_right.x * v24) + (norm_g_cmd_right.y * v23)) + (norm_g_cmd_right.z * v25))
		+ (((norm_g_cmd_right.x * v22) + (norm_g_cmd_right.y * v26)) + (norm_g_cmd_right.z * v28)))
		+ (((norm_g_cmd_right.x * v29) + (norm_g_cmd_right.y * v30)) + (norm_g_cmd_right.z * v27));
	g_cmd->upmove = ((((norm_g_cmd_up.x * v23) + (norm_g_cmd_up.y * v24)) + (norm_g_cmd_up.z * v25))
		+ (((norm_g_cmd_up.x * v26) + (norm_g_cmd_up.y * v22)) + (norm_g_cmd_up.z * v28)))
		+ (((norm_g_cmd_up.x * v30) + (norm_g_cmd_up.y * v29)) + (norm_g_cmd_up.z * v27));

	prediction::get().strafe_angles = g_cmd->viewangles;

	if (g_pLocalPlayer->get_move_type() != MOVETYPE_LADDER)
		g_cmd->buttons &= ~(IN_FORWARD | IN_BACK | IN_MOVERIGHT | IN_MOVELEFT);
}

void antiaim::choose_real()
{
	auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	if (cfg.anti_aimbot.anti_aimbot_angles.yaw_base)
		do_at_target();

	g_cmd->viewangles.x = cfg.anti_aimbot.anti_aimbot_angles.pitch;
	g_cmd->viewangles.y += cfg.anti_aimbot.anti_aimbot_angles.yaw;

	lby_prediction();

	if (cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations)
	{
		g_cmd->viewangles.y += g_inverted_desync ? cfg.anti_aimbot.anti_aimbot_angles.inverted_body_lean : cfg.anti_aimbot.anti_aimbot_angles.body_lean;
		if (cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations)
		{
			if (!(g_cmd->buttons & IN_FORWARD || g_cmd->buttons & IN_BACK || g_cmd->buttons & IN_MOVELEFT || g_cmd->buttons & IN_MOVERIGHT) && g_pLocalPlayer->get_flags() & FL_ONGROUND)
			{
				g_cmd->forwardmove = g_cmd->command_number % 2 ? g_pLocalPlayer->get_flags() & IN_DUCK ? 2.94117647f : .505f : g_pLocalPlayer->get_flags() & IN_DUCK ? -2.94117647f : -.505f;
			}
			if (!g_send_packet)
				g_cmd->viewangles.y += g_inverted_desync ? -cfg.anti_aimbot.anti_aimbot_angles.maximum_delta : cfg.anti_aimbot.anti_aimbot_angles.maximum_delta;
		}
		else if (cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations)
		{
			if (!g_send_packet)
				if (update_lby) {
					g_cmd->viewangles.y += 180.f;
				}
				else
					g_cmd->viewangles.y += g_inverted_desync ? -cfg.anti_aimbot.anti_aimbot_angles.maximum_delta : cfg.anti_aimbot.anti_aimbot_angles.maximum_delta;
		}
	}
}

void antiaim::do_at_target() const
{
	C_CSPlayer* target = nullptr;
	QAngle target_angle;

	auto lowest_fov = 360.f;
	for (auto i = 1; i < g_pGlobals->maxClients; i++)
	{
		auto player = get_entity(i);
		if (!player || !player->get_alive() || !player->is_enemy() || player == g_pLocalPlayer)
			continue;

		if (player->IsDormant() && (player->get_simtime() > g_pGlobals->curtime || player->get_simtime() + 5.f < g_pGlobals->curtime))
			continue;

		const auto angle = math::get().calc_angle(g_pLocalPlayer->get_eye_pos(), player->get_origin());
		const auto fov = math::get().get_fov(g_original_viewangles, angle);

		if (fov < lowest_fov)
		{
			target = player;
			lowest_fov = fov;
			target_angle = angle;
		}
	}

	if (!target)
		return;

	g_cmd->viewangles.y = target_angle.y;
}