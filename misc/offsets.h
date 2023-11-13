#pragma once
#include <minwindef.h>

namespace mem_data {
	DWORD_PTR base_handle;
	namespace offsets {
		DWORD g_name = 0x6e79600;
		DWORD g_world = 0x6FFA2F0;

		DWORD camera_manager = 0x2c0;
		DWORD persistent_level = 0x30;
		DWORD game_instance = 0x180;
		DWORD local_players_array = 0x38;
		DWORD player_controller = 0x30;
		DWORD apawn = 0x2a8;
		DWORD root_component = 0x138;
		DWORD instigator_actor = 0x120;
		DWORD actor_array = 0x98;
		DWORD actor_count = 0xa0;
		DWORD actor_id = 0x18;
		DWORD player_teamid = 0x3d8;
		DWORD player_state = 0x248;
		DWORD actor_mesh = 0x288;
		DWORD actor_health = 0x1c98;
		DWORD relative_location = 0x11c;
	}
	namespace pointers {
		DWORD_PTR game_instance;
		DWORD_PTR g_name;
		DWORD_PTR local_player_array;
		DWORD_PTR local_player;
		DWORD_PTR local_player_controller;
		DWORD_PTR local_player_pawn;
		DWORD_PTR local_player_camera;
		DWORD_PTR local_player_root;
		DWORD_PTR local_player_state;
		DWORD_PTR persistent_level;
		DWORD_PTR actors;
		DWORD_PTR g_world;
	}
}

enum bones : uint32_t
{
	ZERO = 0,
	HEAD = 48,//Ó^
	UPPER_NECK = 7,
	NECK = 6,//≤±◊”
	RIGHT_SHOULDER = 66,//ºÁ∞Ú
	RIGHT_ELBOW = 89,//÷‚
	RIGHT_HAND = 124,// ÷
	LEFT_SHOULDER = 93,
	LEFT_ELBOW = 94,
	LEFT_HAND = 123,
	PELVIS = 2,//π«≈Ë
	STOMACH = 4,//Œ∏
	CHEST = 5,//–ÿ
	RIGHT_THIGH = 130,//¥ÛÕ»
	RIGHT_KNEE = 131,
	RIGHT_FOOT = 140,
	LEFT_THIGH = 125,
	LEFT_KNEE = 126,
	LEFT_FOOT = 139,
	ROOT = 199,
};

struct FPlane
{
	float x, y, z;
	float  w;                                                         // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)

};
struct FMatrix 
{
	struct FPlane                                      XPlane;                                                    // 0x0000(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FPlane                                      YPlane;                                                    // 0x0010(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FPlane                                      ZPlane;                                                    // 0x0020(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct FPlane                                      WPlane;                                                    // 0x0030(0x0010) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};