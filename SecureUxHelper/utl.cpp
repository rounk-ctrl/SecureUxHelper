#include "utl.h"

EXTERN_C_START

NTSYSAPI
NTSTATUS
NTAPI
NtOpenKey(
	_Out_ PHANDLE KeyHandle,
	_In_  ACCESS_MASK DesiredAccess,
	_In_  POBJECT_ATTRIBUTES ObjectAttributes
);
EXTERN_C_END

DWORD utl::open_key(PHKEY handle, const wchar_t* path, ULONG desired_access)
{
	UNICODE_STRING ustr{};
	RtlInitUnicodeString(&ustr, path);
	OBJECT_ATTRIBUTES attr{};
	InitializeObjectAttributes(
		&attr,
		&ustr,
		OBJ_CASE_INSENSITIVE | OBJ_OPENIF,
		nullptr,
		nullptr
	);
	auto status = NtOpenKey(
		(PHANDLE)handle,
		desired_access | KEY_WOW64_64KEY,
		&attr
	);

	return RtlNtStatusToDosError(status);
}

DWORD utl::rename_key(const wchar_t* old_path, const wchar_t* new_path)
{
	HKEY key{};
	const auto ret = open_key(&key, old_path, KEY_ALL_ACCESS);

	if (ret)
		return ret;

	UNICODE_STRING ustr{};
	RtlInitUnicodeString(&ustr, new_path);
	const auto status = NtRenameKey(
		key,
		&ustr
	);

	NtClose(key);

	return RtlNtStatusToDosError(status);
}