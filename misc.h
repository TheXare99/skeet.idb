#pragma once

class misc : public singleton<misc>
{
public:
	static void guard_detect();
	static void update_keybinds();
	static void bhop();
	static void remove_smoke( ClientFrameStage_t stage );
	static void remove_flash( ClientFrameStage_t stage );
	static void radar();
	static void reveal_ranks();
	static void clan_tag();
	static void chat_spam();
};
