#include "../include_cheat.h"

void __stdcall hooks::frame_stage_notify( ClientFrameStage_t stage )
{
	if ( g_erase )
	{
		erase_fn( init::on_startup );
		g_erase = false;
	}

	chams::get().dark_mode();

	if ( g_unload )
		return orig_frame_stage_notify( stage );

	//preserve_death_notices::run( stage );

	g_pEngine->GetViewAngles(g_original_viewangles);

	misc::remove_smoke( stage );

	misc::remove_flash( stage );

	resolver::get().approve_shots( stage );

	resolver::get().update_missed_shots( stage );

	animations::get().fix_local_anims( stage );

	orig_frame_stage_notify( stage );

	player_log::get().log( stage );
}