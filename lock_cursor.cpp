#include "../include_cheat.h"

void __fastcall hooks::lock_cursor( void* ecx, void* )
{
	if (Menu::get().m_bIsOpen)
	{
		g_pSurface->UnlockCursor();
		return;
	}
	
	orig_lock_cursor( ecx );
}