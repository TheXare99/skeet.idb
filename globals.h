#pragma once
#include <d3dx9.h>
#include <map>

struct ImFont;
struct Item_t;
struct WeaponName_t;
class C_CSGameRulesProxy;
struct VMatrix;
class C_locPlayer;
class C_CSPlayer;
class C_BaseEntity;
class CUserCmd;
class C_BaseCombatWeapon;
class Vector;
class QAngle;
class ITraceFilter;
class CGameTrace;
class Color;
class C_AnimationLayer;
struct matrix3x4_t;
struct weaponconfig_t;
struct antiaimconfig_t;
typedef CGameTrace trace_t;
using CBaseHandle = unsigned long;

namespace menu_globals
{
	extern ImFont* tabFont;
	extern ImFont* menuFont;
	extern ImFont* boldMenuFont;
	extern ImFont* weaponFont;
	extern ImFont* keybindsFont;
	extern IDirect3DTexture9* menuBg;
	extern ImFont* controlFont;
}

extern HMODULE g_module;
extern bool g_erase;

extern C_locPlayer g_pLocalPlayer;
extern C_CSPlayer* get_entity(int index);
extern C_BaseCombatWeapon* get_weapon(const CBaseHandle* handle);
extern weaponconfig_t* get_config(C_BaseCombatWeapon* weapon);
extern antiaimconfig_t* get_antiaim(const int type);

extern QAngle g_original_viewangles;
extern bool g_send_packet;
extern CUserCmd* g_cmd;

extern void util_trace_line(const Vector& vec_start, const Vector& vec_end, unsigned int n_mask, C_BaseEntity* p_cs_ignore, trace_t* p_trace);
extern void util_trace_hull(const Vector & vec_start, const Vector & vec_end, unsigned int n_mask, const Vector & extents, trace_t * p_trace);
extern void util_clip_trace_to_players(const Vector & vec_abs_start, const Vector & vec_abs_end, unsigned int mask, ITraceFilter * filter, trace_t * tr);

extern bool g_should_animate;
extern int g_max_lag;
extern bool g_resolver_override;
extern bool g_force_body_aim;
extern bool g_thirdperson_active;
extern bool g_inverted_desync;
extern int g_tickbase_shift;
extern bool g_subscription_check;
extern bool g_dllname_check;
extern bool g_csgobuild_check;
extern bool g_is_loader_run_check;
extern bool g_subscription_invalid;
extern bool g_dllname_invalid;
extern bool g_csgobuild_invalid;
extern bool g_loader_not_run;
extern bool g_stop;
extern bool g_slowwalk;
extern bool g_unload;
extern bool g_in_hbp;
extern bool g_in_gl;
extern bool g_in_lp_draw;

extern VMatrix g_vmatrix;

extern C_CSGameRulesProxy** g_ppGameRules;

HRESULT CreateVertexBuffer(
	UINT Length,
	DWORD Usage,
	DWORD FVF,
	D3DPOOL Pool,
	IDirect3DVertexBuffer9** ppVertexBuffer,
	HANDLE* pHandle
);

HRESULT Lock(
	UINT OffsetToLock,
	UINT SizeToLock,
	VOID** ppbData,
	DWORD Flags
);


extern LPDIRECT3DVERTEXBUFFER9 g_pVB;

extern bool* g_disable_post_processing;

extern float(*random_float) (float from, float to);
extern void(*random_seed) (unsigned int seed);

template <typename T>
auto get_col(const T val)
{
	Color ret;
	ret[0] = (((val) & 0x00ff0000) >> 16);
	ret[1] = (((val) & 0x0000ff00) >> 8);
	ret[2] = ((val) & 0x000000ff);
	ret[3] = (((val) & 0xff000000) >> 24);

	return ret;
}