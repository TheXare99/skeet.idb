#pragma once

namespace hooks
{
	extern c_hook* client;
	extern c_hook* clientmode;
	extern c_hook* engine;
	extern c_hook* predict;
	extern c_hook* modelrender;
	extern c_hook* viewrender;
	extern c_hook* renderview;
	extern c_hook* vpanel;
	extern c_hook* device;
	extern c_hook* engine_sound;
	extern c_hook* surface;
	extern c_bphook* clientstateplus8;
	extern c_bphook* ccsplayer;
	extern c_bphook* ccsplayerrenderable;
	extern c_hook* net_channel;
	extern c_hook* sv_cheats;
	extern c_hook* net_showfragments;
	extern recv_prop_hook* smoke_prop;

	using get_bool_fn = bool( __thiscall * )( void* );
	extern get_bool_fn orig_sv_cheats_get_bool;
	bool __fastcall sv_cheats_get_bool( void*, void* );

	extern WNDPROC orig_wnd_proc;
	LRESULT __stdcall wnd_proc( HWND, UINT, WPARAM, LPARAM );

	using create_move_fn = bool( __stdcall* )( float, CUserCmd* );
	extern create_move_fn orig_create_move;
	bool __stdcall create_move( float, CUserCmd* );

	using frame_stage_notify_fn = void( __stdcall* )( ClientFrameStage_t );
	extern frame_stage_notify_fn orig_frame_stage_notify;
	void __stdcall frame_stage_notify( ClientFrameStage_t );

	using draw_model_execute_fn = void( __thiscall* )( IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
	extern draw_model_execute_fn orig_draw_model_execute;
	void __stdcall draw_model_execute( IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );

	using render_view_fn = void( __thiscall* )( void*, CViewSetup&, CViewSetup&, unsigned int, int );
	extern render_view_fn orig_render_view;
	void __fastcall render_view( void*, void*, CViewSetup&, CViewSetup&, unsigned int, int );

	using scene_end_fn = void( __thiscall* )( void* );
	extern scene_end_fn orig_scene_end;
	void __fastcall scene_end( void*, void* );

	using do_post_screen_space_effects_fn = bool( __thiscall* )( void*, CViewSetup* );
	extern do_post_screen_space_effects_fn orig_do_post_screen_space_effects;
	bool _fastcall do_post_screen_space_effects( void*, void*, CViewSetup* );

	using should_skip_animframe_fn = bool(__thiscall*)();
	extern should_skip_animframe_fn orig_should_skip_animframe;
	bool _fastcall should_skip_animframe();

	using present_fn = long( __stdcall* )( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
	extern present_fn orig_present;
	long __stdcall present( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );

	using reset_fn = long( __stdcall* )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
	extern reset_fn orig_reset;
	long __stdcall reset( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );

	using send_net_msg_fn = int( __thiscall* )( void*, INetMessage&, bool, bool );
	extern send_net_msg_fn orig_send_net_msg;
	bool  __fastcall send_net_msg( void* netchan, void * edx, INetMessage & msg, bool reliable, bool voice );

	using is_hltv_fn = bool( __thiscall* )( void* );
	extern is_hltv_fn orig_is_hltv;
	bool __fastcall is_hltv( void * ecx, void * edx );

	using override_view_fn = void( __stdcall* )( CViewSetup* );
	extern override_view_fn orig_override_view;
	void __stdcall override_view( CViewSetup* setup );

	using paint_traverse_fn = void( __thiscall* )( void*, unsigned int, bool, bool );
	extern paint_traverse_fn orig_paint_traverse;
	void __fastcall paint_traverse( void* pPanels, int edx, unsigned int panel, bool forceRepaint, bool allowForce );


	using emit_sound_fn = void( __thiscall* )( void*, void*, int, int, const char *, unsigned int, const char *, float, float, int, int, int, const void *, const void *, void *, bool, float, int, int );
	extern emit_sound_fn orig_emit_sound;
	void __fastcall emit_sound( void* pThis, int edx, void* filter, int iEntIndex, int iChannel,
								const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
								float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
								const void *pOrigin, const void *pDirection, void * pUtlVecOrigins,
								bool bUpdatePositions, float soundtime, int speakerentity, int something );

	using temp_entities_fn = bool( __thiscall* )( void*, void* );
	extern temp_entities_fn orig_temp_entities;
	bool __fastcall temp_entities( void* ECX, void* EDX, void* msg );

	using do_extra_bone_processing_fn = void( __thiscall* )( void*, CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & matrix, byte * boneComputed, CIKContext * context );
	extern do_extra_bone_processing_fn orig_do_extra_bone_processing;
	void __fastcall do_extra_bone_processing( void * ecx, void * edx, CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & matrix, byte * boneComputed, CIKContext * context );

	using run_command_fn = void(__fastcall*)(void*, void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);
	extern run_command_fn orig_run_command;
	void __fastcall run_command(void*, void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);

	using write_user_cmd_delta_to_buffer_fn = bool(__fastcall*)(void*, void*, int, bf_write*, int, int, bool);
	extern write_user_cmd_delta_to_buffer_fn orig_write_user_cmd_delta_to_buffer;
	bool __fastcall write_user_cmd_delta_to_buffer(void*, void*, int, bf_write*, int, int, bool);

	using play_sound_fn = void( __thiscall* )( void*, const char* );
	extern play_sound_fn orig_play_sound;
	void __stdcall play_sound( const char *folderIme );

	using is_connected_fn = bool( __thiscall* )( void* );
	extern is_connected_fn orig_is_connected;
	bool __fastcall is_connected( void* ecx, void* edx );

	using eye_angles_fn = QAngle * ( __thiscall* )( void* );
	extern eye_angles_fn orig_eye_angles;
	QAngle* __fastcall eye_angles( void* ecx, void* edx );

	using standard_blending_rules_fn = void * ( __thiscall* )( void*, CStudioHdr*, Vector*, Quaternion*, float, int );
	extern standard_blending_rules_fn orig_standard_blending_rules;
	void __fastcall standard_blending_rules( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion *q, float curTime, int boneMask );

	using update_client_side_animation_fn = void (__thiscall*)(void* );
	extern update_client_side_animation_fn orig_update_client_side_animation;
	void __fastcall update_client_side_animation(void* ecx, void* edx);

	using build_transformations_fn = void(__thiscall*)(void*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, const int32_t, byte*);
	extern build_transformations_fn orig_build_transformations;
	void __fastcall build_transformations(void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& camera_transform, const int32_t bone_mask, byte* bone_computed);

	using fire_event_fn = void( __thiscall* )( void* );
	extern fire_event_fn orig_fire_event;
	void __fastcall fire_event( void* ecx, void* edx );

	using cursor_fn = void( __thiscall * )( void* );
	extern cursor_fn orig_lock_cursor;
	void __fastcall lock_cursor( void* ecx, void* );

	//netprops
	extern RecvVarProxyFn orig_smoke_effect;
	void __cdecl smoke_effect(CRecvProxyData* proxy_data_const, void* entity, void* output);
}