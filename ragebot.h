#pragma once

struct aimpoint_t
{
	aimpoint_t(const Vector& point, const int hitbox)
	{
		this->point = point;
		this->hitbox = hitbox;
	}
	Vector point;
	int hitbox;
};

class ragebot : public singleton<ragebot>
{
public:
	void run();
	static void no_recoil();
	static void no_visual_recoil(CViewSetup & v_view);
	void auto_stop();
	static bool can_shoot(float time = g_pGlobals->curtime, bool check = false);
	aimpoint_t get_best_damage(lag_record_t& log, float& damage);
	void multipoint_hitboxes(lag_record_t & log, std::vector<aimpoint_t>& points);
	static bool can_hit_hitbox(const Vector start, const Vector end, lag_record_t& record, studiohdr_t * hdr, int box);
	bool get_ragebot_state() const
	{
		return ragebot_called == g_pGlobals->curtime;
	}
private:
	void select_targets();
	void scan_targets();

	static void build_seed_table();

	bool calculate_hitchance(const aimpoint_t & point, lag_record_t& record, studiohdr_t* hdr) const;

	std::vector<aim_data_t> targets;
	std::array<bool, 21> enabled_hitboxes;
	float ragebot_called = 0.f;
};
