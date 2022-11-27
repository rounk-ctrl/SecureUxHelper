#include "helper.h"

#define FLG_APPLICATION_VERIFIER (0x100)
static const wchar_t kPatcherDllName[] = L"SecureUxTheme.dll";
static constexpr wchar_t kIFEO[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\";
static constexpr wchar_t kCurrentColorsPath[] = L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\";
static constexpr wchar_t kCurrentColorsName[] = L"DefaultColors";
static constexpr wchar_t kCurrentColorsBackup[] = L"DefaultColors_backup";

DWORD InstallForExecutable(const wchar_t* executable)
{
	const auto subkey = std::wstring{ ESTRt(kIFEO) } + executable;
	DWORD GlobalFlag = 0;
	DWORD GlobalFlag_size = sizeof(GlobalFlag);
	// we don't care if it fails
	RegGetValueW(
		HKEY_LOCAL_MACHINE,
		subkey.c_str(),
		ESTRt(L"GlobalFlag"),
		RRF_RT_REG_DWORD | RRF_ZEROONFAILURE,
		nullptr,
		&GlobalFlag,
		&GlobalFlag_size
	);
	GlobalFlag |= FLG_APPLICATION_VERIFIER;
	auto ret = RegSetKeyValueW(
		HKEY_LOCAL_MACHINE,
		subkey.c_str(),
		ESTRt(L"GlobalFlag"),
		REG_DWORD,
		&GlobalFlag,
		sizeof(GlobalFlag)
	);
	if (!ret)
	{
		ret = RegSetKeyValueW(
			HKEY_LOCAL_MACHINE,
			subkey.c_str(),
			ESTRt(L"VerifierDlls"),
			REG_SZ,
			kPatcherDllName,
			sizeof(kPatcherDllName)
		);
	}
	return ret;
}
wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

//std::string userpath = std::string(getenv("WINDIR")) + "\\Rectify11\\files\\";
BOOL Install()
{
	auto ret = InstallForExecutable(ESTRt(L"winlogon.exe"));
}
BOOL ApplyTheme(LPCWSTR theme)
{
	int iCount = 0;
	g_pThemeManager2->GetThemeCount(&iCount);
	std::vector<std::wstring> ok;
	for (auto i = 0; i < iCount; ++i)
	{
		ITheme* pTheme = nullptr;
		g_pThemeManager2->GetTheme(i, &pTheme);

		const std::wstring name = pTheme->GetDisplayName();
		ok.push_back(name);
	}
	int index = std::distance(ok.begin(), std::find(ok.begin(), ok.end(), theme));
	auto result = g_pThemeManager2->SetCurrentTheme(
		0,
		index,
		1,
		(THEME_APPLY_FLAGS)0,
		(THEMEPACK_FLAGS)0
	);
}
BOOL ThemeInfo(LPCWSTR ok)
{
	bool patched = false;
	bool val = sig::check_file(ok);
	if (val && val == E_FAIL)
		patched = false;
	else
		patched = true;
	std::wcout << "\n";
	std::wcout << ok << "=> Patched: " << patched;
	std::wcout << "\n";
	return patched;
}
BOOL ThemePatch(LPCWSTR ok)
{
	const auto val = sig::fix_file(ok);
	if (SUCCEEDED(val))
	{
		std::wcout << "\n";
		std::wcout << "Patched " << ok;
		std::wcout << "\n";
	}
	else
	{
		std::wcout << "\n";
		std::wcout << "Error patching " << ok << " (" << val << ")";
		std::wcout << "\n";
	}
	return val;
}
DWORD RenameDefaultColors()
{
	const auto old_name = std::wstring{ ESTRt(kCurrentColorsPath) } + ESTRt(kCurrentColorsName);
	return utl::rename_key(old_name.c_str(), ESTRt(kCurrentColorsBackup));
}

DWORD RestoreDefaultColors()
{
	const auto old_name = std::wstring{ ESTRt(kCurrentColorsPath) } + ESTRt(kCurrentColorsBackup);
	return utl::rename_key(old_name.c_str(), ESTRt(kCurrentColorsName));
}
