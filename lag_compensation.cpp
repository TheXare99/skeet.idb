#include "../include_cheat.h"

lag_record_t::lag_record_t( C_CSPlayer* player )
{
	index = player->EntIndex();
	valid = false;
	m_dormant = player->IsDormant();
	m_velocity = player->get_velocity();
	m_origin = player->get_origin();
	m_abs_origin = player->get_abs_origin();
	m_layers = player->get_anim_layers();
	m_poses = player->get_pose_params();
	if ((has_anim_state = player->get_anim_state()))
	{
		m_anim_state = player->get_anim_state();
		m_max_desync_delta = lagcomp::get().get_max_desync_delta(m_anim_state);
	}
	m_anim_time = player->get_oldsimtime() + g_pGlobals->interval_per_tick;
	m_last_shot_time = get_weapon(player->get_active_weapon()) ? get_weapon(player->get_active_weapon())->get_last_shot_time() : 0.f;
	m_sim_time = player->get_simtime();
	m_old_sim_time = player->get_oldsimtime();
	m_duck = player->get_duck_amt();
	m_body = player->get_lby();
	m_eye_angles = player->get_eye_angles();
	m_abs_ang = player->get_abs_angles();
	m_flags = player->get_flags();
	m_rotation = player->get_rotation();
	m_lag = time_to_ticks( player->get_simtime() - player->get_oldsimtime() );
	const auto collideable = player->GetCollideable();
	m_obb_maxs = collideable->OBBMaxs();
	m_obb_mins = collideable->OBBMins();
	m_cached_count = player->get_bone_cache()->m_CachedBoneCount;
	if ( m_cached_count )
	{
		memcpy( matrix, player->get_bone_cache()->m_pCachedBones, 48 * m_cached_count );
		m_writable_bones = *( &player->get_bone_accessor()->m_WritableBones + 8 );
	}
	else m_writable_bones = 0;
}

void lag_record_t::apply( C_CSPlayer* player, bool backup ) const
{
	player->get_flags() = m_flags;
	player->get_simtime() = m_sim_time;
	player->get_eye_angles() = m_eye_angles;
	player->set_abs_angles( m_abs_ang );
	player->get_velocity() = m_velocity;
	player->set_abs_origin( backup ? m_abs_origin : m_origin );
	player->get_origin() = m_origin;
	player->get_lby() = m_body;
	player->get_duck_amt() = m_duck;
	const auto collideable = player->GetCollideable();
	collideable->OBBMaxs() = m_obb_maxs;
	collideable->OBBMins() = m_obb_mins;

	if ( m_cached_count && m_cached_count > 0 )
	{
		memcpy( player->get_bone_cache()->m_pCachedBones, matrix, 48 * m_cached_count );
		*( &player->get_bone_accessor()->m_WritableBones + 8 ) = m_writable_bones;
	}
}

float lagcomp::get_max_desync_delta(CCSGOPlayerAnimState* m_anim_state)
{
	float speedfactor = m_anim_state->m_flFeetSpeedForwardsOrSideways;
	float unk1 = ((m_anim_state->m_flStopToFullRunningFraction * -.3f) - .2f) * speedfactor;
	float unk2 = unk1 + 1.f;

	if (m_anim_state->m_flDuckAmount > 0.0f)
	{
		float max_velocity = m_anim_state->m_flFeetSpeedForwardsOrSideways;
		float duck_speed = m_anim_state->m_flDuckAmount * max_velocity;
		unk2 += (duck_speed * (0.5f - unk2));
	}

	return *(float*)((uintptr_t)m_anim_state + 0x334) * unk2;
}

int lagcomp::fix_tickcount( const float& simtime )
{
	return time_to_ticks( simtime + get_lerp_time() );
}

float lagcomp::get_lerp_time()
{
	__(cl_updaterate_s, "cl_updaterate");
	__(cl_interp_s, "cl_interp");

	static auto cl_updaterate = g_pCVar->FindVar(cl_updaterate_s);
	static auto cl_interp = g_pCVar->FindVar(cl_interp_s);

	const auto update_rate = cl_updaterate->get_int();
	const auto interp_ratio = cl_interp->get_float();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= cl_interp->get_float())
		lerp = cl_interp->get_float();

	return lerp;
}

bool lagcomp::valid_simtime( const float & simtime )
{
	const auto nci = g_pEngine->GetNetChannelInfo();
	if (!nci)
		return false;

	float correct = 0.f;
	static auto sv_maxunlag = g_pCVar->FindVar("sv_maxunlag");
	correct += nci->GetLatency(FLOW_OUTGOING);
	correct += nci->GetLatency(FLOW_INCOMING);
	correct += get_lerp_time();

	const auto delta_time = correct - (g_pGlobals->curtime - simtime);

	return fabsf(delta_time) <= .2f && correct < sv_maxunlag->get_float();
}