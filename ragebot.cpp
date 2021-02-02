#include "../include_cheat.h"

void ragebot::no_recoil() {
	if (!cfg.ragebot.aimbot.enabled || !cfg.ragebot.other.no_recoil)
		return;

	_(weapon_recoil_scale_s, "weapon_recoil_scale");
	static auto weapon_recoil_scale = g_pCVar->FindVar(weapon_recoil_scale_s);

	g_cmd->viewangles -= g_pLocalPlayer->get_aim_punch() * weapon_recoil_scale->get_float();
}

void ragebot::no_visual_recoil(CViewSetup& v_view) {
	if (!g_pLocalPlayer || !cfg.ragebot.aimbot.enabled || g_pInput->m_fCameraInThirdPerson || !g_pLocalPlayer->get_alive())
		return;

	v_view.angles -= g_pLocalPlayer->get_aim_punch() * 0.9f + g_pLocalPlayer->get_view_punch();
}

void ragebot::run() {
	targets.clear();

	if (!cfg.ragebot.aimbot.enabled || (*g_ppGameRules)->m_bFreezePeriod() || !g_pLocalPlayer->get_alive())
		return;

	select_targets();

	scan_targets();
}

void ragebot::select_targets() {
	for (auto i = 1; i < g_pGlobals->maxClients; i++) {
		auto player = get_entity(i);
		if (!player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer || player->get_immunity())
			continue;

		targets.emplace_back(player, i);
	}
}

void ragebot::scan_targets() {
	auto best_dist = std::numeric_limits< float >::max();
	auto best_fov = std::numeric_limits< float >::max();
	auto best_damage = 0.f;
	auto best_hp = std::numeric_limits< int >::max();
	auto best_lag = std::numeric_limits< float >::max();
	auto best_height = std::numeric_limits< float >::max();

	lag_record_t* best_record = nullptr;
	aimpoint_t best_point(Vector(0.f, 0.f, 0.f), -1);
	C_CSPlayer* best_player = nullptr;

	player_log::get().filter_records(true);

	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	for (size_t i = 0; i < enabled_hitboxes.size(); i++) {
		switch (i) {
		case HITBOX_HEAD:
			enabled_hitboxes[i] = !g_force_body_aim && cfg.ragebot.aimbot.target_hitbox[0];
			break;
		case HITBOX_PELVIS:
		case HITBOX_STOMACH:
			enabled_hitboxes[i] = cfg.ragebot.aimbot.target_hitbox[2];
			break;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			enabled_hitboxes[i] = cfg.ragebot.aimbot.target_hitbox[1];
			break;
		case HITBOX_RIGHT_THIGH:
		case HITBOX_LEFT_THIGH:
		case HITBOX_RIGHT_CALF:
		case HITBOX_LEFT_CALF:
			enabled_hitboxes[i] = !g_force_body_aim && cfg.ragebot.aimbot.target_hitbox[4];
			break;
		case HITBOX_RIGHT_FOOT:
		case HITBOX_LEFT_FOOT:
			enabled_hitboxes[i] = !g_force_body_aim && cfg.ragebot.aimbot.target_hitbox[5];
			break;
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_LEFT_UPPER_ARM:
			enabled_hitboxes[i] = !g_force_body_aim && cfg.ragebot.aimbot.target_hitbox[3];
			break;
		default:
			break;
		}
	}

	for (auto& target : targets) {
		auto& log = player_log::get().get_log(target.index);
		if (!log.player)
			continue;

		for (auto& record : log.record)	{
			if (!record.valid) // чекаем валидный ли наш рекорд.
				continue;

			float damage, dist, fov, height;
			int   hp;
			const aimpoint_t point = get_best_damage(record, damage);

			// фикс чтоб чит не ебланил если таргет селекшон стоит не по дамагу.
			if (point.point == Vector(0.f, 0.f, 0.f) || point.hitbox == -1)
				continue;

			// спиздил с супримаси, хот€ идк нахуй он нужен вообще мне.
			switch (cfg.ragebot.aimbot.target_selection) {
				// до кого ближе того и ебашим.
			case 0:
				dist = (record.m_origin - g_pLocalPlayer->get_eye_pos()).Length();

				if (dist < best_dist) {
					best_dist = dist;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
				// ну тупа по прицелу хули.
			case 1:
				fov = math::get().get_fov(g_original_viewangles, math::get().calc_angle(g_pLocalPlayer->get_eye_pos(), point.point));

				if (fov < best_fov) {
					best_fov = fov;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
				// а тут по дамажке.
			case 2:
				if (damage > best_damage) {
					best_damage = damage;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
				// дл€ хуесосов, чтобы стилить килы.
			case 3:
				hp = target.player->get_health();

				if (hp < best_hp) {
					best_hp = hp;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
				// у кого фейклаги более нищие того и бьем.
			case 4:
				if (record.m_lag < best_lag) {
					best_lag = record.m_lag;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
				// хуй знает че за высота бл€ть, но в м€се было - пусть и тут будет.
			case 5:
				height = record.m_origin.z - g_pLocalPlayer->get_origin().z;

				if (height < best_height) {
					best_height = height;
					best_point = point;
					best_record = &record;
					best_player = target.player;
				}
				break;
			}
		}
	}

	if (!best_record)
		return;

	const auto model = best_player->GetModel();
	if (!model)
		return;

	const auto studio_model = g_pModelInfo->GetStudioModel(model);
	if (!studio_model)
		return;

	const auto aim_angles = math::get().calc_angle(g_pLocalPlayer->get_eye_pos(), best_point.point);
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	auto hitchance = calculate_hitchance(best_point, *best_record, studio_model);

	switch (cfg.ragebot.other.automatic_stop) {
	case 0:
		g_stop = false;
		g_slowwalk = false;
		break;
	case 1:
		g_stop = true;
		g_slowwalk = false;
		break;
	case 2:
		g_stop = false;
		g_slowwalk = true;
		break;
	}

	auto_stop();

	if (!can_shoot())
		return;

	if (!hitchance && !is_knife && !is_zeus) {
		if (cfg.ragebot.aimbot.automatic_scope) {
			if (weapon->get_weapon_id() == WEAPON_SSG08 || weapon->get_weapon_id() == WEAPON_SG556 || weapon->get_weapon_id() == WEAPON_AUG || weapon->get_weapon_id() == WEAPON_AWP || weapon->get_weapon_id() == WEAPON_G3SG1 || weapon->get_weapon_id() == WEAPON_SCAR20) {
				if (!weapon->get_zoom_level()) {
					g_cmd->buttons |= IN_ZOOM;
				}
			}
		}

		return;
	}

	if (weapon->get_weapon_id() == WEAPON_REVOLVER) {
		const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
		if (!fl_postpone_fire_ready || fl_postpone_fire_ready > g_pGlobals->curtime)
			return;
	}

	g_cmd->viewangles = aim_angles;

	if (!cfg.ragebot.aimbot.silent_aim)
		g_pEngine->SetViewAngles(g_cmd->viewangles);

	if (!g_csgobuild_invalid) // хуй ты норм в мув попадеш друг, если верси€ кс не та что нада.
		g_cmd->tick_count = lagcomp::get().fix_tickcount(best_record->m_sim_time);

	if (cfg.ragebot.aimbot.automatic_fire) {
		g_send_packet = true;

		if (is_knife) {
			if (g_pLocalPlayer->get_eye_pos().Dist(best_point.point) <= 48.f)
				g_cmd->buttons |= IN_ATTACK2;
			else
				g_cmd->buttons |= IN_ATTACK;
		}
		else {
			g_cmd->buttons |= IN_ATTACK;
			resolver::get().add_shot(g_pLocalPlayer->get_eye_pos(), best_record, best_player->EntIndex(), studio_model, best_point.hitbox);
			if (cfg.visuals.effects.bullet_impacts)
				g_pDebugOverlay->AddBoxOverlay(best_point.point, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 255, 0, 0, 127, 4);
		}
	}

	ragebot_called = g_pGlobals->curtime;
}

bool ragebot::can_shoot(const float time, bool check) {
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return false;

	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;
	const auto weapontype = weapon->get_weapon_type();

	if (weapontype == WEAPONTYPE_C4 || weapon->is_grenade())
		return false;

	if (weapon->get_clip1() < 1 && !is_knife)
		return false;

	if (weapon->in_reload())
		return false;

	if (weapon->get_next_primary_attack() > time)
		return false;

	if (g_pLocalPlayer->get_next_attack() > time)
		return false;

	if (!check) {
		if (weapon->get_weapon_id() == WEAPON_REVOLVER && get_config(weapon)->autorevolver->get<bool>()) {
			const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
			if (!fl_postpone_fire_ready || fl_postpone_fire_ready > time)
				g_cmd->buttons |= IN_ATTACK;
		}
	}
	else {
		if (weapon->get_weapon_id() == WEAPON_REVOLVER)	{
			const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
			if (fl_postpone_fire_ready > time)
				return false;
		}
	}

	return true;
}

void ragebot::auto_stop() {
	auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return;

	if (!can_shoot() && !cfg.ragebot.other.stop_between_shots)
		return;

	auto animstate = g_pLocalPlayer->get_anim_state();
	if (!animstate)
		return;

	if (g_pLocalPlayer->get_flags() & FL_ONGROUND && !(g_cmd->buttons & IN_JUMP)) {
		if (g_stop) {
			if (animstate->m_flSpeed > 15.f) {
				QAngle angle;
				math::vector_angles(g_pLocalPlayer->get_velocity(), angle);

				angle.y = prediction::get().unpred_viewangles.y - angle.y;

				Vector direction;
				math::angle_vectors(angle, &direction);

				Vector stop = direction * -animstate->m_flSpeed;

				g_cmd->forwardmove = stop.x;
				g_cmd->sidemove = stop.y;
			}
			else {
				g_cmd->forwardmove = 0;
				g_cmd->sidemove = 0;
			}
		}
		else if (g_slowwalk) {
			float flMaxSpeed = g_pLocalPlayer->get_scoped() ? weapon->get_wpn_data()->flMaxSpeedAlt : weapon->get_wpn_data()->flMaxSpeed;
			float flDesiredSpeed = (flMaxSpeed * 0.33000001);

			if (animstate->m_flSpeed <= flDesiredSpeed) {
				g_cmd->buttons |= IN_SPEED;

				float squirt = sqrtf((g_cmd->forwardmove * g_cmd->forwardmove) + (g_cmd->sidemove * g_cmd->sidemove));

				if (squirt > flDesiredSpeed) {
					float squirt2 = sqrtf((g_cmd->forwardmove * g_cmd->forwardmove) + (g_cmd->sidemove * g_cmd->sidemove));

					float cock1 = g_cmd->forwardmove / squirt2;
					float cock2 = g_cmd->sidemove / squirt2;

					if (flDesiredSpeed + 1.0 <= animstate->m_flSpeed) {
						g_cmd->forwardmove = 0;
						g_cmd->sidemove = 0;
					}
					else {
						g_cmd->forwardmove = cock1 * flDesiredSpeed;
						g_cmd->sidemove = cock2 * flDesiredSpeed;
					}
				}
			}
			else {
				QAngle angle;
				math::get().vector_angles(g_pLocalPlayer->get_velocity(), angle);

				angle.y = prediction::get().unpred_viewangles.y - angle.y;

				Vector direction;
				math::get().angle_vectors(angle, &direction);

				Vector stop = direction * -animstate->m_flSpeed;

				g_cmd->forwardmove = stop.x;
				g_cmd->sidemove = stop.y;
			}
		}
	}
}

aimpoint_t ragebot::get_best_damage(lag_record_t& log, float& damage) {
	const auto player = get_entity(log.index);
	const auto& player_log = player_log::get().get_log(log.index);
	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());

	lag_record_t backup(player);
	log.apply(player);

	std::vector<aimpoint_t> points;

	multipoint_hitboxes(log, points);

	auto best_damage = 0.f;
	auto prevhitbox = -1;
	aimpoint_t best_point(Vector(0.f, 0.f, 0.f), -1);

	if (!weapon)
		return best_point;

	const auto info = weapon->get_wpn_data();

	if (!info)
		return best_point;

	const auto is_zeus = get_weapon(g_pLocalPlayer->get_active_weapon())->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && get_weapon(g_pLocalPlayer->get_active_weapon())->get_weapon_type() == WEAPONTYPE_KNIFE;

	for (auto& point : points) {
		// ебаный чек на фов.
		if (math::get().get_fov(g_original_viewangles, math::get().calc_angle(g_pLocalPlayer->get_eye_pos(), point.point)) > cfg.ragebot.aimbot.maximum_fov)
			continue;

		// нахуй нам сканить поинты, по которым мы не сможем стрел€ть?
		if ((g_pLocalPlayer->get_eye_pos() - point.point).Length() > info->flWeaponRange)
			continue;

		const auto dist = g_pLocalPlayer->get_eye_pos().Dist(point.point);

		if (is_knife && dist > 64.0f)
			continue;

		float point_damage = 0.f;

		if (!is_knife && !penetration::get().can_hit(player, point.point, point_damage, g_pLocalPlayer->get_eye_pos()))
			continue;

		if (!is_knife && point_damage < cfg.ragebot.aimbot.minimum_damage && point_damage < player->get_health())
			continue;

		if (is_zeus && point_damage < player->get_health())
			continue;

		if (is_knife)
			point_damage = 100.f / dist;

		// бл€дские баим кондиции.
		if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && cfg.ragebot.other.body_aim_conditions[0] && point_damage > player->get_health()) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && cfg.ragebot.other.body_aim_conditions[1] && !(log.m_flags & FL_ONGROUND)) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && cfg.ragebot.other.body_aim_conditions[2] && player_log.m_nMissedShotsResolver >= cfg.ragebot.other.x_missed_shots) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && cfg.ragebot.other.body_aim_conditions[3] && player->get_health() <= cfg.ragebot.other.x_hp) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && cfg.ragebot.other.body_aim_conditions[4] && point_damage > cfg.ragebot.other.x_dmg) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
		else if ((point_damage > best_damage && prevhitbox != point.hitbox) || point_damage > best_damage + 30.f) {
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
	}

	backup.apply(player, true);

	damage = best_damage;

	return best_point;
};

void ragebot::multipoint_hitboxes(lag_record_t& log, std::vector<aimpoint_t>& points) {
	const auto player = get_entity(log.index);

	auto& player_log = player_log::get().get_log(log.index);

	const auto model = player->GetModel();
	if (!model)
		return;

	const auto studio_hdr = g_pModelInfo->GetStudioModel(model);
	if (!studio_hdr)
		return;

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;
	const auto weapon_inaccuracy = weapon->get_inaccuracy();
	const auto weapon_spread = weapon->get_spread();

	for (auto i = 0; i < HITBOX_MAX; i++) {
		if (is_zeus && (i != HITBOX_STOMACH && i != HITBOX_PELVIS))
			continue;

		if (is_knife && i != HITBOX_UPPER_CHEST)
			continue;

		if (!is_knife && !is_zeus && (!enabled_hitboxes[i]))
			continue;

		auto hitbox = studio_hdr->pHitbox(i, NULL);
		if (!hitbox)
			continue;

		auto& mat = log.matrix[hitbox->bone];
		const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;

		Vector min, max, transformed_center;
		math::get().vector_transform(hitbox->bbmin - mod, mat, min);
		math::get().vector_transform(hitbox->bbmax + mod, mat, max);

		const auto center = (min + max) * .5f;

		// центр хитбокса ебать.
		points.emplace_back(center, i);

		// наху€ мульты если чел на спидах бежыт (да-да можно сделать проверку на велосити, но это не стильно).
		if (log.m_max_desync_delta < 35.f)
			continue;

		// ну лети лети, на залупу бл€ть.
		if (!(log.m_flags & FL_ONGROUND))
			continue;

		// если дура стрел€ет то она и так улетит оншотом, так что похуй на мульты.
		if (log.m_shot)
			continue;

		// наху€ мульты на зевсе?
		if (is_zeus)
			continue;

		// на эти хитбоксы мульты нахуй не нужны.
		if (i == HITBOX_NECK || i == HITBOX_RIGHT_THIGH || i == HITBOX_LEFT_THIGH || i == HITBOX_RIGHT_CALF || i == HITBOX_LEFT_CALF || i == HITBOX_RIGHT_FOOT || i == HITBOX_LEFT_FOOT || i == HITBOX_RIGHT_HAND || i == HITBOX_LEFT_HAND || i == HITBOX_RIGHT_UPPER_ARM || i == HITBOX_LEFT_UPPER_ARM || i == HITBOX_RIGHT_FOREARM || i == HITBOX_LEFT_FOREARM)
			continue;

		const auto cur_angles = math::get().calc_angle(center, g_pLocalPlayer->get_eye_pos());

		Vector forward;
		math::get().angle_vectors(cur_angles, &forward);

		// дл€ ножа а чо.
		if (is_knife) {
			const auto back = forward * hitbox->radius;
			points.emplace_back(center + back, i);
			continue;
		}

		// щитаем скейлы.
		auto rs = hitbox->radius * .2f;
		switch (i) {
		case HITBOX_HEAD:
			rs = hitbox->radius * (cfg.ragebot.aimbot.head_hitbox_scale_limit / 100.f);
			break;
		case HITBOX_PELVIS:
		case HITBOX_STOMACH:
			rs = hitbox->radius * (cfg.ragebot.aimbot.stomach_hitbox_scale_limit / 100.f);
			break;
		case HITBOX_LOWER_CHEST:
		case HITBOX_CHEST:
		case HITBOX_UPPER_CHEST:
			rs = hitbox->radius * (cfg.ragebot.aimbot.chest_hitbox_scale_limit / 100.f);
			break;
		default:
			break;
		}

		// миссаем по сприду - умешьшаем скейлы.
		if (player_log.m_nMissedShotsSpread < 4)
			rs *= 1 - (player_log.m_nMissedShotsSpread / 4);
		else
			continue;

		// какой смысл бл€ть от скейлов < 0.2 если оно ничего кроме нагрузки пк сканами кучи безполезных точек не даст?
		if (rs < .2f)
			continue;

		// а ета с лв спиздил, похуй, пусть будет.
		math::get().vector_transform(center, mat, transformed_center);
		auto spread = weapon_inaccuracy + weapon_spread;
		auto distance = transformed_center.DistToSqr(g_pLocalPlayer->get_eye_pos());
		distance /= math::fast_sin(DEG2RAD(90.f - RAD2DEG(spread)));
		spread = math::fast_sin(spread);
		auto radius = std::max(hitbox->radius - distance * spread, 0.f);
		rs = hitbox->radius * clamp(radius / hitbox->radius, 0.f, rs);

		const auto right = forward.Cross(Vector(0, 0, 1)) * rs;
		const auto left = Vector(-right.x, -right.y, right.z);
		const auto top = Vector(0, 0, 1) * rs;

		if (i == HITBOX_HEAD)
			points.emplace_back(center + top, i);
		points.emplace_back(center + right, i);
		points.emplace_back(center + left, i);
	}
}

bool ragebot::can_hit_hitbox(const Vector start, const Vector end, lag_record_t& record, studiohdr_t* hdr, int box) {
	const auto studio_box = hdr->pHitbox(static_cast<uint32_t>(box), 0);

	if (!studio_box)
		return false;

	Vector min, max;
	const auto is_capsule = studio_box->radius != -1.f;

	if (is_capsule)	{
		math::get().vector_transform(studio_box->bbmin, record.matrix[studio_box->bone], min);
		math::get().vector_transform(studio_box->bbmax, record.matrix[studio_box->bone], max);
		const auto dist = math::get().segment_to_segment(start, end, min, max);

		if (dist < studio_box->radius)
			return true;
	}
	else {
		// спиздил нагло с рифка, и мне похуй.
		math::get().vector_transform(math::get().vector_rotate(studio_box->bbmin, studio_box->rotation), record.matrix[studio_box->bone], min);
		math::get().vector_transform(math::get().vector_rotate(studio_box->bbmax, studio_box->rotation), record.matrix[studio_box->bone], max);
		math::get().vector_i_transform(start, record.matrix[studio_box->bone], min);
		math::get().vector_i_rotate(end, record.matrix[studio_box->bone], max);

		if (math::get().intersect_line_with_bb(min, max, studio_box->bbmin, studio_box->bbmax))
			return true;
	}

	return false;
}

static std::vector<std::tuple<float, float, float>> precomputed_seeds = {};

__forceinline void ragebot::build_seed_table() {
	if (!precomputed_seeds.empty())
		return;

	for (auto i = 0; i < 255; i++) {
		random_seed(i + 1);
		const auto pi_seed = random_float(0.f, pi * 2.f);
		precomputed_seeds.emplace_back(random_float(0.f, 1.f),
			sin(pi_seed), cos(pi_seed));
	}
}

bool ragebot::calculate_hitchance(const aimpoint_t& point, lag_record_t& record, studiohdr_t* hdr) const {
	// это говно тоже с рифка, хуйн€, но хсать чудищ ноускопом через пол мапы кайфова.
	// generate look-up-table to enhance performance.
	build_seed_table();

	// тут просто по легиту балуемс€.
	if (g_csgobuild_invalid)
		return true;

	__(weapon_accuracy_nospread, "weapon_accuracy_nospread");
	static auto nospread = g_pCVar->FindVar(weapon_accuracy_nospread);

	// если сервак без сприда и включен ноурекойл - хитшанс нам нахуй не нужен.
	if (nospread->get_bool() && cfg.ragebot.other.no_recoil)
		return true;

	const auto weapon = get_weapon(g_pLocalPlayer->get_active_weapon());
	if (!weapon)
		return false;

	const auto info = weapon->get_wpn_data();

	if (!info)
		return false;

	// setup calculation parameters.
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	const auto sniper = weapon->get_weapon_id() == WEAPON_AWP || weapon->get_weapon_id() == WEAPON_G3SG1
		|| weapon->get_weapon_id() == WEAPON_SCAR20 || weapon->get_weapon_id() == WEAPON_SSG08;
	const auto crouched = g_pLocalPlayer->get_flags() & FL_DUCKING;

	// calculate inaccuracy.
	const auto weapon_inaccuracy = weapon->get_inaccuracy();

	if (weapon->get_weapon_id() == WEAPON_REVOLVER)
		return weapon_inaccuracy < (crouched ? .0020f : .0055f);

	// no need for hitchance, if we can't increase it anyway.
	if (crouched)
	{
		if (round_acc(weapon_inaccuracy) == round_acc((sniper && g_pLocalPlayer->get_scoped()) ? info->flInaccuracyCrouchAlt : info->flInaccuracyCrouch))
			return true;
	}
	else
	{
		if (round_acc(weapon_inaccuracy) == round_acc((sniper && g_pLocalPlayer->get_scoped()) ? info->flInaccuracyStandAlt : info->flInaccuracyStand))
			return true;
	}

	// шобы стрел€ть в аирах со скаутом.
	if (weapon->get_weapon_id() == WEAPON_SSG08 && !(g_pLocalPlayer->get_flags() & FL_ONGROUND))
		if (weapon_inaccuracy < 0.009f)
			return true;

	// calculate start and angle.
	const auto start = g_pLocalPlayer->get_eye_pos();
	const auto aim_angle = math::get().calc_angle(start, point.point);

	Vector forward, right, up;
	math::get().angle_vectors(aim_angle, forward, right, up);

	// keep track of all traces that hit the enemy.
	auto current = 0;

	// setup calculation parameters.
	Vector total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	// use look-up-table to find average hit probability.
	for (auto i = 0u; i < 255; i++) { // NOLINT(modernize-loop-convert)
		// get seed.
		seed = &precomputed_seeds[i];

		// calculate spread.
		inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		spread_x = std::get<2>(*seed) * inaccuracy;
		spread_y = std::get<1>(*seed) * inaccuracy;
		total_spread = (forward + right * spread_x + up * spread_y);
		total_spread.Normalize();

		// calculate angle with spread applied.
		math::get().vector_angles(total_spread, spread_angle);

		// calculate end point of trace.
		math::get().angle_vectors(spread_angle, end);
		end.Normalize();
		end = start + end * info->flWeaponRange;

		// did we hit the hitbox?
		if (can_hit_hitbox(start, end, record, hdr, point.hitbox))
			current++;
		
		// abort if hitchance is already sufficent.
		if ((static_cast<float>(current) / 2.55f) >= cfg.ragebot.aimbot.minimum_hit_chance)
			return true;

		// abort if we can no longer reach hitchance.
		if ((static_cast<float>(current + 255 - i) / 2.55f) < cfg.ragebot.aimbot.minimum_hit_chance)
			return false;
	}

	return (static_cast<float>(current) / 2.55f) >= cfg.ragebot.aimbot.minimum_hit_chance;
}