#include "../include_cheat.h"

void __cdecl hooks::smoke_effect( CRecvProxyData* proxy_data_const, void* entity, void* output ) {
	if ( g_unload )
		orig_smoke_effect( proxy_data_const, entity, output );

	if (cfg.visuals.effects.remove_smoke_effects)
		*reinterpret_cast< bool* >( reinterpret_cast< uintptr_t >( output ) + 0x1 ) = true;
}