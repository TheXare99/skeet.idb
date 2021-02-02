#pragma once

class ClientClass;
class IBaseClientDll
{
public:
	VFUNC( 8, GetAllClasses(), ClientClass*( __thiscall* )( void* ) )( )
	VFUNC( 20, IN_KeyEvent( int eventcode, int keynum, const char *pszCurrentBinding ), void( __thiscall* )( void*, int, int, const char* ) )( eventcode, keynum, pszCurrentBinding )
	VFUNC( 21, CreateMove( int sequence_number, float frametime, bool active ), void( __thiscall* )( void*, int, float, bool ) )( sequence_number, frametime, active )
	VFUNC(38, DispatchUserMessage(int messageType, int arg, int arg1, void* data), bool( __thiscall* )( void*, int, int, int, void*) )(messageType, arg, arg1, data)
};

extern IBaseClientDll* g_pClient;