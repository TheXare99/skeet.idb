#include "../include_cheat.h"

bool __fastcall hooks::send_net_msg( void* netchan, void* edx, INetMessage& msg, bool reliable, bool voice )
{
	if ( msg.GetGroup() == 15 ) // ����� ������ �� ����������, � �� �� �����.
		return false;

	if ( msg.GetGroup() == 9 ) // ����� �� ������ � ����������, ����� � �������� � ����� ����������.
		voice = true;

	return orig_send_net_msg( netchan, msg, reliable, voice );
}