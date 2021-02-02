#include "../include_cheat.h"

void resolver::resolve(C_CSPlayer* player, lag_record_t* record) {
	auto& log = player_log::get().get_log(player->EntIndex());

	// рекорд отсутствует, оффнут ресольвер либо чел - бот? нахуй все логи сбрасываем и не ресольвим его.
	if (log.record.empty() || (!cfg.ragebot.other.anti_aim_correction) || player->get_player_info().fakeplayer)	{
		log.m_nMissedShotsResolver = 0;
		log.m_nMissedShotsSpread = 0;
		log.m_nLastSide = 0;
		log.m_nCurrentSide = 0;
		return;
	}

	// никто антиаимы на лестнице и при ноуклипе не юзает - так нахуй их ресольвить?
	if (player->get_move_type() == MOVETYPE_LADDER || player->get_move_type() == MOVETYPE_NOCLIP)
		return;

	// челов в дорманте не ресольвим.
	if (player->IsDormant())
		return;

	// если чел не юзает флаги - десинк работать не будет, так что ресольвить там нехуй.
	if (record->m_lag <= 1)
		return;

	// бекшуты ресольвить - не пастерское дело.
	if (record->m_shot)
		return;

	// создаем переменные, которые потом будем использовать для трушного брутфорса и прочей хуйни.
	float ResolvedYaw = record->m_anim_state->m_flEyeYaw;

	// оверрайд фром 2к18 по мышке, зато охуенный.
	if (g_resolver_override) {
		auto at_target_yaw = math::get().calc_angle(record->m_origin, g_pLocalPlayer->get_origin()).y;
		auto delta = math::get().normalize_float(g_original_viewangles.y - at_target_yaw);
		if (delta < 0)
			ResolvedYaw = record->m_anim_state->m_flEyeYaw - record->m_max_desync_delta;
		else
			ResolvedYaw = record->m_anim_state->m_flEyeYaw + record->m_max_desync_delta;
	}
	else
	{
		// ресольвим этого пездюка хуле.
		if (!log.m_nMissedShotsResolver) {
			ResolvedYaw = record->m_anim_state->m_flEyeYaw + record->m_max_desync_delta;
			log.m_nLastSide = 1;
			log.m_nCurrentSide = log.m_nLastSide;
		}
		else {
			switch (log.m_nMissedShotsResolver % 3) {
			case 0:
				ResolvedYaw = record->m_anim_state->m_flEyeYaw - (log.m_nLastSide * record->m_max_desync_delta);
				log.m_nCurrentSide = -log.m_nLastSide;
				break;
			case 1:
				ResolvedYaw = record->m_anim_state->m_flEyeYaw;
				log.m_nCurrentSide = 0;
				break;
			case 2:
				ResolvedYaw = record->m_anim_state->m_flEyeYaw + (log.m_nLastSide * record->m_max_desync_delta);
				log.m_nCurrentSide = log.m_nLastSide;
				break;
			}
		}
	}

	ResolvedYaw = math::get().normalize_float(ResolvedYaw); // клемпим наш финальный угол.

	record->m_anim_state->m_flGoalFeetYaw = ResolvedYaw; // ну и применяем его а хули.
}

void resolver::add_shot(const Vector & shotpos, lag_record_t * record, const int& enemy_index, studiohdr_t* hdr, int hitbox) {
	static auto last_tick = 0;
	if (g_pGlobals->tickcount != last_tick)
	{
		shots.emplace_back(shotpos, g_pGlobals->tickcount, enemy_index, record, hdr, hitbox);
		last_tick = g_pGlobals->tickcount;
	}
}

void resolver::update_missed_shots(const ClientFrameStage_t& stage) {
	if (stage != FRAME_NET_UPDATE_START)
		return;

	auto it = shots.begin();
	while (it != shots.end())
	{
		const auto shot = *it;
		if (shot.tick + 128 < g_pGlobals->tickcount)
		{
			it = shots.erase(it);
		}
		else
		{
			++it;
		}
	}

	auto it2 = current_shots.begin();
	while (it2 != current_shots.end())
	{
		const auto shot = *it2;
		if (shot.tick + 128 < g_pGlobals->tickcount)
		{
			it2 = current_shots.erase(it2);
		}
		else
		{
			++it2;
		}
	}
}

std::deque<shot_t>& resolver::get_shots() {
	return shots;
}

void resolver::hurt_listener(IGameEvent * game_event) {
	if (shots.empty())
		return;

	_(attacker_s, "attacker");
	_(userid_s, "userid");
	_(hitgroup_s, "hitgroup");
	_(dmg_health_s, "dmg_health");

	const auto attacker = g_pEngine->GetPlayerForUserID(game_event->GetInt(attacker_s));
	const auto victim = g_pEngine->GetPlayerForUserID(game_event->GetInt(userid_s));
	const auto hitgroup = game_event->GetInt(hitgroup_s);
	const auto damage = game_event->GetInt(dmg_health_s);

	if (attacker != g_pEngine->GetLocalPlayer())
		return;

	if (victim == g_pEngine->GetLocalPlayer())
		return;

	auto player = get_entity(victim);
	if (!player || !player->is_enemy())
		return;

	if (unapproved_shots.empty())
		return;

	for (auto& shot : unapproved_shots)
	{
		if (!shot.hurt)
		{
			shot.hurt = true;
			shot.hitinfo.victim = victim;
			shot.hitinfo.hitgroup = hitgroup;
			shot.hitinfo.damage = damage;
			return;
		}
	}
}

shot_t* resolver::closest_shot(int tickcount) {
	shot_t* closest_shot = nullptr;
	auto closest_diff = 128;
	for (auto& shot : shots)
	{
		const auto diff = abs(tickcount - shot.tick);
		if (diff <= closest_diff)
		{
			closest_shot = &shot;
			closest_diff = diff;
			continue;
		}


		break;
	}

	return closest_shot;
}

void resolver::record_shot(IGameEvent * game_event) {
	_(userid_s, "userid");

	const auto userid = g_pEngine->GetPlayerForUserID(game_event->GetInt(userid_s));
	const auto player = get_entity(userid);

	if (player != g_pLocalPlayer)
		return;

	const auto shot = closest_shot(g_pGlobals->tickcount - time_to_ticks(g_pEngine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING)));
	if (!shot)
		return;

	current_shots.push_back(*shot);
}

void resolver::listener(IGameEvent * game_event) {
	_(weapon_fire, "weapon_fire");
	if (!strcmp(game_event->GetName(), weapon_fire))
	{
		record_shot(game_event);
		return;
	}

	if (current_shots.empty())
		return;

	_(userid_s, "userid");

	const auto userid = g_pEngine->GetPlayerForUserID(game_event->GetInt(userid_s));
	const auto player = get_entity(userid);
	if (!player || player != g_pLocalPlayer)
		return;

	_(x, "x");
	_(y, "y");
	_(z, "z");

	const Vector pos(game_event->GetFloat(x), game_event->GetFloat(y), game_event->GetFloat(z));

	if (cfg.visuals.effects.bullet_impacts)
		g_pDebugOverlay->AddBoxOverlay(pos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 0, 0, 155, 127, 4);

	auto shot = &current_shots[0];

	static auto last_curtime = 0.f;
	static auto last_length = 0.f;
	static auto counter = 0;

	if (last_curtime == g_pGlobals->curtime)
		counter++;
	else
	{
		counter = 0;
		last_length = 0.f;
	}

	if (last_curtime == g_pGlobals->curtime && shot->shotpos.Dist(pos) <= last_length)
		return;

	last_length = shot->shotpos.Dist(pos);

	if (counter)
		unapproved_shots.pop_front();

	shot->hitpos = pos;
	unapproved_shots.push_back(*shot);

	last_curtime = g_pGlobals->curtime;
}

void resolver::approve_shots(const ClientFrameStage_t& stage) {
	if (stage != FRAME_NET_UPDATE_START)
		return;

	for (auto& shot : unapproved_shots)
	{
		const auto angles = math::get().calc_angle(shot.shotpos, shot.hitpos);
		Vector direction;
		math::get().angle_vectors(angles, &direction);
		direction.Normalize();

		if (shot.record.index == -1)
		{
			if (shot.hurt)
			{
				const auto player = get_entity(shot.hitinfo.victim);
				if (player)
				{
					hitmarker::get().add_hit(hitmarker_t(g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.hitpos));
					beams::get().add_local_beam(impact_info_t(g_pGlobals->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer, cfg.visuals.effects.bullet_tracers_color));
					continue;
				}

			}
			beams::get().add_local_beam(impact_info_t(g_pGlobals->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer, cfg.visuals.effects.bullet_tracers_color));
			continue;
		}

		auto player = get_entity(shot.enemy_index);
		if (!player || player->IsDormant() || !player->get_alive())
		{
			if (shot.hurt)
			{
				hitmarker::get().add_hit(hitmarker_t(g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.shotpos + direction * shot.shotpos.Dist(shot.record.m_origin)));

				if (!beams::get().beam_exists(g_pLocalPlayer, g_pGlobals->curtime))
					beams::get().add_local_beam(impact_info_t(g_pGlobals->curtime, shot.shotpos, shot.shotpos + direction * shot.shotpos.Dist(shot.record.m_origin), g_pLocalPlayer, cfg.visuals.effects.bullet_tracers_color));
			}
			else
				beams::get().add_local_beam(impact_info_t(g_pGlobals->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer, cfg.visuals.effects.bullet_tracers_color));

			continue;
		}

		if (ragebot::get().can_hit_hitbox(shot.shotpos, shot.hitpos, shot.record, shot.hdr, shot.hitbox))
		{
			shot.hit = true;
		}

		beams::get().add_local_beam(impact_info_t(g_pGlobals->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer, cfg.visuals.effects.bullet_tracers_color));

		if (shot.hurt)
			hitmarker::get().add_hit(hitmarker_t(g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.hitpos ));

		calc_missed_shots(&shot);
	}

	current_shots.clear();
	unapproved_shots.clear();
}


void resolver::calc_missed_shots(shot_t* shot) {
	const auto log = &player_log::get().get_log(shot->enemy_index);
	if (shot->hurt)
		return;

	_(skeet_idb, "[skeet.idb] ");
	_(sprd, "Missed shot due to spread\n");
	_(rslvr, "Missed shot due to bad resolve\n");

	if (shot->hit)
	{
		if (cfg.ragebot.other.anti_aim_correction)
		{
			log->m_nMissedShotsResolver++;
			g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
			util::print_dev_console(true, Color(255, 255, 255, 255), rslvr);
		}
	}
	else if (cfg.ragebot.aimbot.log_misses_due_to_spread)
	{
		log->m_nMissedShotsSpread++;
		g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
		util::print_dev_console(true, Color(255, 255, 255, 255), sprd);
	}
}