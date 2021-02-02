#include "../include_cheat.h"

void animations::fix_local_anims(const ClientFrameStage_t stage)
{
	if (!g_pLocalPlayer || !g_pLocalPlayer->get_alive())
		return;

	const auto animstate = g_pLocalPlayer->get_anim_state();

	if (!animstate)
		return;

	if (!g_pInput->m_fCameraInThirdPerson)
	{
		animstate->m_szInHitGroundAnimation = false;
		return;
	}

	if (stage == FRAME_RENDER_START)
	{
		if (g_should_animate) {
			m_layers = g_pLocalPlayer->get_anim_layers();
			g_pLocalPlayer->update_clientside_anim();
			if (!g_pClientState->m_nChokedCommands) {
				m_rotation.y = animstate->m_flGoalFeetYaw;
				m_poses = g_pLocalPlayer->get_pose_params();
			}
			g_should_animate = false;
		}
		g_pLocalPlayer->set_anim_layers(m_layers);
		g_pLocalPlayer->set_pose_params(m_poses);
		g_pLocalPlayer->set_abs_angles(m_rotation);
	}
}

struct AnimationBackup_t
{
	Vector m_origin;
	Vector m_abs_origin;
	Vector m_velocity;
	Vector m_abs_velocity;
	int m_flags;
	int m_eflags;
	float m_duck;
	float m_body;
	std::array<C_AnimationLayer, 13> m_layers;
};

void ResetAnimationState( CCSGOPlayerAnimState* state )
{
	using ResetAnimState_t = void( __thiscall* )( CCSGOPlayerAnimState* );

	static auto ResetAnimState = reinterpret_cast< ResetAnimState_t >( sig( "client.dll", "56 6A 01 68 ? ? ? ? 8B F1" ) );
	if ( !ResetAnimState )
		return;

	ResetAnimState( state );
}

void animations::update_player_animations(lag_record_t* record, C_CSPlayer* m_player)
{
	auto log = &player_log::get().get_log(m_player->EntIndex());

	auto state = record->m_anim_state;
	if (!state)
		return;

	// player respawned.
	if (m_player->get_spawn_time() != log->m_flSpawntime)
	{
		// reset animation state.
		ResetAnimationState(state);

		// note new spawn time.
		log->m_flSpawntime = m_player->get_spawn_time();
	}

	// backup curtime.
	float curtime = g_pGlobals->curtime;
	float frametime = g_pGlobals->frametime;

	// set curtime to animtime.
	// set frametime to ipt just like on the server during simulation.
	g_pGlobals->curtime = record->m_anim_time;
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	// backup stuff that we do not want to fuck with.
	AnimationBackup_t backup;

	backup.m_origin = m_player->get_origin();
	backup.m_abs_origin = m_player->get_abs_origin();
	backup.m_velocity = m_player->get_velocity();
	backup.m_abs_velocity = m_player->get_abs_velocity();
	backup.m_flags = m_player->get_flags();
	backup.m_eflags = m_player->get_eflags();
	backup.m_duck = m_player->get_duck_amt();
	backup.m_body = m_player->get_lby();
	backup.m_layers = m_player->get_anim_layers();

	// fix velocity.
	if (record->m_lag > 0 && record->m_lag < 16 && log->record.size() >= 2)
	{
		// get pointer to previous record.
		auto previous = &log->record[1];

		if (previous && !previous->m_dormant)
			record->m_velocity = (record->m_origin - previous->m_origin) * (1.f / ticks_to_time(record->m_lag));
	}

	// set this fucker, it will get overriden.
	record->m_anim_velocity = record->m_velocity;

	// did the player shoot?
	record->m_shot = record->m_last_shot_time == record->m_sim_time;

	// fix various issues with the game
	// these issues can only occur when a player is choking data.
	if (record->m_lag > 1 && !m_player->get_player_info().fakeplayer)
	{
		// we need atleast 2 updates/records
		// to fix these issues.
		if (log->record.size() >= 2)
		{
			// get pointer to previous record.
			auto previous = &log->record[1];

			if (previous && !previous->m_dormant)
			{
				// set previous flags.
				m_player->get_flags() = previous->m_flags;

				// strip the on ground flag.
				m_player->get_flags() &= ~FL_ONGROUND;

				// been onground for 2 consecutive ticks? fuck yeah.
				if (record->m_flags & FL_ONGROUND && previous->m_flags & FL_ONGROUND)
					m_player->get_flags() |= FL_ONGROUND;

				// fix jump_fall.
				else
				{
					if (record->m_layers[4].m_flWeight != 1.f && previous->m_layers[4].m_flWeight == 1.f && record->m_layers[5].m_flWeight != 0.f)
						m_player->get_flags() |= FL_ONGROUND;

					if (record->m_flags & FL_ONGROUND && !(previous->m_flags & FL_ONGROUND))
						m_player->get_flags() &= ~FL_ONGROUND;
				}

				// calc fraction between both records.
				float frac = (record->m_anim_time - record->m_sim_time) / (previous->m_sim_time - record->m_sim_time);

				m_player->get_duck_amt() = math::lerp(record->m_duck, previous->m_duck, frac);

				record->m_anim_velocity = math::lerp(record->m_velocity, previous->m_velocity, frac);
			}
		}
	}

	resolver::get().resolve(m_player, record);

	// set stuff before animating.
	m_player->get_origin() = record->m_origin;
	m_player->set_abs_origin(record->m_origin);

	m_player->get_velocity() = m_player->get_abs_velocity() = record->m_anim_velocity;
	m_player->get_lby() = record->m_body;

	// EFL_DIRTY_ABSVELOCITY
	// skip call to C_BaseEntity::CalcAbsoluteVelocity
	m_player->get_eflags() &= ~0x1000;

	const auto backup_eye = m_player->get_eye_angles();

	m_player->get_anim_state() = state;

	// fix animating in same frame.
	if (state->m_iLastClientSideAnimationUpdateFramecount == g_pGlobals->framecount)
		state->m_iLastClientSideAnimationUpdateFramecount -= 1;

	m_player->get_clientside_animation() = true;
	m_player->update_clientside_anim();
	m_player->get_clientside_animation() = false;

	m_player->get_eye_angles() = backup_eye;

	// store updated/animated poses and rotation in lagrecord.
	record->m_poses = m_player->get_pose_params();
	record->m_abs_ang = m_player->get_abs_angles();

	// restore backup data.
	m_player->get_origin() = backup.m_origin;
	m_player->get_velocity() = backup.m_velocity;
	m_player->get_abs_velocity() = backup.m_abs_velocity;
	m_player->get_flags() = backup.m_flags;
	m_player->get_eflags() = backup.m_eflags;
	m_player->get_duck_amt() = backup.m_duck;
	m_player->get_lby() = backup.m_body;
	m_player->set_abs_origin(backup.m_abs_origin);
	m_player->set_anim_layers(backup.m_layers);

	// IMPORTANT: do not restore poses here, since we want to preserve them for rendering.
	// also dont restore the render angles which indicate the model rotation.

	// restore globals.
	g_pGlobals->curtime = curtime;
	g_pGlobals->frametime = frametime;
}

void animations::update_custom_anims(C_CSPlayer * player)
{
	for (auto i = 1; i <= g_pGlobals->maxClients; i++)
	{
		const auto ent = get_entity(i);
		auto& info = anim_info[i];
		if ((!ent || ent != g_pLocalPlayer) && info.animstate)
		{
			g_pMemAlloc->Free(info.animstate);
			info.animstate = nullptr;
		}
	}

	if (!player)
		return;

	auto& info = anim_info[player->EntIndex()];

	if (player->get_spawn_time() != info.spawn_time || player->GetRefEHandle() != info.handle || !player->get_alive())
	{
		info.spawn_time = player->get_spawn_time();
		info.handle = player->GetRefEHandle();

		if (info.animstate)
		{
			g_pMemAlloc->Free(info.animstate);
			info.animstate = nullptr;
		}

		if (!player->get_alive())
			return;
	}

	if (!info.animstate)
	{
		info.animstate = reinterpret_cast<CCSGOPlayerAnimState*> (g_pMemAlloc->Alloc(860u));
		player->create_anim_state(info.animstate);
	}

	// backup curtime.
	const auto curtime = g_pGlobals->curtime;
	const auto frametime = g_pGlobals->frametime;

	// set curtime to animtime.
	// set frametime to ipt just like on the server during simulation.
	g_pGlobals->curtime = player->get_oldsimtime() + g_pGlobals->interval_per_tick;
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	// backup stuff that we do not want to fuck with.
	const auto backup_origin = player->get_origin();
	const auto backup_velocity = player->get_velocity();
	const auto backup_abs_velocity = player->get_abs_velocity();
	const auto backup_flags = player->get_flags();
	const auto backup_eflags = player->get_eflags();
	const auto backup_duck_amt = player->get_duck_amt();
	const auto backup_lby = player->get_lby();
	auto backup_layers = player->get_anim_layers();
	auto backup_poses = player->get_pose_params();

	if (info.animstate->m_iLastClientSideAnimationUpdateFramecount == g_pGlobals->framecount)
		info.animstate->m_iLastClientSideAnimationUpdateFramecount -= 1;

	info.animstate->update(player->get_eye_angles());
	info.animvel = info.animstate->m_flSpeed;

	player->get_origin() = backup_origin;
	player->get_velocity() = backup_velocity;
	player->get_abs_velocity() = backup_abs_velocity;
	player->get_flags() = backup_flags;
	player->get_eflags() = backup_eflags;
	player->get_duck_amt() = backup_duck_amt;
	player->get_lby() = backup_lby;
	player->set_anim_layers(backup_layers);
	player->set_pose_params(backup_poses);

	g_pGlobals->curtime = curtime;
	g_pGlobals->frametime = frametime;
}
