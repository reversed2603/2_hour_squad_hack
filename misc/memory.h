#pragma once
#include <cstdint>


auto w2s_sig = g_globals.sig_scan(0, _("48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 48 81 EC ? ? ? ? 41 0F ? ? 49 8B ? 48 8B"));
auto screen_sig = g_globals.sig_scan(0, _("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 C0 49 8B F8 89 02 48 8B F2 41 89 00 48 8B 99"));

typedef bool(__thiscall* w2s_fn)(uintptr_t, vec3, vec2*, bool);
w2s_fn m_ProjectWorldLocationToScreens;

typedef void(__thiscall* viewport_fn)(uintptr_t, int32_t&, int32_t&);
viewport_fn m_GetViewPortSize;

typedef FMatrix*(__thiscall* get_bonem_fn)(uintptr_t, FMatrix*, int32_t);
get_bonem_fn m_get_bone_matrix;


typedef bool (__thiscall* this_LineOfSign)(uintptr_t, void*, vec3*);
this_LineOfSign m_LineOfSign;

bool lineofsight(uintptr_t actorpawn)
{
	auto Base = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

	vec3 rot = { 0,0,0 }; // lineofsight
	return reinterpret_cast<bool(__fastcall*)(uintptr_t, uintptr_t, vec3*)>(Base + 0x4092a30)(mem_data::pointers::local_player_controller, actorpawn, &rot);
}

bool w2s( vec3 pos, vec2* screen, bool bPlayerViewportRelative )
{
	w2s_fn m_projworld = reinterpret_cast< w2s_fn >( ( DWORD64 )w2s_sig );
	return m_projworld(mem_data::pointers::local_player_controller, pos, screen, bPlayerViewportRelative);
}

FMatrix* get_bone_matrix(uintptr_t mesh, FMatrix* matrix, int32_t boneid)
{
	get_bonem_fn m_get_bone_matrix = reinterpret_cast< get_bonem_fn >( mem_data::base_handle + 0x39DF780 );
	return m_get_bone_matrix(mesh, matrix, boneid);
}

vec3 get_bone_location( uintptr_t mesh, int32_t boneid )
{
	FMatrix temp{};
	vec3 ret{};
	get_bone_matrix(mesh, &temp, boneid);
	ret.x = temp.WPlane.x;
	ret.y = temp.WPlane.y;
	ret.z = temp.WPlane.z;
	return ret;
}

void get_bone_location( uintptr_t mesh, vec3& ret_value, int32_t boneid )
{
	FMatrix temp{};
	get_bone_matrix(mesh, &temp, boneid);
	ret_value.x = temp.WPlane.x;
	ret_value.y = temp.WPlane.y;
	ret_value.z = temp.WPlane.z;
}

void get_screen_size( uintptr_t playerController, int32_t& ScreenWidth, int32_t& ScreenHeight) {
	viewport_fn m_getviewportsize = reinterpret_cast< viewport_fn >( ( DWORD64 )screen_sig );
	return m_getviewportsize( playerController, ScreenWidth, ScreenHeight );
}