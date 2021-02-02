#pragma once

class prediction : public singleton<prediction>
{
public:
	void start();
	void finish();
	QAngle unpred_viewangles;
	QAngle strafe_angles;
private:
	CMoveData move_data;
	float orig_currtime = 0.f;
	float orig_frametime = 0.f;

	int* m_prediction_random_seed = nullptr;
	C_CSPlayer** m_prediction_player = nullptr;
};