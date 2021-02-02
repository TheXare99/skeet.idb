#pragma once

class preserve_death_notices : public singleton<preserve_death_notices>
{
public:
	static void run(ClientFrameStage_t stage);
};
