#pragma once
#include <cstdint>
#include <vector>
#include <Windows.h>

class c_globals
{
public:
	bool menu_open = true;
	uintptr_t base_address = NULL;

	uintptr_t sig_scan(const char* module, const char* pattern);
};

extern c_globals g_globals;