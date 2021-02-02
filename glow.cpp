#include "../include_cheat.h"

void glow::draw()
{
	if ( (!cfg.visuals.player_esp.glow) )
		return;

	g_pGlowObjectManager->RenderGlowEffects();
}
