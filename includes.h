#pragma once

#include <Windows.h>
#include <WinInet.h>
#include <minwindef.h>
#include <string>
#include <vector>
#include <array>
#include <deque>
#include <cmath>
#include <intrin.h>
#include <atlbase.h>

#undef max
template<class T> inline const T& max( const T& a, const T& b )
{
	return b < a ? a : b;
}

#undef min
template<class T> inline const T& min( const T& a, const T& b )
{
	return b > a ? a : b;
}

#include <d3d9.h>
#include <d3dx9.h>
#include "threadsafe_deque.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ntdll.lib")

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
	PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

#pragma warning( disable : 4244 )
#pragma warning( disable : 4838 )
#pragma warning( disable : 4018 )