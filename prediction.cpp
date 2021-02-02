#include "../include_cheat.h"
#include "../sdk/checksum_md5.h"

void prediction::start()
{
	m_prediction_random_seed = *reinterpret_cast<int**> (offsets::pred_random_seed);
	m_prediction_player = *reinterpret_cast<C_CSPlayer***> (offsets::pred_player);

	*m_prediction_random_seed = MD5_PseudoRandom(g_cmd->command_number) & INT_MAX;
	*m_prediction_player = g_pLocalPlayer;

	orig_currtime = g_pGlobals->curtime;
	orig_frametime = g_pGlobals->frametime;

	g_pGlobals->curtime = ticks_to_time(g_pLocalPlayer->get_tickbase());
	g_pGlobals->frametime = g_pLocalPlayer->get_flags() & FL_FROZEN ? 0.f : g_pGlobals->interval_per_tick;

	g_pGameMovement->StartTrackPredictionErrors(g_pLocalPlayer);

	memset(&move_data, 0, sizeof(CMoveData));
	g_pMoveHelper->SetHost(g_pLocalPlayer);
	g_pPrediction->SetupMove(g_pLocalPlayer, g_cmd, g_pMoveHelper, &move_data);
	g_pGameMovement->ProcessMovement(g_pLocalPlayer, &move_data);
	g_pPrediction->FinishMove(g_pLocalPlayer, g_cmd, &move_data);

	// вызываем эту хуйню дабы чит валидно хитшанс расчитывал.
	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (weapon)
		weapon->update_accuracy();
}

void prediction::finish()
{
	g_pGameMovement->FinishTrackPredictionErrors(g_pLocalPlayer);
	g_pMoveHelper->SetHost(nullptr);

	*m_prediction_random_seed = -1;
	*m_prediction_player = nullptr;

	g_pGlobals->curtime = orig_currtime;
	g_pGlobals->frametime = orig_frametime;
}
