#pragma once

class lagcomp : public singleton<lagcomp>
{
public:
	float get_max_desync_delta(CCSGOPlayerAnimState * m_anim_state);
	static int fix_tickcount( const float& simtime );
	static bool valid_simtime( const float & simtime );
	static float get_lerp_time();
};
