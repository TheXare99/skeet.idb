#include "../include_cheat.h"

std::string hitgroup_to_name( const int hitgroup )
{
	switch ( hitgroup )
	{
		case HITGROUP_HEAD:
			return _w( "head" );
		case HITGROUP_CHEST:
			return _w("chest");
		case HITGROUP_STOMACH:
			return _w("stomach");
		case HITGROUP_LEFTARM:
			return _w("left arm");
		case HITGROUP_RIGHTARM:
			return _w("right arm");
		case HITGROUP_LEFTLEG:
			return _w("left leg");
		case HITGROUP_RIGHTLEG:
			return _w( "right leg" );
		default:
			return _w( "body" );;
	}
}

void console_log::buy_event( IGameEvent * game_event )
{
	_( team, "team" );
	_( userid, "userid" );
	_( weapon_s, "weapon" );
	_( weapon_, "weapon_" );
	_( item_, "item_" );
	_(skeet_idb, "[skeet.idb] ");
	_( first, "%s bought " );
	_( second, "%s\n" );

	if ( !g_pLocalPlayer || g_pLocalPlayer->get_team() == game_event->GetInt( team ) )
		return;

	const auto index = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid ) );

	player_info_t pinfo;
	g_pEngine->GetPlayerInfo( index, &pinfo );

	std::string weapon( game_event->GetString( weapon_s ) );

	g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
	util::print_dev_console( true, Color(255, 255, 255, 255), first, pinfo.name );
	util::print_dev_console( true, Color( 255, 255, 255, 255 ), second, weapon.c_str() );
}

void console_log::hurt_event( IGameEvent * game_event )
{
	_( attacker_s, "attacker" );
	_( userid, "userid" );

	const auto attacker = g_pEngine->GetPlayerForUserID( game_event->GetInt( attacker_s ) );
	const auto victim = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid ) );
	if ( attacker != g_pEngine->GetLocalPlayer() || victim == g_pEngine->GetLocalPlayer() )
		return;

	_( dmg_health, "dmg_health" );
	_(health, "health");

	player_info_t pinfo;
	g_pEngine->GetPlayerInfo( victim, &pinfo );

	_(skeet_idb, "[skeet.idb] ");
	_(first, "Hit %s in ");
	_(hitgroup_s, "hitgroup");
	_u(hitgroup, hitgroup_to_name(game_event->GetInt(hitgroup_s)));
	_(second, "%s ");
	_(third, "for %d ");
	_(fourth, "damage (%d health remaining)\n");

	g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
	util::print_dev_console(true, Color(255, 255, 255, 255), first, pinfo.name);
	util::print_dev_console(true, Color(255, 255, 255, 255), second, hitgroup);
	util::print_dev_console(true, Color(255, 255, 255, 255), third, game_event->GetInt(dmg_health));
	util::print_dev_console(true, Color(255, 255, 255, 255), fourth, game_event->GetInt(health));
}

void console_log::setup()
{
	if ( m_setup )
		return;

	_( developer_s, "developer" );
	_( con_filter_text_s, "con_filter_text" );
	_( con_filter_text_out_s, "con_filter_text_out" );
	_( con_filter_enable_s, "con_filter_enable" );
	_( contimes_s, "contimes" );

	static auto developer = g_pCVar->FindVar( developer_s );
	developer->set_value( 1 );
	static auto con_filter_text = g_pCVar->FindVar( con_filter_text_s );
	static auto con_filter_text_out = g_pCVar->FindVar( con_filter_text_out_s );
	static auto con_filter_enable = g_pCVar->FindVar( con_filter_enable_s );
	static auto contimes = g_pCVar->FindVar( contimes_s );

	_(skeet_idb, "[skeet.idb] " );
	_( s, " " );
	_( clear, "clear" );

	contimes->set_value( 15 );
	con_filter_text->set_value(skeet_idb);
	con_filter_text_out->set_value( s );
	con_filter_enable->set_value( 2 );
	g_pEngine->ClientCmd_Unrestricted( clear );

	m_setup = true;
}

void console_log::listener( IGameEvent * game_event )
{
	_( player_hurt, "player_hurt" );
	_( item_purchase, "item_purchase" );

	if (( cfg.misc.miscellaneous.log_damage_dealt) && !strcmp( game_event->GetName(), player_hurt ) )
		hurt_event( game_event );
	else if (( cfg.misc.miscellaneous.log_weapon_puchases) && !strcmp( game_event->GetName(), item_purchase ) )
		buy_event( game_event );
}
