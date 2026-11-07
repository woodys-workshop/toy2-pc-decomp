#include "Common.h"

#include <windows.h>

#define VWIN32_DIOC_DOS_IOCTL 1

struct DIOC_REGISTERS
{
	DWORD reg_EBX;
	DWORD reg_EDX;
	DWORD reg_ECX;
	DWORD reg_EAX;
	DWORD reg_EDI;
	DWORD reg_ESI;
	DWORD reg_Flags;
};

#define CARRY_FLAG 0x0001

struct PARAMBLOCK
{
	BYTE bOperation;
	BYTE bNumLocks;
};

STATIC_ASSERT(sizeof(DIOC_REGISTERS) == 0x1C);
STATIC_ASSERT(sizeof(PARAMBLOCK) == 0x2);

namespace VWin32
{
	// GLOBAL: TOY2 0x00882E2C
	HANDLE g_hVWin32;
}

namespace VWin32
{
	// FUNCTION: TOY2 0x004A6DB0 [MATCHED]
	BOOL WINAPI LockLogicalVolume(HANDLE hVWin32, BYTE bDriveNum, BYTE bLockLevel, WORD wPermissions)
	{
		BOOL fResult;
		DIOC_REGISTERS regs = { 0 };
		BYTE bDeviceCat;
		DWORD cb;

		bDeviceCat = 0x48;

	LBL_ATTEMPT_AGAIN:
		regs.reg_EAX = 0x440D;
		regs.reg_EBX = MAKEWORD(bDriveNum, bLockLevel);
		regs.reg_ECX = MAKEWORD(0x4A, bDeviceCat);
		regs.reg_EDX = wPermissions;

		fResult = DeviceIoControl(hVWin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0);

		fResult = fResult && ! (regs.reg_Flags & CARRY_FLAG);

		if (! fResult && (bDeviceCat != 0x08))
		{
			bDeviceCat = 0x08;
			goto LBL_ATTEMPT_AGAIN;
		}

		return fResult;
	}

	// FUNCTION: TOY2 0x004A6E50 [MATCHED]
	BOOL WINAPI UnlockLogicalVolume(HANDLE hVWin32, BYTE bDriveNum)
	{
		BOOL fResult;
		DIOC_REGISTERS regs = { 0 };
		BYTE bDeviceCat;
		DWORD cb;

		bDeviceCat = 0x48;

	LBL_ATTEMPT_AGAIN:
		regs.reg_EAX = 0x440D;
		regs.reg_EBX = bDriveNum;
		regs.reg_ECX = MAKEWORD(0x6A, bDeviceCat);

		fResult = DeviceIoControl(hVWin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0);

		fResult = fResult && ! (regs.reg_Flags & CARRY_FLAG);

		if (! fResult && (bDeviceCat != 0x08))
		{
			bDeviceCat = 0x08;
			goto LBL_ATTEMPT_AGAIN;
		}
		return fResult;
	}

	// FUNCTION: TOY2 0x004A6EE0 [MATCHED]
	BOOL UnlockMedia(HANDLE hVWin32, BYTE bDrive)
	{
		DIOC_REGISTERS regs = { 0 };
		PARAMBLOCK unlockParams = { 0 };
		BOOL fResult;
		DWORD cb;

		unlockParams.bOperation = 1;

		regs.reg_EAX = 0x440D;
		regs.reg_EBX = bDrive;
		regs.reg_ECX = MAKEWORD(0x48, 0x08);
		regs.reg_EDX = (DWORD)&unlockParams;

		fResult = DeviceIoControl(hVWin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0);

		fResult = fResult && ! (regs.reg_Flags & CARRY_FLAG);
		if (! fResult)
			return FALSE;

		return fResult;
	}

	// FUNCTION: TOY2 0x004A6F60 [MATCHED]
	BOOL LockMedia(HANDLE hVWin32, BYTE bDrive)
	{
		DIOC_REGISTERS regs = { 0 };
		PARAMBLOCK lockParams = { 0 };
		BOOL fResult;
		DWORD cb;

		lockParams.bOperation = 0;

		regs.reg_EAX = 0x440D;
		regs.reg_EBX = bDrive;
		regs.reg_ECX = MAKEWORD(0x48, 0x08);
		regs.reg_EDX = (DWORD)&lockParams;

		fResult = DeviceIoControl(hVWin32, VWIN32_DIOC_DOS_IOCTL, &regs, sizeof(regs), &regs, sizeof(regs), &cb, 0);

		fResult = fResult && ! (regs.reg_Flags & CARRY_FLAG);
		if (! fResult)
			return FALSE;

		return fResult;
	}

	// FUNCTION: TOY2 0x004A6FE0 [MATCHED]
	HANDLE WINAPI OpenVWin32(void) { return CreateFile("\\\\.\\vwin32", 0, 0, 0, 0, FILE_FLAG_DELETE_ON_CLOSE, 0); }

	// FUNCTION: TOY2 0x004A7000 [MATCHED]
	BOOL WINAPI CloseVWin32(HANDLE hVWin32) { return CloseHandle(hVWin32); }

	// FUNCTION: TOY2 0x004A7010
	void UnlockDriveMedia(void)
	{
		UnlockMedia(g_hVWin32, 6);
		CloseVWin32(g_hVWin32);
	}

	// FUNCTION: TOY2 0x004A7080
	void LockDriveMedia(void)
	{
		g_hVWin32 = OpenVWin32();
		if (g_hVWin32 != INVALID_HANDLE_VALUE)
		{
			if (LockMedia(g_hVWin32, 6))
				atexit(UnlockDriveMedia);
			else
				CloseVWin32(g_hVWin32);
		}
	}
}
