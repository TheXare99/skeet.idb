#include "../include_cheat.h"

long __stdcall hooks::reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp)
{
	render::get().invalidate_device_objects();

	if (g_unload)
	{
		return orig_reset(device, pp);
	}

	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto hr = orig_reset(device, pp);
	render::get().init_device_objects(device);
	ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}