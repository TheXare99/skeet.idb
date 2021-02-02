#include "include_cheat.h"

namespace menu_globals
{
	ImFont* tabFont;
	ImFont* menuFont;
	ImFont* boldMenuFont;
	ImFont* weaponFont;
	ImFont* keybindsFont;
	IDirect3DTexture9* menuBg;
	ImFont* controlFont;
}


HMODULE g_module;
bool g_erase = false;
C_locPlayer g_pLocalPlayer;
C_CSPlayer* get_entity(const int index) { return reinterpret_cast<C_CSPlayer*>(g_pEntitiyList->GetClientEntity(index)); }
C_BaseCombatWeapon* get_weapon(const CBaseHandle * handle) { if (!handle) { return nullptr; } return reinterpret_cast<C_BaseCombatWeapon*>(g_pEntitiyList->GetClientEntityFromHandle(*handle)); }

int get_config_index(int weaponindex)
{
	switch (weaponindex)
	{
	case WEAPON_G3SG1:
	case WEAPON_SCAR20:
		return 0;
	case WEAPON_SSG08:
		return 1;
	case WEAPON_AWP:
		return 2;
	case WEAPON_DEAGLE:
	case WEAPON_REVOLVER:
		return 3;
	case WEAPON_CZ75A:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_HKP2000:
	case WEAPON_P250:
	case WEAPON_USP_SILENCER:
	case WEAPON_TEC9:
		return 4;
	default:
		return 5;
	}
}

weaponconfig_t* get_config(C_BaseCombatWeapon* weapon)
{
	return &config::get().weapon_config[get_config_index(weapon->get_weapon_id())];
}

antiaimconfig_t* get_antiaim(const int type)
{
	return &config::get().antiaim_config[type];
}

CUserCmd* g_cmd = nullptr;
QAngle g_original_viewangles;
bool g_send_packet = true;
bool g_should_animate = false;
int g_max_lag = 14;
bool g_resolver_override = false;
bool g_force_body_aim = false;
bool g_thirdperson_active = false;
bool g_inverted_desync = false;
int g_tickbase_shift = 0;
bool g_subscription_check = true;
bool g_dllname_check = true;
bool g_csgobuild_check = true;
bool g_is_loader_run_check = true;
bool g_subscription_invalid = false;
bool g_dllname_invalid = false;
bool g_csgobuild_invalid = false;
bool g_loader_not_run = false;
bool g_stop = false;
bool g_slowwalk = false;
bool g_unload = false;
bool g_in_hbp = false;
bool g_in_gl = false;
bool g_in_lp_draw = false;

C_CSGameRulesProxy** g_ppGameRules = nullptr;
VMatrix g_vmatrix;

msg_queue<std::pair<int, LPDIRECT3DVERTEXBUFFER9 >> g_vertex_buffer;
LPDIRECT3DVERTEXBUFFER9 g_pVB;

bool* g_disable_post_processing = nullptr;

float(*random_float) (float from, float to);
void(*random_seed) (unsigned int seed);

void util_trace_line(const Vector & vec_start, const Vector & vec_end, const unsigned int n_mask, C_BaseEntity * p_cs_ignore, trace_t * p_trace)
{
	Ray_t ray;
	ray.Init(vec_start, vec_end);

	if (p_cs_ignore)
	{
		CTraceFilter filter;
		filter.pSkip = p_cs_ignore;
		g_pTrace->TraceRay(ray, n_mask, &filter, p_trace);
		return;
	}

	CTraceFilterWorldOnly filter;
	g_pTrace->TraceRay(ray, n_mask, &filter, p_trace);
}

void util_trace_hull(const Vector & vec_start, const Vector & vec_end, const unsigned int n_mask, const Vector & extents, trace_t * p_trace)
{
	CTraceFilterNoPlayers filter;

	Ray_t ray;
	ray.Init(vec_start, vec_end);
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_pTrace->TraceRay(ray, n_mask, &filter, p_trace);
}

void util_clip_trace_to_players(const Vector & vec_abs_start, const Vector & vec_abs_end, unsigned int mask, ITraceFilter * filter, trace_t * tr)
{
	static auto dw_address = offsets::clip_trace_to_players;

	if (!dw_address)
		return;

	_asm
	{
		mov		eax, filter
		lea		ecx, tr
		push	ecx
		push	eax
		push	mask
		lea		edx, vec_abs_end
		lea		ecx, vec_abs_start
		call	dw_address
		add		esp, 0x10
	}
}