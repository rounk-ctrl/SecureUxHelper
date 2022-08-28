// thing.cpp : Defines the entry point for the console application.
// https://github.com/namazso/SecureUxTheme but console edition ig.
//

#include "stdafx.h"

int _tmain(int argc, TCHAR* argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if(!wcscmp(argv[i], L"info"))
		{
			LPCWSTR ok = argv[2];
			ThemeInfo(ok);
		}
		else if (!wcscmp(argv[i], L"patch"))
		{
			LPCWSTR ok = argv[2];
			ThemePatch(ok);
		}
	}
}

