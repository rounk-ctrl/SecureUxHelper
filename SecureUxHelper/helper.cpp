#include "helper.h"

#define FLG_APPLICATION_VERIFIER (0x100)
static const wchar_t kPatcherDllName[] = L"SecureUxTheme.dll";
static const wchar_t kIFEO[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\";
static constexpr wchar_t kCurrentColorsPath[] = L"\\Registry\\Machine\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\";
static constexpr wchar_t kCurrentColorsName[] = L"DefaultColors";
static constexpr wchar_t kCurrentColorsBackup[] = L"DefaultColors_backup";

DWORD InstallForExecutable(const wchar_t* executable)
{
	const auto subkey = std::wstring(kIFEO) + executable;
	DWORD GlobalFlag = 0;
	DWORD GlobalFlag_size = sizeof(GlobalFlag);
	// we don't care if it fails
	RegGetValueW(
		HKEY_LOCAL_MACHINE,
		subkey.c_str(),
		L"GlobalFlag",
		RRF_RT_REG_DWORD | RRF_ZEROONFAILURE,
		nullptr,
		&GlobalFlag,
		&GlobalFlag_size
	);
	GlobalFlag |= FLG_APPLICATION_VERIFIER;
	auto ret = RegSetKeyValueW(
		HKEY_LOCAL_MACHINE,
		subkey.c_str(),
		L"GlobalFlag",
		REG_DWORD,
		&GlobalFlag,
		sizeof(GlobalFlag)
	);
	if (!ret)
	{
		ret = RegSetKeyValueW(
			HKEY_LOCAL_MACHINE,
			subkey.c_str(),
			L"VerifierDlls",
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
BOOL ThemeInfo(LPCWSTR ok)
{
	bool patched = false;
	bool val = sig::check_file(ok);
	if (val && val == 1)
		patched = false;
	else if (val && val == 0)
		patched = true;
	std::wcout << "\n";
	std::wcout << ok << "=> Patched: " << val;
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
