#include "../include_cheat.h"

void __stdcall hooks::override_view( CViewSetup* setup )
{
	if ( g_unload )
		return orig_override_view( setup );

	thirdperson::get().run();

	orig_override_view( setup );

	if ( g_pEngine->IsInGame() )
	{
		*g_disable_post_processing = cfg.visuals.effects.disable_post_processing;

		// спиздил с зевса куз це пиздато.
		const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
		if (weapon && g_pLocalPlayer->get_scoped())
		{
			auto zoom_level = weapon->get_zoom_level();

			switch (zoom_level)
			{
			case 1:
				setup->fov = cfg.misc.miscellaneous.override_fov;
				break;
			case 2:
				setup->fov = cfg.misc.miscellaneous.override_fov / 2;
				break;
			default:
				setup->fov = cfg.misc.miscellaneous.override_fov;
				break;
			}
		}
		else
			setup->fov = cfg.misc.miscellaneous.override_fov;
	}
}