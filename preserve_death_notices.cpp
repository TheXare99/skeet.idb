#include "../include_cheat.h"

template<class T>
static T* find_hud_element(const char* name)
{
	static auto p_hud = *reinterpret_cast<DWORD**>(offsets::hud);
	static auto p_find_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(offsets::find_element);
	return reinterpret_cast<T*>(p_find_element(p_hud, name));
}

void preserve_death_notices::run(const ClientFrameStage_t stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	if (!g_pLocalPlayer)
		return;

	_(CCSGO_HudDeathNotice_s, "CCSGO_HudDeathNotice");
	static DWORD* death_notice = find_hud_element<DWORD>(CCSGO_HudDeathNotice_s);
	static void(__thiscall * clear_death_notices)(DWORD) = (void(__thiscall*)(DWORD))(offsets::clear_death_notices);
	static bool preservedn = vars::misc.preservedn.get<bool>();

	if (death_notice)
	{
		*(float*)((DWORD)death_notice + 0x50) = vars::misc.preservedn.get<bool>() ? FLT_MAX : 1.f;
		if ((*g_ppGameRules)->m_bFreezePeriod() || preservedn != vars::misc.preservedn.get<bool>())
		{
			preservedn = vars::misc.preservedn.get<bool>();
			if (clear_death_notices)
			{
				clear_death_notices(((DWORD)death_notice - 0x14));
			}
		}
	}
} // тут почему-то краш при смене мапы, и выборе тимы, мб я еблан хз.
