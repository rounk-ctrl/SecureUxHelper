#pragma once
#include "stdafx.h"
namespace utl {
	DWORD open_key(PHKEY handle, const wchar_t* path, ULONG desired_access);
	DWORD rename_key(const wchar_t* old_path, const wchar_t* new_path);
}
