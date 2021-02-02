#include "../../include_cheat.h"

bool is_grenade( const int id )
{
	return id == ClassId_CDecoyGrenade || id == ClassId_CHEGrenade || id == ClassId_CIncendiaryGrenade || id == ClassId_CMolotovGrenade || id == ClassId_CSmokeGrenade || id == ClassId_CFlashbang || id == ClassId_CDecoyProjectile || id == ClassId_CBaseCSGrenadeProjectile || id == ClassId_CMolotovProjectile || id == ClassId_CSmokeGrenadeProjectile || id == ClassId_CFlashbang;
}

bool is_weapon( const int id )
{
	return id >= ClassId_CWeaponAug && id <= ClassId_CWeaponXM1014 || id == ClassId_CAK47 || id == ClassId_CDEagle;
}

void CGlowObjectManager::RenderGlowEffects()
{
	const auto player_glow = [ & ]( const int idx )
	{
		const auto classid = m_glObjectDefinitions[ idx ].m_pEntity->GetClientClass()->m_ClassID;
		if ( classid != ClassId_CCSPlayer)
			return;

		auto player = reinterpret_cast< C_BasePlayer* >( m_glObjectDefinitions[ idx ].m_pEntity );

		if (player == g_pLocalPlayer)
			return;
		else
			if ( !cfg.visuals.player_esp.teammates && !player->is_enemy() )
				return;

		const Vector color(cfg.visuals.player_esp.glow_color[0], cfg.visuals.player_esp.glow_color[1], cfg.visuals.player_esp.glow_color[2] );

		m_glObjectDefinitions[ idx ].m_vGlowColor = color;
		m_glObjectDefinitions[ idx ].m_flGlowAlpha = cfg.visuals.player_esp.glow_color[3];
		m_glObjectDefinitions[ idx ].m_bRenderWhenOccluded = true;
		m_glObjectDefinitions[ idx ].m_bRenderWhenUnoccluded = false;
		m_glObjectDefinitions[ idx ].m_flBloomAmount = 0.8f;
	};

	const auto weapon_glow = [ & ]( const int idx )
	{
		const auto classid = m_glObjectDefinitions[ idx ].m_pEntity->GetClientClass()->m_ClassID;
		if ( !is_weapon( classid ) )
			return;

		if ( m_glObjectDefinitions[ idx ].m_pEntity->get_owner_ent() != -1 )
			return;

		if ( !vars::visuals.glow.world.weapons.get<bool>() )
			return;

		auto col = get_col( vars::visuals.glow.world.color.get<uintptr_t>() );
		const Vector color( col.rBase(), col.gBase(), col.bBase() );

		m_glObjectDefinitions[ idx ].m_vGlowColor = color;
		m_glObjectDefinitions[ idx ].m_flGlowAlpha = col.aBase();
		m_glObjectDefinitions[ idx ].m_bRenderWhenOccluded = true;
		m_glObjectDefinitions[ idx ].m_bRenderWhenUnoccluded = false;
		m_glObjectDefinitions[ idx ].m_flBloomAmount = 0.8f;
	};

	const auto grenade_glow = [ & ]( const int idx )
	{
		const auto classid = m_glObjectDefinitions[ idx ].m_pEntity->GetClientClass()->m_ClassID;
		if ( !is_grenade( classid ) )
			return;

		if ( !vars::visuals.glow.world.grenades.get<bool>() )
			return;

		auto col = get_col( vars::visuals.glow.world.color.get<uintptr_t>() );
		const Vector color( col.rBase(), col.gBase(), col.bBase() );

		m_glObjectDefinitions[ idx ].m_vGlowColor = color;
		m_glObjectDefinitions[ idx ].m_flGlowAlpha = col.aBase();
		m_glObjectDefinitions[ idx ].m_bRenderWhenOccluded = true;
		m_glObjectDefinitions[ idx ].m_bRenderWhenUnoccluded = false;
		m_glObjectDefinitions[ idx ].m_flBloomAmount = 0.8f;
	};

	const auto c4_glow = [ & ]( const int idx )
	{
		const auto classid = m_glObjectDefinitions[ idx ].m_pEntity->GetClientClass()->m_ClassID;
		if ( classid != ClassId_CC4 && classid != ClassId_CPlantedC4 )
			return;

		if ( !vars::visuals.glow.world.c4.get<bool>() )
			return;

		auto c4 = reinterpret_cast< C_PlantedC4* >( m_glObjectDefinitions[ idx ].m_pEntity );
		auto defusing = c4->get_defuser() != -1 || c4->get_defused();

		auto col = get_col( vars::visuals.glow.world.color.get<uintptr_t>() );
		if ( classid == ClassId_CPlantedC4 )col = Color::Red();
		if ( defusing )col = Color::LightBlue();
		const Vector color( col.rBase(), col.gBase(), col.bBase() );

		m_glObjectDefinitions[ idx ].m_vGlowColor = color;
		m_glObjectDefinitions[ idx ].m_flGlowAlpha = col.aBase();
		m_glObjectDefinitions[ idx ].m_bRenderWhenOccluded = true;
		m_glObjectDefinitions[ idx ].m_bRenderWhenUnoccluded = false;
		m_glObjectDefinitions[ idx ].m_flBloomAmount = 0.8f;
	};

	for ( int i = 0; i < m_glObjectDefinitions.Count(); i++ )
	{
		if ( m_glObjectDefinitions[ i ].IsUnused() || !m_glObjectDefinitions[ i ].m_pEntity )
			continue;

		player_glow( i );

		weapon_glow( i );

		grenade_glow( i );

		c4_glow( i );
	}
}