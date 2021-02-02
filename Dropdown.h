#pragma once
std::string multiselect_comboboxes_previews[2];

std::vector<const char*> target_selection = {
	"Distance",
	"Crosshair",
	"Damage",
	"Health",
	"Lag",
	"Height"
};

std::unordered_map<int, bool> old_target_hitbox_ib;
std::unordered_map<int, bool> target_hitbox_ib;
std::vector<const char*> target_hitbox = {
	"Head",
	"Chest",
	"Stomach",
	"Arms",
	"Legs",
	"Feet"
};

std::unordered_map<int, bool> old_body_aim_conditions_ib;
std::unordered_map<int, bool> body_aim_conditions_ib;
std::vector<const char*> body_aim_conditions = {
	"Lethal",
	"In air",
	"After x missed shots",
	"If hp <= x",
	"If damage > x"
};

std::vector<const char*> automatic_stop = {
	"Off",
	"Full",
	"Adaptive"
};

std::vector<const char*> yaw_base = {
	"Local view",
	"Crosshair"
};

std::vector<const char*> desynchronization_animations = {
	"Off",
	"Static",
	"Opposite"
};

std::vector<const char*> player_type = {
	"Default",
	"Flat"
};

std::vector<const char*> brightness_adjustment = {
	"Off",
	"Nightmode"
};

std::vector<const char*> air_duck = {
	"Off",
	"Default"
};

std::vector<const char*> presets = {
	"Legit",
	"Rage",
	"HvH",
	"Secret",
	"Headshot",
	"Alpha",
	"Bravo",
	"Charlie",
	"Delta",
	"Echo",
	"Foxtrot",
	"Golf",
	"Hotel",
	"India",
	"Juliet",
	"Kilo"
};