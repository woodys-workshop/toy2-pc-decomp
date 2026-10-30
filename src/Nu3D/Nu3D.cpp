#include "Nu3D/Nu3D.h"

#include <WINDOWS.H>
#include <MMSYSTEM.H>
#include <STDDEF.H>

namespace Nu3D
{
	// GLOBAL: TOY2 0x00884470
	int32_t g_isSoftwareRendering;

	// GLOBAL: TOY2 0x00883FF8
	uint8_t g_hasTimeStampCounter;

	// GLOBAL: TOY2 0x00883FF9
	uint8_t g_hasMMX;

	// GLOBAL: TOY2 0x00883FFA
	uint8_t g_has3DNow;

	// GLOBAL: TOY2 0x00883FE0
	int64_t g_cpuTicksPerSecond;

	// GLOBAL: TOY2 0x00884004
	int32_t g_calibrationGetterInitialized;

	// GLOBAL: TOY2 0x00508A68
	int32_t g_useAsDiffuseModulation = 1;

	// GLOBAL: TOY2 0x00B1C3B8
	int32_t g_defaultPrimitiveFlags;

	// FUNCTION: TOY2 0x004B1880
	void SetIsSoftwareRendering(int32_t value) { g_isSoftwareRendering = value; }

	// FUNCTION: TOY2 0x004AB7D0
	void GetCPUFeatures()
	{
		__asm
		{
			pushfd                       ; save EFLAGS
			pop     eax
			mov     ebx, eax            ; ebx = original flags
			xor     eax, 200000h        ; toggle ID bit (bit 21)
			push    eax
			popfd
			pushfd                       ; read flags back
			pop     eax
			sub     eax, ebx            ; did the ID bit stick?
			jz      LBL_DONE            ; no CPUID support -> bail

			mov     eax, 1
			cpuid
			mov     eax, 1              ; al = 1 for the stores below

			test    edx, 10h            ; TSC (bit 4)
			jz      LBL_NO_TSC
			mov     g_hasTimeStampCounter, al

		LBL_NO_TSC:
			test    edx, 800000h        ; MMX (bit 23)
			jz      LBL_NO_MMX
			mov     g_hasMMX, al

		LBL_NO_MMX:
			mov     eax, 80000001h      ; extended features leaf
			cpuid
			test    eax, 80000000h      ; high bit of eax
			jz      LBL_DONE
			mov     eax, 1
			test    edx, 80000000h      ; 3DNow (edx bit 31)
			jz      LBL_DONE
			mov     g_has3DNow, al

		LBL_DONE:
		}
	}

	// FUNCTION: TOY2 0x004AB7C0
	int32_t QueryTimeStampCounter()
	{
		GetCPUFeatures();
		return (int8_t)g_hasTimeStampCounter;
	}

	// FUNCTION: TOY2 0x004AB830
	int32_t QueryMMX()
	{
		GetCPUFeatures();
		return (int8_t)g_hasMMX;
	}

	// FUNCTION: TOY2 0x004AB840
	int32_t Query3DNow()
	{
		GetCPUFeatures();
		return (int8_t)g_has3DNow;
	}

	// FUNCTION: TOY2 0x004AB8C0 [MATCHED]
	void GetCPUTimestamp(int64_t* timestamp)
	{
		__asm
		{
			mov   al, byte ptr [g_hasTimeStampCounter]   ; dead load, matches original
			rdtsc                                        ; EDX:EAX = 64-bit counter
			mov   ebx, timestamp                         ; pointer arg
			mov   [ebx], eax                             ; low dword
			mov   [ebx + 4], edx                         ; high dword
		}
	}

	// FUNCTION: TOY2 0x004AB8E0 [MATCHED]
	void CalibrateTimestampCounter()
	{
		// GLOBAL: TOY2 0x00883FFC
		static int32_t g_hasCpuFeatures;

		if (! g_hasCpuFeatures)
		{
			g_hasCpuFeatures = 1;

			GetCPUFeatures();

			if (g_hasTimeStampCounter)
			{
				int64_t startTicks;
				GetCPUTimestamp(&startTicks);

				Sleep(1000);

				int64_t endTicks;
				GetCPUTimestamp(&endTicks);

				g_cpuTicksPerSecond = endTicks - startTicks;
			}
		}
	}

	// FUNCTION: TOY2 0x004AB850 [MATCHED]
	uint32_t GetHighResolutionTime()
	{
		// GLOBAL: TOY2 0x00884000
		static int32_t g_cpuTimestampInitialized;

		if (! g_cpuTimestampInitialized)
		{
			g_cpuTimestampInitialized = 1;
			CalibrateTimestampCounter();
		}

		if (g_hasTimeStampCounter)
		{
			int64_t currentTicks;
			GetCPUTimestamp(&currentTicks);
			currentTicks = (1000 * currentTicks) / g_cpuTicksPerSecond;

			return (uint32_t)currentTicks;
		}
		else
		{
			return timeGetTime();
		}
	}

	// FUNCTION: TOY2 0x004AB950 [MATCHED]
	void PrecisionSleep(int32_t delayMs)
	{
		if (g_hasTimeStampCounter)
		{
			int64_t delayTicks = delayMs * g_cpuTicksPerSecond / 1000;

			int64_t startTicks;
			GetCPUTimestamp(&startTicks);

			do
			{
				Sleep(1);

				int64_t currentTicks;
				GetCPUTimestamp(&currentTicks);

				int64_t elapsedTicks = currentTicks - startTicks;

				if (elapsedTicks < 0) // TSC went backwards -> clamp to 1
					elapsedTicks = 1;

				startTicks = currentTicks;
				delayTicks -= elapsedTicks;

			} while (delayTicks > 0);
		}
		else
		{
			Sleep(delayMs);
		}
	}

	// FUNCTION: TOY2 0x004ABA10
	int32_t GetCalibrationLow32()
	{
		if (! g_calibrationGetterInitialized)
		{
			g_calibrationGetterInitialized = 1;
			CalibrateTimestampCounter();
		}

		return g_hasTimeStampCounter ? (int32_t)g_cpuTicksPerSecond : 1;
	}

	// FUNCTION: TOY2 0x0047D520 [MATCHED]
	void MemSet32Util(void* buffer, int32_t count, uint32_t value)
	{
		__asm
		{
			push    edi
			mov     edi, buffer
			mov     ecx, count
			mov     eax, value
			rep     stosd
			pop     edi
		}
	}

	// FUNCTION: TOY2 0x004CB2C0 [MATCHED]
	int32_t SetUseAsDiffuseModulation(int32_t option)
	{
		int32_t useAsDiffuseModulation = g_useAsDiffuseModulation;
		g_useAsDiffuseModulation = option;

		return useAsDiffuseModulation;
	}

	// FUNCTION: TOY2 0x004CB2D0
	int32_t GetUseAsDiffuseModulation() { return g_useAsDiffuseModulation; }

	// FUNCTION: TOY2 0x004CB2A0 [MATCHED]
	int32_t SetDefaultPrimFlags(int32_t option)
	{
		int32_t defaultPrimitiveFlags = g_defaultPrimitiveFlags;
		g_defaultPrimitiveFlags = option;

		return defaultPrimitiveFlags;
	}

	// FUNCTION: TOY2 0x004CB2B0
	int32_t GetDefaultPrimFlags() { return g_defaultPrimitiveFlags; }

	STATIC_ASSERT(offsetof(VertexTL, position) == 0x00);
	STATIC_ASSERT(offsetof(VertexTL, rhw) == 0x0C);
	STATIC_ASSERT(offsetof(VertexTL, diffuse) == 0x10);
	STATIC_ASSERT(offsetof(VertexTL, specular) == 0x14);
	STATIC_ASSERT(offsetof(VertexTL, uv) == 0x18);

	// FUNCTION: TOY2 0x004B9D40
	__declspec(naked) void SetTimingDisplayVertex(VertexTL* vertex, float x, float y, uint32_t color)
	{
		// X passes through x87; Y retains its raw float bits.
		__asm
		{
			mov eax, [esp + 4]
			mov ecx, [esp + 12]
			fld dword ptr [esp + 8]
			mov [eax + 4], ecx
			xor ecx, ecx
			fstp dword ptr [eax]
			mov [eax + 8], ecx
			mov [eax + 28], ecx
			mov [eax + 24], ecx
			mov ecx, [esp + 16]
			mov dword ptr [eax + 12], 3F000000h
			mov [eax + 20], ecx
			mov [eax + 16], ecx
			ret
		}
	}
}
