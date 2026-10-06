#include "Logger.h"

#include <cstdio>
#include <cstdarg>

namespace Logger
{
	// GLOBAL: TOY2 0x00B6269C
	const char* g_errorHandlerPath;

	// GLOBAL: TOY2 0x00B626A0
	int32_t g_errorHandlerLine;

	// GLOBAL: TOY2 0x00883348
	int32_t g_showMsgBoxOnThrow;

	// GLOBAL: TOY2 0x00882F38
	int32_t g_logsEnabled;

	// GLOBAL: TOY2 0x00504E54
	int32_t g_logFileExists = 1;
}

namespace Logger
{
	// FUNCTION: TOY2 0x004A87C0
	ThrowErrorFunc GetErrorHandler(char* filePath, int32_t lineNumber)
	{
		g_errorHandlerPath = filePath;
		g_errorHandlerLine = lineNumber;

		return ThrowError;
	}

	// FUNCTION: TOY2 0x004A8710
	void ThrowError(char* format, ...)
	{
		char caption[256];
		char text[1024];

		va_list argList;
		va_start(argList, format);

		if (*format)
		{
			sprintf(caption, "Soft Abort - %s Line %d", g_errorHandlerPath, g_errorHandlerLine);
			vsprintf(text, format, argList);

			FILE* file = fopen("toy2.err", "wb");

			if (file)
			{
				fprintf(file, "%s\r\n%s\r\n", caption, text);
				fclose(file);
			}

			if (! g_showMsgBoxOnThrow)
				MessageBoxA(0, text, caption, 0);
		}

		exit(-1);
	}

	// FUNCTION: TOY2 0x004A66A0 [MODIFIED]
	void Log(char* format, ...)
	{
		char buffer[1024];

		va_list argList;
		va_start(argList, format);

		memset(buffer, 0, sizeof(buffer));
		vsprintf(buffer, format, argList);

		printf("%s", buffer); // Addition

		if (g_logsEnabled)
		{
			if (g_logFileExists)
			{
				g_logFileExists = 0;
				remove("toy2.log");
			}

			FILE* file = fopen("toy2.log", "at");

			if (file)
			{
				fprintf(file, buffer);
				fclose(file);
			}
		}
	}

	// FUNCTION: TOY2 0x004A6730
	void LogLn(char* format, ...)
	{
		char buffer[1024];

		va_list argList;
		va_start(argList, format);

		memset(buffer, 0, sizeof(buffer));
		vsprintf(buffer, format, argList);
		lstrcatA(buffer, "\r\n");

		Log(buffer);
	}

	// STUB: TOY2 0x004ADFD0
	void LogD3DError(int32_t errorCode) {}

	// FUNCTION: TOY2 0x00431900
	void LogDDError(const char* message, HRESULT error)
	{
		char buffer[2048];

		memset(buffer, 0, sizeof(buffer));
		char* errorToMsg = ErrorToMessage(error);
		sprintf(buffer, "ERROR - %s\nERROR - %s\n", message, errorToMsg);

		LogLn(buffer);
	}

	// FUNCTION: TOY2 0x004A8870
	void DebugLog(char* format, ...)
	{
		char buffer[1024];
		va_list vaList;

		va_start(vaList, format);
		vsprintf(buffer, format, vaList);
		OutputDebugStringA(buffer);
	}

	// STUB: TOY2 0x0040D490;
	char* ErrorToMessage(HRESULT error) { return "Unimplemented"; }

	// STUB: TOY2 0x0040C130
	void LogErrorNotSetLn(char* format, ...) {}
}
