#pragma once

#include "stdafx.h"

extern CComPtr<IThemeManager2> g_pThemeManager2;
BOOL ThemeInfo(LPCWSTR ok);
BOOL ThemePatch(LPCWSTR ok);
DWORD RenameDefaultColors();
DWORD RestoreDefaultColors();
