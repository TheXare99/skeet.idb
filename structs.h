#pragma once

struct aim_data_t
{
	aim_data_t( C_CSPlayer* player, const int& idx )
	{
		this->player = player;
		this->index = idx;
	}
	C_CSPlayer*	player;
	int			index;
};

struct visual_data_t
{
	visual_data_t()
	{
		this->player = nullptr;
		this->index = -1;
		this->fade_time = 0.f;
		this->alpha = 0.f;
		this->valid = false;
		this->was_unseen = true;
		this->oof = false;
	}

	C_CSPlayer*	player;
	bool		valid;
	int			index;
	float		fade_time;
	float		alpha;
	Vector		top;
	Vector		bot;
	float		width;
	float		height;
	bool was_unseen;
	bool oof;
	Vector pos;

	//extra
	float vel;
	int act;
	float l1_weight;
	float l1_cycle;
	bool dormant;
	int maxclip;
	int clip;
	int armor;
	int health;
	bool scoped;
	bool reload;
	float ping;
	bool bomb;

	//record
	int shots;
	std::string name;
	std::string weapon_name;
	std::string rmode;
};

struct world_data_t
{
	int classid;
	float alpha;
	Vector pos;
	std::string name;
	float explode_time;
	int clip;
	int maxclip;
	bool defusing;
	int32 defuser;
};


struct visual_world_data_t
{
	std::vector<world_data_t> world_data;
};
	

class C_AnimationLayer;
class CCSGOPlayerAnimState;
struct lag_record_t
{
	lag_record_t() = default;
	lag_record_t(C_CSPlayer* player);
	void apply(C_CSPlayer* player, bool backup = false) const;

	int index;
	bool valid, has_anim_state;
	matrix3x4_t matrix[MAX_SETUP_BONES];

	bool m_dormant;
	Vector m_velocity;
	Vector m_origin;
	Vector m_abs_origin;
	Vector m_anim_velocity;
	Vector m_obb_mins;
	Vector m_obb_maxs;
	std::array<C_AnimationLayer, 13> m_layers;
	std::array<C_AnimationLayer, 13> m_resolver_layers[3];
	std::array<float, 24> m_poses;
	CCSGOPlayerAnimState* m_anim_state;
	float m_max_desync_delta;
	float m_anim_time;
	float m_sim_time;
	float m_old_sim_time;
	float m_last_shot_time;
	float m_duck;
	float m_body;
	QAngle m_eye_angles;
	QAngle m_abs_ang;
	int m_flags;
	int m_lag;
	QAngle m_rotation;

	int m_cached_count;
	int m_writable_bones;

	bool m_shot;
};

struct dormant_record_t
{
	QAngle m_abs_ang;
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flLeanAmount; //0x90
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
};


struct player_log_t
{
	player_log_t()
	{
		player = nullptr;
		lastvalid = 0;
		lastvalid_vis = 0;
	}
	C_CSPlayer*	player;

	int lastvalid;
	int lastvalid_vis;

	float m_flSpawntime;

	float  m_flLastResolvedYaw;

	int m_nMissedShotsResolver;
	int m_nMissedShotsSpread;
	int m_nLastSide;
	int m_nCurrentSide;

	std::deque<lag_record_t> record;
};

struct FireBulletData
{
	Vector           src;
	trace_t          enter_trace;
	Vector           direction;
	ITraceFilter     filter;
	CCSWeaponData*  wpn_data;
	float           trace_length;
	float           trace_length_remaining;
	float           length_to_end;
	float           current_damage;
	int             penetrate_count;
};