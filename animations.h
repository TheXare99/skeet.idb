#pragma once

struct anim_state_info_t
{
	CBaseHandle	handle;
	float spawn_time;
	CCSGOPlayerAnimState* animstate;
	float animvel;
	std::array<float, 24> poses;
};

class animations : public singleton<animations>
{
public:
	void fix_local_anims( const ClientFrameStage_t stage );
	QAngle m_angle;
	static void update_player_animations( lag_record_t* record, C_CSPlayer* m_player );
	void update_custom_anims( C_CSPlayer* player );
private:
	QAngle m_rotation;
	std::array<C_AnimationLayer, 13> m_layers;
	std::array<float, 24> m_poses;
	anim_state_info_t anim_info[ 65 ];
};