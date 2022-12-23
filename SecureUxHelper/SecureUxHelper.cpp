// SecureUxHelper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

std::pair<LPCVOID, SIZE_T> get_resource(HMODULE mod, WORD type, WORD id)
{
	const auto rc = FindResource(
		mod,
		MAKEINTRESOURCE(id),
		MAKEINTRESOURCE(type)
	);
	if (!rc)
		return { nullptr, 0 };
	const auto rc_data = LoadResource(mod, rc);
	const auto size = SizeofResource(mod, rc);
	if (!rc_data)
		return { nullptr, 0 };
	const auto data = static_cast<const void*>(LockResource(rc_data));
	return { data, size };
}

void bind_resource_to_arch(HMODULE mod, WORD resid, WORD arch)
{
	const auto res = get_resource(mod, 256, resid);
	secureuxtheme_set_dll_for_arch(res.first, res.second, arch);
}

void do_init(HMODULE mod)
{
	bind_resource_to_arch(mod, IDR_SECUREUXTHEME_DLL_X86, IMAGE_FILE_MACHINE_I386);
	bind_resource_to_arch(mod, IDR_SECUREUXTHEME_DLL_X64, IMAGE_FILE_MACHINE_AMD64);
	bind_resource_to_arch(mod, IDR_SECUREUXTHEME_DLL_ARM64, IMAGE_FILE_MACHINE_ARM64);
}
const HMODULE GetCurrentModule()
{
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	::VirtualQuery(GetCurrentModule, &mbi, sizeof(mbi));

	return reinterpret_cast<HMODULE>(mbi.AllocationBase);
}
int _tmain(int argc, TCHAR* argv[])
{
	do_init(GetCurrentModule());
	themetool_init();
	for (int i = 1; i < argc; i++)
	{
		if (!wcscmp(argv[i], L"info"))
		{
			LPCWSTR ok = argv[2];
			HRESULT result = themetool_signature_check(ok);
			if (result == E_FAIL)
			{
				std::wcout << "\n" << ok << ": " << "Not Patched\n";
			}
			else if (result == S_OK)
			{
				std::wcout << "\n" << ok << ": " << "Patched\n";
			}
		}
		else if (!wcscmp(argv[i], L"patch"))
		{
			LPCWSTR ok = argv[2];
			const auto fix_result = themetool_signature_fix(ok);
			if (!SUCCEEDED(fix_result))
			{
				std::wcout << "\nPatching " << ok << ": " << "Error " << fix_result << "\n";
			}
			else
			{
				std::wcout << "\nPatching " << ok << ": " << "success\n";
			}
		}
		else if (!wcscmp(argv[i], L"install"))
		{
			ULONG install_flags{};
			install_flags |= SECUREUXTHEME_INSTALL_HOOK_LOGONUI;
			install_flags |= SECUREUXTHEME_INSTALL_RENAME_DEFAULTCOLORS;
			const auto ok = secureuxtheme_install(install_flags);
			std::wcout << ok;
		}
		else if (!wcscmp(argv[i], L"apply"))
		{
			LPCWSTR ok = argv[2];
			ULONG theme_count{};
			themetool_get_theme_count(&theme_count);
			for (auto i = 0u; i < theme_count; ++i)
			{
				ITheme* theme{};
				themetool_get_theme(i, &theme);
				wchar_t path[256]{};
				themetool_theme_get_display_name(theme, path, std::size(path));
				std::wstring style = std::wstring(path);
				std::wstring okstr = std::wstring(ok);
				std::transform(style.begin(), style.end(), style.begin(),
					[](unsigned char c) { return std::tolower(c); });

				std::transform(okstr.begin(), okstr.end(), okstr.begin(),
					[](unsigned char c) { return std::tolower(c); });
				if (!wcscmp(style.c_str(), okstr.c_str()))
				{
					wchar_t name[256]{};
					themetool_theme_get_vs_path(theme, name, std::size(name));
					std::wcout << "\nFound " << style;
					std::wcout << "\nApplying " << name;
					HRESULT result = themetool_set_active(0, i, TRUE, 0, 0);

				}
				themetool_theme_release(theme);
			}
		}
	}
}