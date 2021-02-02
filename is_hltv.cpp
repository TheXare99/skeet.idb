#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::is_hltv( void* ecx, void* edx )
{
	static auto setup_velocity = reinterpret_cast< uintptr_t* >( sig( "client.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" ) );

	if (_ReturnAddress() == setup_velocity)
		return true;

	static auto accumulate_layers = reinterpret_cast<uintptr_t*>(sig("client.dll", "84 C0 75 0D F6 87"));

	if (_ReturnAddress() == accumulate_layers)
		return true;

	return orig_is_hltv( ecx );
}