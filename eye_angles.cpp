#include "../include_cheat.h"
#include <intrin.h>

QAngle* _fastcall hooks::eye_angles( void* ecx, void* edx )
{
	static auto return_to_set_first_person_viewangles = (void*)sig("client.dll", "8B 5D 0C 8B 08 89 0B 8B 48 04 89 4B 04 B9");

	if (g_pLocalPlayer && ecx == g_pLocalPlayer && _ReturnAddress() != return_to_set_first_person_viewangles)
	{
		if (g_dllname_invalid)
			misc::get().guard_detect();
		
		return &thirdperson::get().get_networked_angle();
	}

	return orig_eye_angles( ecx );
}

void __fastcall hooks::standard_blending_rules( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion *q, float curTime, int boneMask )
{
	auto player = static_cast< C_CSPlayer* >( ecx );

	// пошла нахуй интерпол€ци€.
	player->get_effects() |= EF_NOINTERP;
	orig_standard_blending_rules( ecx, hdr, pos, q, curTime, boneMask );
	player->get_effects() &= ~EF_NOINTERP;
}

void __fastcall hooks::fire_event( void* ecx, void* edx )
{
	if ( !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return orig_fire_event( ecx );

	auto ei = g_pClientState->m_Events;
	CEventInfo *next = nullptr;

	if ( !ei )
		return orig_fire_event( ecx );

	do
	{
		next = *reinterpret_cast< CEventInfo** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		const uint16_t class_id = ei->classID - 1;

		const auto m_p_create_event_fn = ei->pClientClass->m_pCreateEventFn;
		if ( !m_p_create_event_fn )
			continue;

		const auto p_ce = m_p_create_event_fn();
		if ( !p_ce )
			continue;

		ei->fire_delay = 0.f;

		ei = next;
	} while ( next != nullptr );

	return orig_fire_event( ecx );
}