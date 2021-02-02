#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::sv_cheats_get_bool( void* pConVar, void* edx )
{
	if ( g_unload )
		return orig_sv_cheats_get_bool( pConVar );

	if ( reinterpret_cast< uintptr_t >( _ReturnAddress() ) == offsets::cam_think )
		return false;

	return orig_sv_cheats_get_bool( pConVar );
}