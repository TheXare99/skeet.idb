#include "../include_cheat.h"

void hitmarker::listener( IGameEvent * game_event )
{
	if ( !cfg.visuals.player_esp.hit_marker_sound)
		return;

	_( attacker_s, "attacker" );
	_( userid_s, "userid" );

	const auto attacker = g_pEngine->GetPlayerForUserID( game_event->GetInt( attacker_s ) );
	const auto victim = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid_s ) );

	if ( attacker != g_pEngine->GetLocalPlayer() )
		return;

	if ( victim == g_pEngine->GetLocalPlayer() )
		return;

	const auto player = get_entity( victim );
	if ( !player || !player->is_enemy() )
		return;

	_u(name, _w("buttons/arena_switch_press_02.wav"));
	g_pSurface->PlaySound(name);
}

void hitmarker::draw_hits()
{
	if (!cfg.visuals.player_esp.hit_marker)
		return;

	for (size_t i = 0; i < hits.size(); i++ )
	{
		auto& hit = hits[ i ];

		if ( hit.time + 1.5f < g_pGlobals->curtime )
		{
			hits.erase( hits.begin() + i );
			i--;
		}

		Vector screen_pos;
		if ( math::get().world_to_screen( hit.pos, screen_pos ) )
		{
			render_hitmarker( hit, screen_pos );
		}
	}
}

void hitmarker::add_hit( const hitmarker_t hit )
{
	hits.push_back( hit );
}

void hitmarker::render_hitmarker( hitmarker_t& hit, const Vector& screen_pos )
{
	static auto line_size = 6;

	const auto step = 255.f / 1.0f * g_pGlobals->frametime;
	const auto step_move = 30.f / 1.5f * g_pGlobals->frametime;
	const auto multiplicator = 0.3f;

	auto damage = hit.damage;
	damage = clamp(damage, 0, 100);
	const int green = 255 - damage * 2.55;
	const int red = damage * 2.55;

	hit.moved -= step_move;

	if ( hit.time + 0.5f <= g_pGlobals->curtime )
		hit.alpha -= step;

	const auto int_alpha = static_cast< int >( hit.alpha );

	if ( int_alpha > 0 )
	{
		auto col = Color( 255, 255, 255, int_alpha );

		render::get().line( Vector2D( screen_pos.x - line_size * multiplicator, screen_pos.y - line_size * multiplicator ), Vector2D( screen_pos.x - line_size, screen_pos.y - line_size ), col );
		render::get().line( Vector2D( screen_pos.x - line_size * multiplicator, screen_pos.y + line_size * multiplicator ), Vector2D( screen_pos.x - line_size, screen_pos.y + line_size ), col );
		render::get().line( Vector2D( screen_pos.x + line_size * multiplicator, screen_pos.y + line_size * multiplicator ), Vector2D( screen_pos.x + line_size, screen_pos.y + line_size ), col );
		render::get().line( Vector2D( screen_pos.x + line_size * multiplicator, screen_pos.y - line_size * multiplicator ), Vector2D( screen_pos.x + line_size, screen_pos.y - line_size ), col );

		//col = Color( red, green, 0, int_alpha );
		//_( s, "- " );
		//render::get().text( Vector2D( screen_pos.x + 8, screen_pos.y - 12 + hit.moved ), s + to_string( hit.damage ), col, fonts::esp_small, c_font::drop_shadow );
	}
}