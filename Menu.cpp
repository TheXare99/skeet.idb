#include "../include_cheat.h"
#include "MenuControls.h"
#include "MenuBackground.h"
#include "MenuFonts.h"
#include "Dropdown.h"

void Menu::Init(IDirect3DDevice9* pDevice)
{
	if (m_bInitialized)
		return;

	ImGui::CreateContext();
	auto io = ImGui::GetIO();
	auto style = &ImGui::GetStyle();

	style->WindowRounding = 0.f;
	style->AntiAliasedLines = true;
	style->AntiAliasedFill = true;
	style->ScrollbarRounding = 0.f;
	style->ScrollbarSize = 6.f;
	style->WindowPadding = ImVec2(0, 0);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1.f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(65 / 255.f, 65 / 255.f, 65 / 255.f, 1.f);
	style->Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.2f, 0.2f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

	m_bIsOpen = true;

	IDirect3DSwapChain9* pChain = nullptr;
	D3DPRESENT_PARAMETERS pp = {};
	D3DDEVICE_CREATION_PARAMETERS param = {};
	pDevice->GetCreationParameters(&param);
	pDevice->GetSwapChain(0, &pChain);

	if (pChain)
		pChain->GetPresentParameters(&pp);

	ImGui_ImplWin32_Init(param.hFocusWindow);
	ImGui_ImplDX9_Init(pDevice);

	if(!menu_globals::menuBg)
		D3DXCreateTextureFromFileInMemoryEx(pDevice, &MenuBG, sizeof(MenuBG),
		648, 548, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, NULL, NULL, &menu_globals::menuBg);

	ImFontConfig _cfg;
	menu_globals::tabFont = io.Fonts->AddFontFromMemoryTTF(&tabFont, sizeof tabFont, 50.f, &_cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	menu_globals::boldMenuFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(boldMenuFont, sizeof boldMenuFont, 11, &_cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	menu_globals::menuFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(menuFont, sizeof menuFont, 11, &_cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	menu_globals::controlFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(controlFont, sizeof controlFont, 12, &_cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	menu_globals::weaponFont = io.Fonts->AddFontFromMemoryTTF(&weaponFont, sizeof weaponFont, 26.f, &_cfg, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	menu_globals::keybindsFont = io.Fonts->AddFontFromMemoryTTF(keybindsFont, sizeof keybindsFont, 10.f, &_cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontDefault();

	m_bInitialized = true;
}

void Menu::Begin()
{
	if (!m_bInitialized)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Menu::End() {
	if (!m_bInitialized)
		return;

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::ColorPicker(const char* name, float* color, bool alpha) {

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiStyle* style = &ImGui::GetStyle();

	auto alphaSliderFlag = alpha ? ImGuiColorEditFlags_AlphaBar : ImGuiColorEditFlags_NoAlpha;

	ImGui::SameLine(window->Size.x - 45);
	ImGui::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
}

void update_multiselect_comboboxes() {
	// дадада, говнокод уровень симвал - мне поебать зато это ворк.
	// Target hitbox.
	for (size_t i = 0; i < target_hitbox.size(); ++i)
		target_hitbox_ib[i] = cfg.ragebot.aimbot.target_hitbox[i];
	if (old_target_hitbox_ib != target_hitbox_ib) {
		old_target_hitbox_ib = target_hitbox_ib;
		multiselect_comboboxes_previews[0] = "";
		std::vector<std::string> vec;
		for (size_t i = 0; i < target_hitbox.size(); i++)
			if (cfg.ragebot.aimbot.target_hitbox[i]) vec.push_back(target_hitbox.at(i));
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec.size() == 1)
				multiselect_comboboxes_previews[0] += vec.at(i);
			else if (i != vec.size())
				multiselect_comboboxes_previews[0] += vec.at(i) + ", ";
			else
				multiselect_comboboxes_previews[0] += vec.at(i);
		}
		if (multiselect_comboboxes_previews[0].size() >= 2) {
			std::string temp(multiselect_comboboxes_previews[0], (multiselect_comboboxes_previews[0].size() - 2), 2);
			if (!temp.compare(", ")) multiselect_comboboxes_previews[0] = multiselect_comboboxes_previews[0].substr(0, multiselect_comboboxes_previews[0].size() - 2);
		}
	}
	// Body aim conditions.
	for (size_t i = 0; i < body_aim_conditions.size(); ++i)
		body_aim_conditions_ib[i] = cfg.ragebot.other.body_aim_conditions[i];
	if (old_body_aim_conditions_ib != body_aim_conditions_ib) {
		old_body_aim_conditions_ib = body_aim_conditions_ib;
		multiselect_comboboxes_previews[1] = "";
		std::vector<std::string> vec;
		for (size_t i = 0; i < body_aim_conditions.size(); i++)
			if (cfg.ragebot.other.body_aim_conditions[i]) vec.push_back(body_aim_conditions.at(i));
		for (size_t i = 0; i < vec.size(); i++) {
			if (vec.size() == 1)
				multiselect_comboboxes_previews[1] += vec.at(i);
			else if (i != vec.size())
				multiselect_comboboxes_previews[1] += vec.at(i) + ", ";
			else
				multiselect_comboboxes_previews[1] += vec.at(i);
		}
		if (multiselect_comboboxes_previews[1].size() >= 2) {
			std::string temp(multiselect_comboboxes_previews[1], (multiselect_comboboxes_previews[1].size() - 2), 2);
			if (!temp.compare(", ")) multiselect_comboboxes_previews[1] = multiselect_comboboxes_previews[1].substr(0, multiselect_comboboxes_previews[1].size() - 2);
		}
	}
}

void Menu::Render() {
	// ебаный костыль чтобы мультиселекты отображали правильную превьюшку.
	update_multiselect_comboboxes();

	ImGuiStyle* style = &ImGui::GetStyle();

	style->Colors[ImGuiCol_MenuTheme] = ImVec4(cfg.misc.settings.theme_color[0], cfg.misc.settings.theme_color[1], cfg.misc.settings.theme_color[2], cfg.misc.settings.theme_color[3]);
	style->WindowPadding = ImVec2(6, 6);

	ImGui::PushFont(menu_globals::menuFont);

	ImGui::SetNextWindowSize(ImVec2(660.f, 560.f));
	ImGui::BeginMenuBackground("скит айдиби нахуй, пон€л чудище?", &Menu::get().m_bIsOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar); {

		ImGui::BeginChild("Complete Border", ImVec2(648.f, 548.f), false); {

			ImGui::Image(menu_globals::menuBg, ImVec2(648.f, 548.f));

		} ImGui::EndChild();

		ImGui::SameLine(6.f);

		style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 0);

		ImGui::BeginChild("Menu Contents", ImVec2(648.f, 548.f), false); {

			ImGui::ColorBar("unicorn", ImVec2(648.f, 2.f));

			ImGui::BeginTabs("Tabs", ImVec2(75.f, 542.f), false); {

				style->ItemSpacing = ImVec2(0.f, 0.f);

				style->ButtonTextAlign = ImVec2(0.5f, 0.47f);

				ImGui::PopFont();
				ImGui::PushFont(menu_globals::tabFont);

				switch (m_nActiveTab) {

				case 0:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::SelectedTab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 1:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::SelectedTab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 2:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::SelectedTab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 3:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::SelectedTab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 4:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::SelectedTab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 5:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::SelectedTab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::Tab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				case 6:
					ImGui::TabSpacer("##Top Spacer", ImVec2(75.f, 10.f));

					if (ImGui::Tab("A", ImVec2(75.f, 75.f))) m_nActiveTab = 0;
					if (ImGui::Tab("G", ImVec2(75.f, 75.f))) m_nActiveTab = 1;
					if (ImGui::Tab("B", ImVec2(75.f, 75.f))) m_nActiveTab = 2;
					if (ImGui::Tab("C", ImVec2(75.f, 75.f))) m_nActiveTab = 3;
					if (ImGui::Tab("D", ImVec2(75.f, 75.f))) m_nActiveTab = 4;
					if (ImGui::Tab("E", ImVec2(75.f, 75.f))) m_nActiveTab = 5;
					if (ImGui::SelectedTab("F", ImVec2(75.f, 75.f))) m_nActiveTab = 6;

					ImGui::TabSpacer2("##Bottom Spacer", ImVec2(75.f, 7.f));
					break;
				}

				ImGui::PopFont();
				ImGui::PushFont(menu_globals::menuFont);

				style->ButtonTextAlign = ImVec2(0.5f, 0.5f);

			} ImGui::EndTabs();

			ImGui::SameLine(75.f);

			ImGui::BeginChild("Tab Contents", ImVec2(572.f, 542.f), false); {

				style->Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);

				switch (m_nActiveTab) {

				case 0:
					Ragebot();
					break;
				case 1:
					Anti_aimbot();
					break;
				case 2:
					Legitbot();
					break;
				case 3:
					Visuals();
					break;
				case 4:
					Misc();
					break;
				case 5:
					Skins();
					break;
				case 6:
					Players();
					break;
				}

				style->Colors[ImGuiCol_Border] = ImColor(10, 10, 10, 255);

			} ImGui::EndChild();

			style->ItemSpacing = ImVec2(4.f, 4.f);
			style->Colors[ImGuiCol_ChildBg] = ImColor(17, 17, 17, 255);

		} ImGui::EndChild();

		ImGui::PopFont();

	} ImGui::End();
}

void Menu::Shutdown() {

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
}
void dmt(std::string key) {
	/*
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text(key.c_str());
		ImGui::EndTooltip();
	}
	*/
}

void Menu::Ragebot() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Aimbot", 506.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Enabled", cfg.ragebot.aimbot.enabled); dmt("cfg.ragebot.aimbot.enabled");
			InsertCombo("Target selection", cfg.ragebot.aimbot.target_selection, target_selection); dmt("cfg.ragebot.aimbot.target_selection");
			InsertMultiCombo("Target hitbox", cfg.ragebot.aimbot.target_hitbox, target_hitbox, multiselect_comboboxes_previews[0]); dmt("cfg.ragebot.aimbot.target_hitbox");
			if (cfg.ragebot.aimbot.target_hitbox[0])
			{
				InsertSlider("Head hitbox scale limit", cfg.ragebot.aimbot.head_hitbox_scale_limit, 20.f, 100.f, "%1.f%%"); dmt("cfg.ragebot.aimbot.head_hitbox_scale_limit");
			}
			if (cfg.ragebot.aimbot.target_hitbox[1])
			{
				InsertSlider("Chest hitbox scale limit", cfg.ragebot.aimbot.chest_hitbox_scale_limit, 20.f, 100.f, "%1.f%%"); dmt("cfg.ragebot.aimbot.chest_hitbox_scale_limit");
			}
			if (cfg.ragebot.aimbot.target_hitbox[2])
			{
				InsertSlider("Stomach hitbox scale limit", cfg.ragebot.aimbot.stomach_hitbox_scale_limit, 20.f, 100.f, "%1.f%%"); dmt("cfg.ragebot.aimbot.stomach_hitbox_scale_limit");
			}
			InsertCheckbox("Automatic fire", cfg.ragebot.aimbot.automatic_fire); dmt("cfg.ragebot.aimbot.automatic_fire");
			InsertCheckbox("Silent aim", cfg.ragebot.aimbot.silent_aim); dmt("cfg.ragebot.aimbot.silent_aim");
			InsertSlider("Minimum hit chance", cfg.ragebot.aimbot.minimum_hit_chance, 1.f, 99.f, "%1.f%%"); dmt("cfg.ragebot.aimbot.minimum_hit_chance");
			InsertSlider("Minimum damage", cfg.ragebot.aimbot.minimum_damage, 1.f, 100.f, "%1.f hp%"); dmt("cfg.ragebot.aimbot.minimum_damage");
			InsertCheckbox("Automatic scope", cfg.ragebot.aimbot.automatic_scope); dmt("cfg.ragebot.aimbot.automatic_scope");
			InsertSlider("Maximum FOV", cfg.ragebot.aimbot.maximum_fov, 1.f, 360.f, "%1.f∞%"); dmt("cfg.ragebot.aimbot.maximum_fov");
			InsertCheckbox("Log misses due to spread", cfg.ragebot.aimbot.log_misses_due_to_spread); dmt("cfg.ragebot.aimbot.log_misses_due_to_spread");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Aimbot Cover", "Aimbot");
	}
	ImGui::NextColumn(); {

		InsertGroupBoxRight("Other", 506.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Remove recoil", cfg.ragebot.other.no_recoil); dmt("cfg.ragebot.other.no_recoil");
			InsertCombo("Automatic stop", cfg.ragebot.other.automatic_stop, automatic_stop); dmt("cfg.ragebot.other.automatic_stop");
			InsertCheckbox("Stop between shots", cfg.ragebot.other.stop_between_shots); dmt("cfg.ragebot.other.stop_between_shots");
			InsertCheckbox("Anti-aim correction", cfg.ragebot.other.anti_aim_correction); dmt("cfg.ragebot.other.anti_aim_correction");
			InsertKeyBindStyle("##anti_aim_correction_override_key", cfg.ragebot.other.anti_aim_correction_override_key, cfg.ragebot.other.anti_aim_correction_override_key_style); dmt("cfg.ragebot.other.anti_aim_correction_override_key | cfg.ragebot.other.anti_aim_correction_override_key_style");
			InsertMultiCombo("Body aim conditions", cfg.ragebot.other.body_aim_conditions, body_aim_conditions, multiselect_comboboxes_previews[1]); dmt("cfg.ragebot.other.body_aim_conditions");
			if (cfg.ragebot.other.body_aim_conditions[2])
			{
				InsertSliderWithoutText("##x_missed_shots", cfg.ragebot.other.x_missed_shots, 1.f, 5.f, "%1.f missed shots%"); dmt("cfg.ragebot.other.x_missed_shots");
			}
			if (cfg.ragebot.other.body_aim_conditions[3])
			{
				InsertSliderWithoutText("##x_hp", cfg.ragebot.other.x_hp, 1.f, 100.f, "%1.f hp%"); dmt("cfg.ragebot.other.x_hp");
			}
			if (cfg.ragebot.other.body_aim_conditions[4])
			{
				InsertSliderWithoutText("##x_dmg", cfg.ragebot.other.x_dmg, 1.f, 100.f, "%1.f dmg%"); dmt("cfg.ragebot.other.x_dmg");
			}
			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::Text("Force body aim");
			InsertKeyBindStyle("##force_body_aim_key", cfg.ragebot.other.force_body_aim_key, cfg.ragebot.other.force_body_aim_key_style); dmt("cfg.ragebot.other.force_body_aim_key | cfg.ragebot.other.force_body_aim_key_style");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Anti_aimbot() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Anti-aimbot angles", 506.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Enabled", cfg.anti_aimbot.anti_aimbot_angles.enabled); dmt("cfg.anti_aimbot.anti_aimbot_angles.enabled");
			InsertSlider("Pitch", cfg.anti_aimbot.anti_aimbot_angles.pitch, -89.f, 89.f, "%1.f∞%"); dmt("cfg.anti_aimbot.anti_aimbot_angles.pitch");
			InsertCombo("Yaw base", cfg.anti_aimbot.anti_aimbot_angles.yaw_base, yaw_base); dmt("cfg.anti_aimbot.anti_aimbot_angles.yaw_base");
			InsertSlider("Yaw", cfg.anti_aimbot.anti_aimbot_angles.yaw, -180.f, 180.f, "%1.f∞%"); dmt("cfg.anti_aimbot.anti_aimbot_angles.yaw");
			InsertCombo("Desynchronization animations", cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations, desynchronization_animations); dmt("cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations");
			InsertKeyBindStyle("##invert_key", cfg.anti_aimbot.anti_aimbot_angles.invert_key, cfg.anti_aimbot.anti_aimbot_angles.invert_key_style); dmt("cfg.anti_aimbot.anti_aimbot_angles.invert_key | cfg.anti_aimbot.anti_aimbot_angles.invert_key_style");
			if (cfg.anti_aimbot.anti_aimbot_angles.desynchronization_animations)
			{
				InsertSlider("Maximum delta", cfg.anti_aimbot.anti_aimbot_angles.maximum_delta, 0.f, 60.f, "%1.f∞%"); dmt("cfg.anti_aimbot.anti_aimbot_angles.maximum_delta");
				InsertSlider("Body lean", cfg.anti_aimbot.anti_aimbot_angles.body_lean, -180.f, 180.f, "%1.f∞%"); dmt("cfg.anti_aimbot.anti_aimbot_angles.body_lean");
				InsertSlider("Inverted body lean", cfg.anti_aimbot.anti_aimbot_angles.inverted_body_lean, -180.f, 180.f, "%1.f∞%"); dmt("cfg.anti_aimbot.anti_aimbot_angles.inverted_body_lean");
			}
			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Anti-aimbot angles Cover", "Anti-aimbot angles");
	}
	ImGui::NextColumn(); {
		InsertGroupBoxRight("Fake lag", 331.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Enabled", cfg.anti_aimbot.fake_lag.enabled); dmt("cfg.anti_aimbot.fake_lag.enabled");
			InsertSlider("Limit when stand", cfg.anti_aimbot.fake_lag.limit_when_stand, 1.f, 14.f, "%1.f ticks%"); dmt("cfg.anti_aimbot.fake_lag.when_stand");
			InsertSlider("Limit when move", cfg.anti_aimbot.fake_lag.limit_when_move, 1.f, 14.f, "%1.f ticks%"); dmt("cfg.anti_aimbot.fake_lag.when_move");
			InsertSlider("Limit when in air", cfg.anti_aimbot.fake_lag.limit_when_in_air, 1.f, 14.f, "%1.f ticks%"); dmt("cfg.anti_aimbot.fake_lag.when_in_air");
			InsertSlider("Limit when slowmotion", cfg.anti_aimbot.fake_lag.limit_when_slowmotion, 1.f, 14.f, "%1.f ticks%"); dmt("cfg.anti_aimbot.fake_lag.when_slowmotion");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Fake lag Cover", "Fake lag");

		InsertSpacer("Fake lag - Other Spacer");

		InsertGroupBoxRight("Other", 157.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Legitbot() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	InsertGroupBoxTop("Weapon Selection", ImVec2(535.f, 61.f)); {
		ImGui::Columns(6, nullptr, false);
		ImGui::PopFont();
		ImGui::PushFont(menu_globals::weaponFont);
		style->ButtonTextAlign = ImVec2(0.5f, 0.75f);
		{
			if (ImGui::TabButton("G", ImVec2(80, 45)))
				m_nActiveLegitTab = 0;

			ImGui::NextColumn();
			{
				if (ImGui::TabButton("P", ImVec2(80, 45)))
					m_nActiveLegitTab = 1;
			}
			ImGui::NextColumn();
			{
				if (ImGui::TabButton("W", ImVec2(80, 45)))
					m_nActiveLegitTab = 2;
			}
			ImGui::NextColumn();
			{
				if (ImGui::TabButton("d", ImVec2(80, 45)))
					m_nActiveLegitTab = 3;
			}
			ImGui::NextColumn();
			{
				if (ImGui::TabButton("f", ImVec2(80, 45)))
					m_nActiveLegitTab = 4;
			}
			ImGui::NextColumn();
			{
				if (ImGui::TabButton("a", ImVec2(80, 45)))
					m_nActiveLegitTab = 5;
			}
		}
	} InsertEndGroupBoxTop("Weapon Selection Cover", "Weapon Selection", ImVec2(536.f, 11.f));

	InsertSpacer("Weapon Selection - Main Group boxes Spacer");

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Aimbot", 427.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);



			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Aimbot Cover", "Aimbot");
	}
	ImGui::NextColumn(); {

		InsertGroupBoxRight("Triggerbot", 277.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);



			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Triggerbot Cover", "Triggerbot");

		InsertSpacer("Triggerbot - Other Spacer");

		InsertGroupBoxRight("Other", 132.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);



			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Visuals() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Player ESP", 331.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);
			InsertCheckbox("Teammates", cfg.visuals.player_esp.teammates); dmt("cfg.visuals.player_esp.bounding_box");
			InsertCheckbox("Dormant", cfg.visuals.player_esp.dormant); dmt("cfg.visuals.player_esp.bounding_box");
			InsertCheckbox("Bounding box", cfg.visuals.player_esp.bounding_box); dmt("cfg.visuals.player_esp.bounding_box");
			InsertColorPicker("##bounding_box_color", cfg.visuals.player_esp.bounding_box_color, false); dmt("cfg.visuals.player_esp.bounding_box_color");
			InsertCheckbox("Health bar", cfg.visuals.player_esp.health_bar); dmt("cfg.visuals.player_esp.health_bar");
			InsertCheckbox("Name", cfg.visuals.player_esp.name); dmt("cfg.visuals.player_esp.name");
			InsertColorPicker("##name_color", cfg.visuals.player_esp.name_color, false); dmt("cfg.visuals.player_esp.name_color");
			InsertCheckbox("Glow", cfg.visuals.player_esp.glow); dmt("cfg.visuals.player_esp.glow");
			InsertColorPicker("##glow_color", cfg.visuals.player_esp.glow_color, true); dmt("cfg.visuals.player_esp.glow_color");
			InsertCheckbox("Hit marker", cfg.visuals.player_esp.hit_marker); dmt("cfg.visuals.player_esp.hit_marker");
			InsertCheckbox("Hit marker sound", cfg.visuals.player_esp.hit_marker_sound); dmt("cfg.visuals.player_esp.hit_marker_sound");
			InsertCheckbox("Out of FOV arrow", cfg.visuals.player_esp.out_of_fov_arrow); dmt("cfg.visuals.player_esp.out_of_fov_arrow");
			InsertColorPicker("##out_of_fov_arrow_color", cfg.visuals.player_esp.out_of_fov_arrow_color, false); dmt("cfg.visuals.player_esp.out_of_fov_arrow_color");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Player ESP Cover", "Player ESP");

		InsertSpacer("Player ESP - Colored models Spacer");

		InsertGroupBoxLeft("Colored models", 157.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Player", cfg.visuals.colored_models.player); dmt("cfg.visuals.colored_models.player");
			InsertColorPicker("##player_color", cfg.visuals.colored_models.player_color, true); dmt("cfg.visuals.player_esp.bounding_box_color");
			if (cfg.visuals.colored_models.player)
			{
				InsertCheckbox("Player behind wall", cfg.visuals.colored_models.player_behind_wall); dmt("cfg.visuals.colored_models.player_behind_wall");
				InsertColorPicker("##player_behind_wall_color", cfg.visuals.colored_models.player_behind_wall_color, true); dmt("cfg.visuals.colored_models.player_behind_wall_color");
				InsertComboWithoutText("##player_type", cfg.visuals.colored_models.player_type, player_type); dmt("cfg.visuals.colored_models.player_type");
			}

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Colored models Cover", "Colored models");
	}
	ImGui::NextColumn(); {
		InsertGroupBoxRight("Other ESP", 199.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertCheckbox("Radar", cfg.visuals.other_esp.radar); dmt("cfg.visuals.other_esp.radar");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Other ESP Cover", "Other ESP");

		InsertSpacer("Other ESP - Effects Spacer");

		InsertGroupBoxRight("Effects", 289.f); {

			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertSlider("Flash alpha limit", cfg.visuals.effects.flash_alpha_limit, 0.f, 255.f, "%1.f%"); dmt("cfg.visuals.effects.flash_alpha_limit");
			InsertCheckbox("Remove smoke effects", cfg.visuals.effects.remove_smoke_effects); dmt("cfg.visuals.effects.remove_smoke_effects");
			InsertCombo("Brightness adjustment", cfg.visuals.effects.brightness_adjustment, brightness_adjustment); dmt("cfg.visuals.effects.brightness_adjustment");
			InsertCheckbox("Remove scope overlay", cfg.visuals.effects.remove_scope_overlay); dmt("cfg.visuals.effects.remove_scope_overlay");
			// че эта хуйн€ в ските делает € ниху€ не помню.
			//InsertCheckbox("Instant scope", cfg.visuals.effects.instant_scope); dmt("cfg.visuals.effects.instant_scope");
			InsertCheckbox("Disable post processing", cfg.visuals.effects.disable_post_processing); dmt("cfg.visuals.effects.disable_post_processing");
			InsertCheckbox("Force third person (alive)", cfg.visuals.effects.force_third_person_alive); dmt("cfg.visuals.effects.force_third_person_alive");
			InsertKeyBindStyle("##third_person_key", cfg.visuals.effects.third_person_key, cfg.visuals.effects.third_person_key_style); dmt("cfg.visuals.effects.third_person_key | cfg.visuals.effects.third_person_key_style");
			InsertCheckbox("Force third person (dead)", cfg.visuals.effects.force_third_person_dead); dmt("cfg.visuals.effects.force_third_person_dead");
			// лень ща делать хуйню эту хз.
			//InsertCheckbox("Disable rendering of teammates", cfg.visuals.effects.disable_rendering_of_teammates); dmt("cfg.visuals.effects.disable_rendering_of_teammates");
			InsertCheckbox("Bullet tracers", cfg.visuals.effects.bullet_tracers); dmt("cfg.visuals.effects.bullet_tracers");
			InsertColorPicker("##bullet_tracers_color", cfg.visuals.effects.bullet_tracers_color, false); dmt("cfg.visuals.effects.bullet_tracers_color");
			InsertCheckbox("Bullet impacts", cfg.visuals.effects.bullet_impacts); dmt("cfg.visuals.effects.bullet_impacts");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Effects Cover", "Effects");
	}
}

void Menu::Misc() {
	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Miscellaneous", 506.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			InsertSlider("Override FOV", cfg.misc.miscellaneous.override_fov, 0.f, 150.f, "%1.f∞%"); dmt("cfg.misc.miscellaneous.override_fov");
			InsertCheckbox("Bunny hop", cfg.misc.miscellaneous.bunny_hop); dmt("cfg.misc.miscellaneous.bunny_hop");
			InsertCheckbox("Air strafe", cfg.misc.miscellaneous.air_strafe); dmt("cfg.misc.miscellaneous.air_strafe");
			InsertCombo("Air duck", cfg.misc.miscellaneous.air_duck, air_duck); dmt("cfg.misc.miscellaneous.air_duck");
			InsertCheckbox("Auto-accept matchmaking", cfg.misc.miscellaneous.auto_accept_matchmaking); dmt("cfg.misc.miscellaneous.auto_accept_matchmaking");
			InsertCheckbox("Clan tag spammer", cfg.misc.miscellaneous.clan_tag_spammer); dmt("cfg.misc.miscellaneous.clan_tag_spammer");
			InsertCheckbox("Log weapon purchases", cfg.misc.miscellaneous.log_weapon_puchases); dmt("cfg.misc.miscellaneous.log_weapon_puchases");
			InsertCheckbox("Log damage dealt", cfg.misc.miscellaneous.log_damage_dealt); dmt("cfg.misc.miscellaneous.log_damage_dealt");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxLeft("Miscellaneous Cover", "Miscellaneous");
	}
	ImGui::NextColumn(); {
		InsertGroupBoxRight("Settings", 156.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			ImGui::CustomSpacing(9.f);

			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::Text("Menu key"); 
			InsertKeyBind("##menu_key", cfg.misc.settings.menu_key); dmt("cfg.misc.settings.menu_key");
			ImGui::Spacing(); ImGui::NewLine(); ImGui::SameLine(42.f);
			ImGui::Text("Theme color");
			InsertColorPicker("##theme_color", cfg.misc.settings.theme_color, false); dmt("cfg.misc.settings.theme_color");

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);

		} InsertEndGroupBoxRight("Settings Cover", "Settings");

		InsertSpacer("Settings - Other Spacer");

		InsertGroupBoxRight("Other", 332.f); {
			style->ItemSpacing = ImVec2(4, 2);
			style->WindowPadding = ImVec2(4, 4);
			style->ButtonTextAlign = ImVec2(0.5f, -2.0f);
			ImGui::CustomSpacing(9.f);

			static int current_preset;

			InsertCombo("Presets", current_preset, presets);

			ImGui::CustomSpacing(6.f); ImGui::NewLine(); ImGui::SameLine(42.f); 

			// ставим трушную длину дл€ ебаных кнопок.
			ImGui::PushItemWidth(158.f);

			// логи дл€ кфг по приколу добавил, хз нахуй они нужны но пусть будут.
			_(skeet_idb, "[skeet.idb] ");
			_(sucsessfuly_loaded, "%s.cfg sucsessfuly loaded\n");
			_(sucsessfuly_saved, "%s.cfg sucsessfuly saved\n");
			_(error_when_loading, "Error when loading %s.cfg\n");
			_(error_when_saveing, "Error when saving %s.cfg\n");
			if (ImGui::Button("Load config"))
			{
				if (cfg.Config.Load(presets[current_preset]))
				{
					g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
					util::print_dev_console(true, Color(255, 255, 255, 255), sucsessfuly_loaded, presets[current_preset]);
				}
				else
				{
					g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
					util::print_dev_console(true, Color(255, 255, 255, 255), error_when_loading, presets[current_preset]);
				}
			}

			ImGui::CustomSpacing(6.f); ImGui::NewLine(); ImGui::SameLine(42.f);
			if (ImGui::Button("Save config"))
			{
				cfg.Config.CreateConfig(presets[current_preset]);
				if (cfg.Config.Save(presets[current_preset]))
				{
					g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
					util::print_dev_console(true, Color(255, 255, 255, 255), sucsessfuly_saved, presets[current_preset]);
				}
				else
				{
					g_pCVar->ConsoleColorPrintf(cfg.misc.settings.theme_color, skeet_idb);
					util::print_dev_console(true, Color(255, 255, 255, 255), error_when_saveing, presets[current_preset]);
				}
			}

			ImGui::CustomSpacing(6.f); ImGui::NewLine(); ImGui::SameLine(42.f);
			if (ImGui::Button("Open configs folder"))
				ShellExecute(0, "open", "C:/skeet.idb/Configs", NULL, NULL, SW_NORMAL);

			// возвращаем все как было.
			ImGui::PopItemWidth();

			style->ItemSpacing = ImVec2(0, 0);
			style->WindowPadding = ImVec2(6, 6);
		} InsertEndGroupBoxRight("Other Cover", "Other");
	}
}

void Menu::Skins() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {
		InsertGroupBoxLeft("Knife options", 112.f); {



		} InsertEndGroupBoxLeft("Knife options Cover", "Knife options");

		InsertSpacer("Fake lag - Other Spacer");

		InsertGroupBoxLeft("Glove options", 376.f); {



		} InsertEndGroupBoxLeft("Glove options Cover", "Glove options");
	}
	ImGui::NextColumn(); {

		InsertGroupBoxRight("Weapon skin", 506.f); {


		} InsertEndGroupBoxRight("Weapon skin Cover", "Weapon skin");
	}
}

void Menu::Players() {

	ImGuiStyle* style = &ImGui::GetStyle();
	InsertSpacer("Top Spacer");

	ImGui::Columns(2, NULL, false); {

		InsertGroupBoxLeft("Players", 506.f); {


		} InsertEndGroupBoxLeft("Players Cover", "Players");
	}
	ImGui::NextColumn(); {

		InsertGroupBoxRight("Adjustments", 506.f); {


		} InsertEndGroupBoxRight("Adjustments Cover", "Adjustments");
	}
}