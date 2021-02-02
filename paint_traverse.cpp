#include "../include_cheat.h"
#include <Tlhelp32.h>
bool IsLoaderRun() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W);
	Process32FirstW(hSnapshot, &pe);

	while (TRUE) {
		if (wcscmp(pe.szExeFile, L"skeet.idb.exe") == 0) return true;
		if (!Process32NextW(hSnapshot, &pe)) return false;
	}
}
void __fastcall hooks::paint_traverse( void* pPanels, int edx, unsigned int panel, bool forceRepaint, bool allowForce ) {
	if ( g_unload )
		return orig_paint_traverse( pPanels, panel, forceRepaint, allowForce );

	static unsigned int scope_panel;
	if ( !scope_panel ) {
		const auto panelname = g_pPanel->GetName( panel );

		_( HudZoom, "HudZoom" );
			if ( !strcmp( panelname, HudZoom ) )
			scope_panel = panel;
	}

	if ( panel == scope_panel && cfg.visuals.effects.remove_scope_overlay)
		return;

	g_vmatrix = g_pEngine->WorldToScreenMatrix();

	orig_paint_traverse( pPanels, panel, forceRepaint, allowForce );

	static unsigned int draw_panel;
	if ( !draw_panel ) {
		const auto panelname = g_pPanel->GetName( panel );

		_( MatSystemTopPanel, "MatSystemTopPanel" );
		if ( !strcmp( panelname, MatSystemTopPanel ) )
			draw_panel = panel;
	}

	if ( panel != draw_panel )
		return;

	// ������ �������� �� ����� �����, � ������� ���������� ��� (��� ���, ������� ��������� ����� ����� ������).
	if (g_csgobuild_check) {
		_(proper_csgobuild_s, "13763");
		/*if (g_pEngine->GetEngineBuildNumber() != std::stoi(proper_csgobuild_s)) {
			g_csgobuild_invalid = true;
			g_csgobuild_check = false;
		}
		else*/
			g_csgobuild_check = false;
	}

	// ������ ������� �� ������� ������� ��� ������� (����������� ����, �� ��������, ��� ���� ����� ���� �� �������� ��� ����� ����� ����).
	if (g_is_loader_run_check) {
		/*if (!IsLoaderRun() {
			g_loader_not_run = true;
			g_is_loader_run_check = false;
		}
		else*/
			g_is_loader_run_check = false;
	}

	// ���� ���-�� �� ��� ��� ���� - ���� �����.
	if (g_subscription_check || g_dllname_check || g_csgobuild_check || g_is_loader_run_check || g_subscription_invalid || g_loader_not_run)
		misc::get().guard_detect();

	visuals::get().run();
}