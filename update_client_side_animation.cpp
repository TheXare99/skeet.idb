#include "../include_cheat.h"

void __fastcall hooks::update_client_side_animation(void* ecx, void* edx)
{
	if (ecx != g_pLocalPlayer)
		return orig_update_client_side_animation(ecx);

	// �������� �������� ���� ��� � ���.
	if (g_should_animate)
		orig_update_client_side_animation(ecx);
}