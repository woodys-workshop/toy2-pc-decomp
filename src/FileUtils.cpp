#include "FileUtils.h"
#include "Logger.h"
#include "Toy2/D3DApp.h"
#include "AudioManager/AudioManager.h"
#include <stdio.h>
#include <windows.h>

namespace FileUtils
{
	// GLOBAL: TOY2 0x00882F40
	char g_pathRegValue[513];

	// GLOBAL: TOY2 0x00883144
	char g_cdPathRegValue[513];

	// GLOBAL: TOY2 0x00882F3C
	int32_t g_registryKeysRead;

	// GLOBAL: TOY2 0x00882E34
	char g_fileNameBuffer[260];

	// FUNCTION: TOY2 0x004A67D0
	int32_t GetFileSize(const char* fileName)
	{
		if (! g_registryKeysRead)
			ValidateInstall();

		strcpy(g_fileNameBuffer, g_pathRegValue);
		strcat(g_fileNameBuffer, fileName);

		Logger::Log("LOAD : Getting length of file %s.\n", g_fileNameBuffer);

		FILE* fd = fopen(g_fileNameBuffer, "rb");

		if (fd)
		{
			fseek(fd, 0, 2);
			int32_t fileSize = ftell(fd);
			fclose(fd);

			Logger::Log("LOAD : Length found - file %s is %d bytes long.\n", g_fileNameBuffer, fileSize);
			return fileSize;
		}
		else
		{
			Logger::Log("LOAD : File %s not found.\n", g_fileNameBuffer);
			return 0;
		}
	}

	// FUNCTION: TOY2 0x004A65E0
	void AppendCDPath(char* path)
	{
		if (! g_registryKeysRead)
			ValidateInstall();

		strcpy(path, g_cdPathRegValue);
	}

	// FUNCTION: TOY2 0x004A6790
	void AppendRegPathToBuffer()
	{
		if (! g_registryKeysRead)
			ValidateInstall();

		strcpy(g_fileNameBuffer, g_pathRegValue);
	}

	// FUNCTION: TOY2 0x004A65A0
	void GetPathValue(char* pathOut)
	{
		if (! g_registryKeysRead)
			ValidateInstall();

		strcpy(pathOut, g_pathRegValue);
	}

	// FUNCTION: TOY2 0x004A6940
	size_t LoadFile(const char* fileName, void* buffer)
	{
		if (! g_registryKeysRead)
			ValidateInstall();

		strcpy(g_fileNameBuffer, g_pathRegValue);
		strcat(g_fileNameBuffer, fileName);

		Logger::Log("LOAD : Loading file %s.\n", g_fileNameBuffer);
		AudioManager::StopAndFlush();

		FILE* fin = fopen(g_fileNameBuffer, "rb");

		if (! fin)
			return 0;

		if (fseek(fin, 0, 2))
		{
			Logger::Log("FATAL_ERROR - %s failed - error was %s.\n", "fseek(fin,0,2)", strerror(errno));
			exit(1);
		}

		size_t elemSize = ftell(fin);
		if (fseek(fin, 0, 0))
		{
			Logger::Log("FATAL_ERROR - %s failed - error was %s.\n", "fseek(fin,0,0)", strerror(errno));
			exit(1);
		}

		fread(buffer, elemSize, 1, fin);
		if (fclose(fin))
		{
			Logger::Log("FATAL_ERROR - %s failed - error was %s.\n", "fclose(fin)", strerror(errno));
			exit(1);
		}

		return elemSize;
	}

	// FUNCTION: TOY2 0x004A6390
	void ValidateInstall()
	{
		HKEY keyHandle;
		HKEY phkResult;
		int32_t allow32B;
		char fileNameBuffer[1024];

		g_cdPathRegValue[0] = '\0';
		g_pathRegValue[0] = '\0';

		if (! RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software", 0, KEY_ALL_ACCESS, &phkResult))
		{
			if (! RegOpenKeyExA(phkResult, "TravellersTalesToyStory2", 0, KEY_ALL_ACCESS, &keyHandle))
			{
				DWORD dataSize = 512;

				if (! RegQueryValueExA(keyHandle, "path", 0, 0, (LPBYTE)g_pathRegValue, &dataSize))
					g_pathRegValue[dataSize] = 0;

				dataSize = 512;

				if (RegQueryValueExA(keyHandle, "cdpath", 0, 0, (LPBYTE)g_cdPathRegValue, &dataSize))
					strcpy(g_cdPathRegValue, g_pathRegValue);
				else
					g_cdPathRegValue[dataSize] = 0;

				dataSize = 4;

				if (! RegQueryValueExA(keyHandle, "allow32bit", 0, 0, (LPBYTE)&allow32B, &dataSize) && allow32B)
					D3DApp::g_no32bitColors = 0;

				RegCloseKey(keyHandle);
			}

			RegCloseKey(phkResult);
		}

		g_registryKeysRead = 1;

		if (! g_pathRegValue[0] || ! g_cdPathRegValue[0])
			Logger::GetErrorHandler("C:\\projects\\toy2\\Win95.cpp", 217)("Toy Story 2 is not correctly installed,\r\nplease re-install.");

		strcpy(fileNameBuffer, g_cdPathRegValue);
		strcat(fileNameBuffer, "validate.tta");

		FILE* validateFile = fopen(fileNameBuffer, "rb");

		if (validateFile)
		{
			fclose(validateFile);
		}
		else
		{
			Logger::GetErrorHandler("C:\\projects\\toy2\\Win95.cpp", 227)(
				"Unable to find file \r\n\"%s\".\r\nPlease ensure your Toy Story 2 "
				"CD\r\nis in the specified drive.",
				fileNameBuffer);
		}
	}
}
