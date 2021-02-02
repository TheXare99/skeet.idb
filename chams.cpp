#include "../include_cheat.h"
#include <fstream>
#include <cctype>

bool strstric( const std::string & strHaystack, const std::string & strNeedle )
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[]( char ch1, char ch2 ) { return std::toupper( ch1 ) == std::toupper( ch2 ); }
	);
	return ( it != strHaystack.end() );
}

void chams::draw_record(IMatRenderContext * context, const DrawModelState_t & state, const ModelRenderInfo_t & info, matrix3x4_t * custom_bone_to_world)
{
	if (!info.pModel || !g_pLocalPlayer)
		return hooks::orig_draw_model_execute(g_pModelRender, context, state, info, custom_bone_to_world);

	const auto name = g_pModelInfo->GetModelName(info.pModel);

	_(sleeve, "sleeve");
	if (strstr(name, sleeve))
		return;

	const auto player = get_entity(info.entity_index);
	if (!player || player->GetClientClass()->m_ClassID != ClassId_CCSPlayer)
		return hooks::orig_draw_model_execute(g_pModelRender, context, state, info, custom_bone_to_world);

	if (player == g_pLocalPlayer && !g_in_lp_draw)
		return;

	if (player == g_pLocalPlayer)
	{
		g_pModelRender->ForcedMaterialOverride();
		float a[3] = { 1.f,1.f,1.f };
		g_pRenderView->SetColorModulation(a);
	}

	if (vars::misc.transparency_in_scope.get<float>() && player == g_pLocalPlayer && g_pLocalPlayer->get_scoped())
		g_pRenderView->SetBlend(1.f - vars::misc.transparency_in_scope.get<float>() * 0.01f);

	if (!cfg.visuals.colored_models.player)
		return hooks::orig_draw_model_execute(g_pModelRender, context, state, info, custom_bone_to_world);

	if (strlen(name) > 19 && name[13] == '/' && name[7] == 'p' &&  name[0] == 'm')
	{
		auto & log = player_log::get().get_log(player->EntIndex());
		auto& records = log.record;

		if (log.player && log.player->is_enemy() && vars::visuals.chams.history.enabled.get<bool>() && records.size() > 1 && records.size() < 105 && log.lastvalid_vis > 0 && log.lastvalid_vis < records.size())
		{
			matrix3x4_t matrix[MAX_SETUP_BONES];
			if (draw_hst(records[log.lastvalid], matrix))
			{
				hooks::orig_draw_model_execute(g_pModelRender, context, state, info, matrix);
				g_pModelRender->ForcedMaterialOverride();
			}
		}

		if (!draw_ch(context, state, info, custom_bone_to_world))
			return hooks::orig_draw_model_execute(g_pModelRender, context, state, info, custom_bone_to_world);
	}

	return hooks::orig_draw_model_execute(g_pModelRender, context, state, info, custom_bone_to_world);
}

bool chams::draw_hst( lag_record_t& record, matrix3x4_t* out )
{
	const auto player = get_entity( record.index );

	if ( !player )
		return false;

	auto & log = player_log::get().get_log( record.index );

	if ( log.lastvalid_vis + 1 >= log.record.size() )
		return false;

	const auto itp1 = &log.record[ log.lastvalid_vis ];
	const auto it = &log.record[ log.lastvalid_vis + 1 ];

	if ( it->m_dormant )
		return false;

	if ( ( itp1 )->m_sim_time - it->m_sim_time > 0.5f )
		return false;

	const auto nci = g_pEngine->GetNetChannelInfo();

	bool end = log.lastvalid_vis == 0;
	Vector next = end ? player->get_origin() : ( itp1 )->m_origin;
	float  time_next = end ? player->get_simtime() : ( itp1 )->m_sim_time;

	const float correct = nci->GetLatency( FLOW_OUTGOING ) + nci->GetLatency( FLOW_INCOMING ) + lagcomp::get_lerp_time() - ( it->m_sim_time - ( itp1 )->m_sim_time );
	float time_delta = time_next - it->m_sim_time;
	float add = end ? 0.2f : time_delta;
	float deadtime = it->m_sim_time + correct + add;

	float curtime = g_pGlobals->curtime;
	float delta = deadtime - curtime;

	float mul = 1.f / add;

	Vector lerp = math::lerp( next, it->m_origin, clamp( delta * mul, 0.f, 1.f ) );

	if ( ( math::matrix_get_origin( it->matrix[ 1 ] ) - it->m_origin + lerp ).Dist( player->get_abs_origin() ) < 7.5f )
		return false;

	matrix3x4_t ret[MAX_SETUP_BONES];
	memcpy( ret, it->matrix, sizeof( matrix3x4_t[MAX_SETUP_BONES] ) );

	for ( size_t i{}; i < MAX_SETUP_BONES; ++i )
	{
		const Vector matrix_delta = math::matrix_get_origin( it->matrix[ i ] ) - it->m_origin;
		math::matrix_set_origin( matrix_delta + lerp, ret[ i ] );
	}

	memcpy( out, ret, sizeof( matrix3x4_t[MAX_SETUP_BONES] ) );
	_u(tex_group, TEXTURE_GROUP_MODEL);
	IMaterial* material = g_pMaterialSystem->FindMaterial("debug/debugdrawflat", tex_group);
	material->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);
	material->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
	auto col = Color( vars::visuals.chams.history.color.get<D3DCOLOR>() );
	g_pRenderView->SetColorModulation( col.Base() );
	g_pRenderView->SetBlend( col.aBase() );
	g_pModelRender->ForcedMaterialOverride( material );

	return true;
}

bool chams::draw_ch( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld )
{
	auto player = get_entity( info.entity_index );

	if (player == g_pLocalPlayer)
		return false;

	if (!player->is_enemy())
		return false;

	if (!cfg.visuals.colored_models.player)
		return false;

	_u(tex_group, TEXTURE_GROUP_MODEL);
	IMaterial* material = g_pMaterialSystem->FindMaterial("debug/debugambientcube", tex_group);
	_(envmaptint, "$envmaptint");
	bool is_specific_material = false;

	if (cfg.visuals.colored_models.player_behind_wall)
	{
		if (player->is_enemy())
		{
			switch (cfg.visuals.colored_models.player_type)
			{
			case 0:
				is_specific_material = false;
				material = g_pMaterialSystem->FindMaterial("debug/debugambientcube", tex_group);
				break;
			case 1:
				is_specific_material = false;
				material = g_pMaterialSystem->FindMaterial("debug/debugdrawflat", tex_group);
				break;
			}
		}

		material->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, true);
		material->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, true);
		material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		
		if (is_specific_material)
		{
			auto var = material->FindVar(envmaptint, nullptr);
			var->set_vec_val(cfg.visuals.colored_models.player_behind_wall_color[0], cfg.visuals.colored_models.player_behind_wall_color[1], cfg.visuals.colored_models.player_behind_wall_color[2]);
		}
		else
			g_pRenderView->SetColorModulation(cfg.visuals.colored_models.player_behind_wall_color);
		g_pRenderView->SetBlend(cfg.visuals.colored_models.player_behind_wall_color[3]);
		g_pModelRender->ForcedMaterialOverride( material );
		hooks::orig_draw_model_execute( g_pModelRender, context, state, info, pCustomBoneToWorld );
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_ZNEARER, false);
	material->SetMaterialVarFlag(MATERIAL_VAR_NOFOG, false);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
	
	switch (cfg.visuals.colored_models.player_type)
	{
	case 0:
		is_specific_material = false;
		material = g_pMaterialSystem->FindMaterial("debug/debugambientcube", tex_group);
		break;
	case 1:
		is_specific_material = false;
		material = g_pMaterialSystem->FindMaterial("debug/debugdrawflat", tex_group);
		break;
	}

	if (is_specific_material)
	{
		auto var = material->FindVar(envmaptint, nullptr);
		var->set_vec_val(cfg.visuals.colored_models.player_color[0], cfg.visuals.colored_models.player_color[1], cfg.visuals.colored_models.player_color[2]);
	}
	else
		g_pRenderView->SetColorModulation(cfg.visuals.colored_models.player_color);
	g_pRenderView->SetBlend(cfg.visuals.colored_models.player_color[3]);
	g_pModelRender->ForcedMaterialOverride(material);

	hooks::orig_draw_model_execute( g_pModelRender, context, state, info, pCustomBoneToWorld );
	return true;
}

void chams::dark_mode() const
{
	const auto reset = [ & ]()
	{
		_( r_DrawSpecificStaticProp, "r_DrawSpecificStaticProp" );
		g_pCVar->FindVar( r_DrawSpecificStaticProp )->set_value( 1 );

		_( World, "World" );
		_( SkyBox, "SkyBox" );
		_( StaticProp, "StaticProp" );

		for ( auto i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial( i ) )
		{
			auto mat = g_pMaterialSystem->GetMaterial( i );
			if ( !mat )
				continue;

			if ( mat->IsErrorMaterial() )
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();


			if ( strstr( tex_name, World ) || strstr( tex_name, StaticProp ) || strstr( tex_name, SkyBox ) )
			{
				mat->ColorModulate( 1.f, 1.f, 1.f );
				mat->AlphaModulate( 1.f );
			}
		}
	};

	_( sky_csgo_night02, "sky_csgo_night02" );
	const auto set = [ & ]()
	{
		static auto load_named_sky = reinterpret_cast< void( __fastcall* )( const char* ) >( sig( "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) );
		load_named_sky( sky_csgo_night02 );

		_( r_DrawSpecificStaticProp, "r_DrawSpecificStaticProp" );
		g_pCVar->FindVar( r_DrawSpecificStaticProp )->set_value( 0 );

		_( World, "World" );
		_( SkyBox, "SkyBox" );
		_( StaticProp, "StaticProp" );
		_( wall, "wall" );
		_(wood, "wood");

		for ( auto i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial( i ) )
		{
			auto mat = g_pMaterialSystem->GetMaterial( i );
			if ( !mat )
				continue;

			if ( mat->IsErrorMaterial() )
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();

			if (( cfg.visuals.effects.brightness_adjustment) && strstr( tex_name, World ) )
			{
				mat->ColorModulate( 0.18f, 0.18f, 0.18f );
			}
			if ( strstr( tex_name, StaticProp ) )
			{
				if (cfg.visuals.effects.brightness_adjustment)
					mat->ColorModulate( 0.28f, 0.28f, 0.28f );
				//if ( !strstric( name, wood) && !strstric( name, wall ) )
				//	mat->AlphaModulate( 1.f - (cfg.visuals.prop_transparency) * 0.01f );
			}
		}
	};

	static auto done = true;
	static auto last_setting = 0;
	static auto last_transparency = 0.f;
	static auto was_ingame = false;
	static auto unloaded = false;

	if ( !done )
	{
		if ( last_setting || last_transparency )
		{
			reset();
			set();
			done = true;
		}
		else
		{
			reset();
			done = true;
		}
	}

	if ( unloaded )
		return;

	if ( g_unload || was_ingame != g_pEngine->IsInGame() || last_setting != (cfg.visuals.effects.brightness_adjustment) || last_transparency != vars::visuals.prop_transparency.get<float>() )
	{
		last_setting = (cfg.visuals.effects.brightness_adjustment);
		last_transparency = vars::visuals.prop_transparency.get<float>();
		was_ingame = g_pEngine->IsInGame();
		unloaded = g_unload;

		if ( g_unload )
			last_setting = false;

		done = false;
	}
}