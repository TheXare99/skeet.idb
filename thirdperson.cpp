#include "../include_cheat.h"

void thirdperson::run() const
{
	if ( !g_pEngine->IsInGame() || !g_pLocalPlayer )
		return;

	if ( g_pLocalPlayer->get_alive() && cfg.visuals.effects.force_third_person_alive && g_thirdperson_active)
	{
		g_pInput->m_fCameraInThirdPerson = true;
		g_pInput->m_vecCameraOffset = Vector( g_original_viewangles.x, g_original_viewangles.y, 150.f);

		const auto cam_hull_offset = 12.f + (( cfg.misc.miscellaneous.override_fov) / 4.8f - 18.f );
		const Vector cam_hull_extents( cam_hull_offset, cam_hull_offset, cam_hull_offset );
		Vector cam_forward;
		const auto origin = g_pLocalPlayer->get_eye_pos();

		math::get().angle_vectors( QAngle( g_pInput->m_vecCameraOffset.x, g_pInput->m_vecCameraOffset.y, 0.0f ), &cam_forward );

		trace_t trace;
		util_trace_hull( origin, origin - ( cam_forward * g_pInput->m_vecCameraOffset.z ), MASK_SOLID & ~CONTENTS_MONSTER, cam_hull_extents, &trace );

		g_pInput->m_vecCameraOffset.z *= trace.fraction;
	}
	else if ( g_pInput->m_fCameraInThirdPerson )
	{
		g_pInput->m_fCameraInThirdPerson = false;
	}

	static auto b_once = false;

	if ( g_pLocalPlayer->get_alive() || !cfg.visuals.effects.force_third_person_dead )
	{
		b_once = false;
		return;
	}

	if ( b_once )
		g_pLocalPlayer->get_observer_mode() = 5;

	if ( g_pLocalPlayer->get_observer_mode() == 4 )
		b_once = true;
}

void thirdperson::set_networked_angle( const QAngle networked_angle ) const
{
	this->networked_angle = networked_angle;
}

void thirdperson::set_clientside_angle(const QAngle clientside_angle) const
{
	this->clientside_angle = clientside_angle;
}

