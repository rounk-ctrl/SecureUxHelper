// SecureUxHelper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "themetool.h"
#include "secureuxtheme.h"

int main(int argc, TCHAR* argv[])
{
	themetool_init();
	for (int i = 1; i < argc; i++)
	{
		if (!wcscmp(argv[i], L"info"))
		{

		}
	}
}