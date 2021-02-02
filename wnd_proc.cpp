#include "../include_cheat.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall  hooks::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KEYDOWN)
	{
		if (wParam == cfg.misc.settings.menu_key)
			Menu::get().m_bIsOpen = !Menu::get().m_bIsOpen;
	}

	if (Menu::get().m_bIsOpen)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		if (wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ImGui::GetIO().WantTextInput)
			return true;
	}

	return CallWindowProcA(orig_wnd_proc, hWnd, uMsg, wParam, lParam);
}