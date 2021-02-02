#pragma once

class antiaim : public singleton<antiaim>
{
public:
	void fake_lags();
	void slow_walk();
	void run();
	static void fix_movement();
	void choose_real();
private:
	void do_at_target() const;
	void lby_prediction();
	float next_lby_update = 0.f;
	bool update_lby = false;
};
