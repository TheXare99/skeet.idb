#pragma once

class Menu : public singleton<Menu> {

public:
	void Render();
	void Shutdown();
	void Init(IDirect3DDevice9 * pDevice);
	void Begin();
	void End();
	void ColorPicker(const char* name, float* color, bool alpha);

	void Ragebot();
	void Anti_aimbot();
	void Legitbot();
	void Visuals();
	void Misc();
	void Skins();
	void Players();
	int m_nActiveTab = 4;
	int m_nActiveLegitTab = 0;
	bool m_bInitialized = false;
	bool m_bIsOpen = false;
};