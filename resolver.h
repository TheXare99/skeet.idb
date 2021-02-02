#pragma once

struct shot_t
{
	shot_t( const Vector& shotpos, const int tick, const int& enemy_index, const lag_record_t* record, studiohdr_t* hdr, int hitbox)
	{
		this->shotpos = shotpos;
		this->enemy_index = enemy_index;
		this->tick = tick;
		if ( record )
			this->record = *record;
		else
			this->record.index = -1;
		if (hdr)
			this->hdr = hdr;
		else
			this->hdr = nullptr;
		this->hitbox = hitbox;
		this->hit = false;
		this->hurt = false;
	}
	Vector shotpos;
	Vector hitpos;
	bool hit;
	bool hurt;
	struct
	{
		int victim = -1;
		int damage = -1;
		int hitgroup = -1;
	}hitinfo;
	int tick;
	int enemy_index;
	lag_record_t record;
	studiohdr_t* hdr;
	int hitbox;
};

class resolver : public singleton<resolver>
{
public:
	void resolve( C_CSPlayer* player, lag_record_t* record );
	void add_shot(const Vector & shotpos, lag_record_t * record, const int & enemy_index, studiohdr_t* hdr = nullptr, int hitbox = -1);
	void update_missed_shots( const ClientFrameStage_t& stage );
	std::deque<shot_t>& get_shots();
	static void calc_missed_shots( shot_t* shot );
	void hurt_listener( IGameEvent * game_event );
	shot_t* closest_shot( int tickcount );
	void record_shot( IGameEvent* game_event );
	void listener( IGameEvent * game_event );
	void approve_shots( const ClientFrameStage_t & stage );
private:
	std::deque<shot_t> shots;
	std::deque<shot_t> current_shots;
	std::deque<shot_t> unapproved_shots;
};