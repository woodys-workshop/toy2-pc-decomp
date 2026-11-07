#include "Common.h"

namespace D3DApp
{
	// FUNCTION: TOY2 0x0040FA60 [MATCHED]
	char* LSTRCHR(const uint8_t* lpString, int32_t bChar)
	{
		if (lpString != 0)
		{
			while (*lpString != 0)
			{
				if (*lpString == bChar)
				{
					return (char*)lpString;
				}

				lpString++;
			}
		}
		return 0;
	}

	// FUNCTION: TOY2 0x0040FA90 [MATCHED]
	char* LSTRRCHR(const uint8_t* lpString, int32_t bChar)
	{
		if (lpString != 0)
		{
			const uint8_t* lpBegin;

			lpBegin = lpString;

			while (*lpString != 0)
			{
				lpString++;
			}

			while (1)
			{
				if (*lpString == bChar)
				{
					return (char*)lpString;
				}

				if (lpString == lpBegin)
				{
					break;
				}

				lpString--;
			}
		}

		return 0;
	}
}
