#include "../include_cheat.h"

bool __fastcall hooks::send_net_msg( void* netchan, void* edx, INetMessage& msg, bool reliable, bool voice )
{
	if ( msg.GetGroup() == 15 ) // чтобы сервер не доебывался, а то он охуел.
		return false;

	if ( msg.GetGroup() == 9 ) // чтобы не лагать с фейклагами, когда с чудищами в войсе трешкаешся.
		voice = true;

	return orig_send_net_msg( netchan, msg, reliable, voice );
}