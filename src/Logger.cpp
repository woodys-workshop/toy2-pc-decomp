#include "Logger.h"

#include <cstdio>
#include <cstdarg>

#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <directx6/d3drm.h>

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
	uint16_t g_logFileExists = 1;

	// GLOBAL: TOY2 0x004E0698
	char g_errorNotSet[256] = "ERROR NOT SET";
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
#ifndef APPLY_FIXES
		if (! g_logsEnabled)
			return;
#endif

		char buffer[1024];

		va_list argList;
		va_start(argList, format);

		memset(buffer, 0, sizeof(buffer));
		vsprintf(buffer, format, argList);

#ifdef APPLY_FIXES
		printf("%s", buffer);

		if (g_logsEnabled)
#endif
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

	// FUNCTION: TOY2 0x004ADFD0
	void LogD3DError(int32_t errorCode)
	{
		char* errorStr = D3DAppErrorToString(errorCode);

		OutputDebugStringA(errorStr);
		OutputDebugStringA("\n");
	}

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

	// FUNCTION: TOY2 0x0040D490
	char* ErrorToMessage(HRESULT error)
	{
		switch (error)
		{
			// TODO: implement DSOUND errors
			case E_NOTIMPL:
				return "Action not supported.";
			case (HRESULT)0x80004002:
				return "No such interface supported";
			case (HRESULT)0x80004003:
				return "Invalid pointer";
			case (HRESULT)0x80004004:
				return "Operation aborted";
			case E_FAIL:
				return "Generic failure.";
			case (HRESULT)0x80070005:
				return "General access denied error";
			case (HRESULT)0x80070006:
				return "Invalid handle";
			case E_OUTOFMEMORY:
				return "DirectDraw does not have enough memory to perform the operation.";
			case E_INVALIDARG:
				return "One or more of the parameters passed to the function are incorrect.";
			case DDERR_ALREADYINITIALIZED:
				return "This object is already initialized.";
			case DDERR_CANNOTATTACHSURFACE:
				return "This surface can not be attached to the requested surface.";
			case DDERR_CANNOTDETACHSURFACE:
				return "This surface can not be detached from the requested surface.";
			case DDERR_CURRENTLYNOTAVAIL:
				return "Support is currently not available.";
			case DDERR_EXCEPTION:
				return "An exception was encountered while performing the requested operation.";
			case DDERR_HEIGHTALIGN:
				return "Height of rectangle provided is not a multiple of reqd alignment.";
			case DDERR_INCOMPATIBLEPRIMARY:
				return "Unable to match primary surface creation request with existing primary surface.";
			case DDERR_INVALIDCAPS:
				return "One or more of the caps bits passed to the callback are incorrect.";
			case DDERR_INVALIDCLIPLIST:
				return "DirectDraw does not support the provided cliplist.";
			case DDERR_INVALIDMODE:
				return "DirectDraw does not support the requested mode.";
			case DDERR_INVALIDOBJECT:
				return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.";
			case DDERR_INVALIDPIXELFORMAT:
				return "The pixel format was invalid as specified.";
			case DDERR_INVALIDRECT:
				return "Rectangle provided was invalid.";
			case DDERR_LOCKEDSURFACES:
				return "Operation could not be carried out because one or more surfaces are locked.";
			case DDERR_NO3D:
				return "There is no 3D present.";
			case DDERR_NOALPHAHW:
				return "Operation could not be carried out because there is no alpha accleration hardware present or available.";
			case DDERR_NOCLIPLIST:
				return "No cliplist available.";
			case DDERR_NOCOLORCONVHW:
				return "Operation could not be carried out because there is no color conversion hardware present or available.";
			case DDERR_NOCOOPERATIVELEVELSET:
				return "Create function called without DirectDraw object method SetCooperativeLevel being called.";
			case DDERR_NOCOLORKEY:
				return "Surface doesn't currently have a color key";
			case DDERR_NOCOLORKEYHW:
				return "Operation could not be carried out because there is no hardware support of the destination color key.";
			case DDERR_NOEXCLUSIVEMODE:
				return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.";
			case DDERR_NOFLIPHW:
				return "Flipping visible surfaces is not supported.";
			case DDERR_NOGDI:
				return "There is no GDI present.";
			case DDERR_NOMIRRORHW:
				return "Operation could not be carried out because there is no hardware present or available.";
			case DDERR_NOTFOUND:
				return "Requested item was not found.";
			case DDERR_NOOVERLAYHW:
				return "Operation could not be carried out because there is no overlay hardware present or available.";
			case DDERR_NORASTEROPHW:
				return "Operation could not be carried out because there is no appropriate raster op hardware present or available.";
			case DDERR_NOROTATIONHW:
				return "Operation could not be carried out because there is no rotation hardware present or available.";
			case DDERR_NOSTRETCHHW:
				return "Operation could not be carried out because there is no hardware support for stretching.";
			case DDERR_NOT4BITCOLOR:
				return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.";
			case DDERR_NOT4BITCOLORINDEX:
				return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.";
			case DDERR_NOT8BITCOLOR:
				return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.";
			case DDERR_NOTEXTUREHW:
				return "Operation could not be carried out because there is no texture mapping hardware present or available.";
			case DDERR_NOVSYNCHW:
				return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.";
			case DDERR_NOZBUFFERHW:
				return "Operation could not be carried out because there is no hardware support for zbuffer blitting.";
			case DDERR_NOZOVERLAYHW:
				return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.";
			case DDERR_OUTOFCAPS:
				return "The hardware needed for the requested operation has already been allocated.";
			case DDERR_OUTOFVIDEOMEMORY:
				return "DirectDraw does not have enough memory to perform the operation.";
			case DDERR_OVERLAYCANTCLIP:
				return "The hardware does not support clipped overlays.";
			case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
				return "Can only have ony color key active at one time for overlays.";
			case DDERR_PALETTEBUSY:
				return "Access to this palette is being refused because the palette is already locked by another thread.";
			case DDERR_COLORKEYNOTSET:
				return "No src color key specified for this operation.";
			case DDERR_SURFACEALREADYATTACHED:
				return "This surface is already attached to the surface it is being attached to.";
			case DDERR_SURFACEALREADYDEPENDENT:
				return "This surface is already a dependency of the surface it is being made a dependency of.";
			case DDERR_SURFACEBUSY:
				return "Access to this surface is being refused because the surface is already locked by another thread.";
			case DDERR_SURFACEISOBSCURED:
				return "Access to surface refused because the surface is obscured.";
			case DDERR_SURFACELOST:
				return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface "
					   "should have Restore called on it.";
			case DDERR_SURFACENOTATTACHED:
				return "The requested surface is not attached.";
			case DDERR_TOOBIGHEIGHT:
				return "Height requested by DirectDraw is too large.";
			case DDERR_TOOBIGSIZE:
				return "Size requested by DirectDraw is too large, but the individual height and width are OK.";
			case DDERR_TOOBIGWIDTH:
				return "Width requested by DirectDraw is too large.";
			case DDERR_UNSUPPORTEDFORMAT:
				return "FOURCC format requested is unsupported by DirectDraw.";
			case DDERR_UNSUPPORTEDMASK:
				return "Bitmask in the pixel format requested is unsupported by DirectDraw.";
			case DDERR_VERTICALBLANKINPROGRESS:
				return "Vertical blank is in progress.";
			case DDERR_WASSTILLDRAWING:
				return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.";
			case DDERR_XALIGN:
				return "Rectangle provided was not horizontally aligned on required boundary.";
			case DDERR_INVALIDDIRECTDRAWGUID:
				return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.";
			case DDERR_DIRECTDRAWALREADYCREATED:
				return "A DirectDraw object representing this driver has already been created for this process.";
			case DDERR_NODIRECTDRAWHW:
				return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.";
			case DDERR_PRIMARYSURFACEALREADYEXISTS:
				return "This process already has created a primary surface.";
			case DDERR_NOEMULATION:
				return "Software emulation not available.";
			case DDERR_REGIONTOOSMALL:
				return "Region passed to Clipper::GetClipList is too small.";
			case DDERR_CLIPPERISUSINGHWND:
				return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.";
			case DDERR_NOCLIPPERATTACHED:
				return "No clipper object attached to surface object.";
			case DDERR_NOHWND:
				return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.";
			case DDERR_HWNDSUBCLASSED:
				return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.";
			case DDERR_HWNDALREADYSET:
				return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.";
			case DDERR_NOPALETTEATTACHED:
				return "No palette object attached to this surface.";
			case DDERR_NOPALETTEHW:
				return "No hardware support for 16 or 256 color palettes.";
			case DDERR_BLTFASTCANTCLIP:
				return "Return if a clipper object is attached to the source surface passed into a BltFast call.";
			case DDERR_NOBLTHW:
				return "No blitter hardware present.";
			case DDERR_NODDROPSHW:
				return "No DirectDraw ROP hardware.";
			case DDERR_OVERLAYNOTVISIBLE:
				return "Returned when GetOverlayPosition is called on a hidden overlay.";
			case DDERR_NOOVERLAYDEST:
				return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.";
			case DDERR_INVALIDPOSITION:
				return "Returned when the position of the overlay on the destination is no longer legal for that destination.";
			case DDERR_NOTAOVERLAYSURFACE:
				return "Returned when an overlay member is called for a non-overlay surface.";
			case DDERR_EXCLUSIVEMODEALREADYSET:
				return "An attempt was made to set the cooperative level when it was already set to exclusive.";
			case DDERR_NOTFLIPPABLE:
				return "An attempt has been made to flip a surface that is not flippable.";
			case DDERR_CANTDUPLICATE:
				return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.";
			case DDERR_NOTLOCKED:
				return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.";
			case DDERR_CANTCREATEDC:
				return "Windows can not create any more DCs.";
			case DDERR_NODC:
				return "No DC was ever created for this surface.";
			case DDERR_WRONGMODE:
				return "This surface can not be restored because it was created in a different mode.";
			case DDERR_IMPLICITLYCREATED:
				return "This surface can not be restored because it is an implicitly created surface.";
			case DDERR_NOTPALETTIZED:
				return "The surface being used is not a palette-based surface.";
			case DDERR_UNSUPPORTEDMODE:
				return "The FourCC format requested is not supported by DirectDraw. ";
			case DDERR_NOMIPMAPHW:
				return "The operation cannot be carried out because no mipmap texture mapping hardware is present or available.";
			case D3DERR_BADMINORVERSION:
				return "D3DERR_BADMINORVERSION";
			case D3DERR_INVALID_DEVICE:
				return "D3DERR_INVALID_DEVICE";
			case D3DERR_INITFAILED:
				return "D3DERR_INITFAILED";
			case D3DERR_DEVICEAGGREGATED:
				return "D3DERR_DEVICEAGGREGATED";
			case D3DERR_EXECUTE_CREATE_FAILED:
				return "D3DERR_EXECUTE_CREATE_FAILED";
			case D3DERR_EXECUTE_DESTROY_FAILED:
				return "D3DERR_EXECUTE_DESTROY_FAILED";
			case D3DERR_EXECUTE_LOCK_FAILED:
				return "D3DERR_EXECUTE_LOCK_FAILED";
			case D3DERR_EXECUTE_UNLOCK_FAILED:
				return "D3DERR_EXECUTE_UNLOCK_FAILED";
			case D3DERR_EXECUTE_LOCKED:
				return "D3DERR_EXECUTE_LOCKED";
			case D3DERR_EXECUTE_NOT_LOCKED:
				return "D3DERR_EXECUTE_NOT_LOCKED";
			case D3DERR_EXECUTE_FAILED:
				return "D3DERR_EXECUTE_FAILED";
			case D3DERR_EXECUTE_CLIPPED_FAILED:
				return "D3DERR_EXECUTE_CLIPPED_FAILED";
			case D3DERR_TEXTURE_NO_SUPPORT:
				return "D3DERR_TEXTURE_NO_SUPPORT";
			case D3DERR_TEXTURE_CREATE_FAILED:
				return "D3DERR_TEXTURE_CREATE_FAILED";
			case D3DERR_TEXTURE_DESTROY_FAILED:
				return "D3DERR_TEXTURE_DESTROY_FAILED";
			case D3DERR_TEXTURE_LOCK_FAILED:
				return "D3DERR_TEXTURE_LOCK_FAILED";
			case D3DERR_TEXTURE_UNLOCK_FAILED:
				return "D3DERR_TEXTURE_UNLOCK_FAILED";
			case D3DERR_TEXTURE_LOAD_FAILED:
				return "D3DERR_TEXTURE_LOAD_FAILED";
			case D3DERR_TEXTURE_SWAP_FAILED:
				return "D3DERR_TEXTURE_SWAP_FAILED";
			case D3DERR_TEXTURE_LOCKED:
				return "D3DERR_TEXTURE_LOCKED";
			case D3DERR_TEXTURE_NOT_LOCKED:
				return "D3DERR_TEXTURE_NOT_LOCKED";
			case D3DERR_TEXTURE_GETSURF_FAILED:
				return "D3DERR_TEXTURE_GETSURF_FAILED";
			case D3DERR_MATRIX_CREATE_FAILED:
				return "D3DERR_MATRIX_CREATE_FAILED";
			case D3DERR_MATRIX_DESTROY_FAILED:
				return "D3DERR_MATRIX_DESTROY_FAILED";
			case D3DERR_MATRIX_SETDATA_FAILED:
				return "D3DERR_MATRIX_SETDATA_FAILED";
			case D3DERR_MATRIX_GETDATA_FAILED:
				return "D3DERR_MATRIX_GETDATA_FAILED";
			case D3DERR_SETVIEWPORTDATA_FAILED:
				return "D3DERR_SETVIEWPORTDATA_FAILED";
			case D3DERR_INVALIDCURRENTVIEWPORT:
				return "D3DERR_INVALIDCURRENTVIEWPORT";
			case D3DERR_INVALIDPRIMITIVETYPE:
				return "D3DERR_INVALIDPRIMITIVETYPE";
			case D3DERR_INVALIDVERTEXTYPE:
				return "D3DERR_INVALIDVERTEXTYPE";
			case D3DERR_TEXTURE_BADSIZE:
				return "D3DERR_TEXTURE_BADSIZE";
			case D3DERR_INVALIDRAMPTEXTURE:
				return "D3DERR_INVALIDRAMPTEXTURE";
			case D3DERR_MATERIAL_CREATE_FAILED:
				return "D3DERR_MATERIAL_CREATE_FAILED";
			case D3DERR_MATERIAL_DESTROY_FAILED:
				return "D3DERR_MATERIAL_DESTROY_FAILED";
			case D3DERR_MATERIAL_SETDATA_FAILED:
				return "D3DERR_MATERIAL_SETDATA_FAILED";
			case D3DERR_MATERIAL_GETDATA_FAILED:
				return "D3DERR_MATERIAL_GETDATA_FAILED";
			case D3DERR_INVALIDPALETTE:
				return "D3DERR_INVALIDPALETTE";
			case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
				return "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY";
			case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
				return "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY";
			case D3DERR_SURFACENOTINVIDMEM:
				return "D3DERR_SURFACENOTINVIDMEM";
			case D3DERR_LIGHT_SET_FAILED:
				return "D3DERR_LIGHT_SET_FAILED";
			case D3DERR_LIGHTHASVIEWPORT:
				return "D3DERR_LIGHTHASVIEWPORT";
			case D3DERR_LIGHTNOTINTHISVIEWPORT:
				return "D3DERR_LIGHTNOTINTHISVIEWPORT";
			case D3DERR_SCENE_IN_SCENE:
				return "D3DERR_SCENE_IN_SCENE";
			case D3DERR_SCENE_NOT_IN_SCENE:
				return "D3DERR_SCENE_NOT_IN_SCENE";
			case D3DERR_SCENE_BEGIN_FAILED:
				return "D3DERR_SCENE_BEGIN_FAILED";
			case D3DERR_SCENE_END_FAILED:
				return "D3DERR_SCENE_END_FAILED";
			case D3DERR_INBEGIN:
				return "D3DERR_INBEGIN";
			case D3DERR_NOTINBEGIN:
				return "D3DERR_NOTINBEGIN";
			case D3DERR_NOVIEWPORTS:
				return "D3DERR_NOVIEWPORTS";
			case D3DERR_VIEWPORTDATANOTSET:
				return "D3DERR_VIEWPORTDATANOTSET";
			case D3DERR_VIEWPORTHASNODEVICE:
				return "D3DERR_VIEWPORTHASNODEVICE";
			case D3DERR_NOCURRENTVIEWPORT:
				return "D3DERR_NOCURRENTVIEWPORT";
			case (HRESULT)0xFFFFFFF8:
				return "Compression algorithm not recognized.";
			case (HRESULT)0xFFFFFFF9:
				return "Input parameter out of acceptable range.";
			case (HRESULT)0xFFFFFFFA:
				return "Bad global handle.";
			case (HRESULT)0xFFFFFFFB:
				return "Insufficient memory for LZFile structure.";
			case (HRESULT)0xFFFFFFFC:
				return "Out of space for output file.";
			case (HRESULT)0xFFFFFFFD:
				return "Corrupt compressed file format.";
			case (HRESULT)0xFFFFFFFE:
				return "Invalid output handle.";
			case (HRESULT)0xFFFFFFFF:
				return "Invalid input handle.";
			case DD_OK:
				return "No error.";
			case 1:
				return "The function is incorrect.";
			case 2:
				return "The system cannot find the file specified.";
			case 3:
				return "The system cannot find the specified path.";
			case 4:
				return "The system cannot open the file.";
			case 5:
				return "Access is denied.";
			case 6:
				return "The internal file identifier is incorrect.";
			case 7:
				return "The storage control blocks were destroyed.";
			case 8:
				return "Not enough storage is available to process this command.";
			case 9:
				return "The storage control block address is invalid.";
			case 10:
				return "The environment is incorrect.";
			case 11:
				return "An attempt was made to load a program with an incorrect format.";
			case 12:
				return "The access code is invalid.";
			case 13:
				return "The data is invalid.";
			case 14:
				return "Not enough storage is available to complete this operation.";
			case 15:
				return "The system cannot find the specified drive.";
			case 16:
				return "The directory cannot be removed.";
			case 17:
				return "The system cannot move the file to a different disk drive.";
			case 18:
				return "There are no more files.";
			case 19:
				return "The media is write protected.";
			case 20:
				return "The system cannot find the specified device.";
			case 21:
				return "The drive is not ready.";
			case 22:
				return "The device does not recognize the command.";
			case 23:
				return "Data ERROR (cyclic redundancy check)";
			case 24:
				return "The program issued a command but the command length is incorrect.";
			case 25:
				return "The drive cannot locate a specific area or track on the disk.";
			case 26:
				return "The specified disk cannot be accessed.";
			case 27:
				return "The drive cannot find the requested sector.";
			case 28:
				return "The printer is out of paper.";
			case 29:
				return "The system cannot write to the specified device.";
			case 30:
				return "The system cannot read from the specified device.";
			case 31:
				return "A device attached to the system is not functioning.";
			case 32:
				return "The process cannot access the file because it is being used by another process.";
			case 33:
				return "The process cannot access the file because another process has locked a portion of the file.";
			case 34:
				return "The wrong disk is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.";
			case 36:
				return "Too many files opened for sharing.";
			case 38:
				return "Reached End Of File.";
			case 39:
				return "The disk is full.";
			case 50:
				return "The network request is not supported.";
			case 51:
				return "The remote computer is not available.";
			case 52:
				return "A duplicate name exists on the network.";
			case 53:
				return "The network path was not found.";
			case 54:
				return "The network is busy.";
			case 55:
				return "The specified network resource is no longer available.";
			case 56:
				return "The network BIOS command limit has been reached.";
			case 57:
				return "A network adapter hardware ERROR occurred.";
			case 58:
				return "The specified server cannot perform the requested operation.";
			case 59:
				return "An unexpected network ERROR occurred.";
			case 60:
				return "The remote adapter is not compatible.";
			case 61:
				return "The printer queue is full.";
			case 62:
				return "Space to store the file waiting to be printed is not available on the server.";
			case 63:
				return "File waiting to be printed was deleted.";
			case 64:
				return "The specified network name is no longer available.";
			case 65:
				return "Network access is denied.";
			case 66:
				return "The network resource type is incorrect.";
			case 67:
				return "The network name cannot be found.";
			case 68:
				return "The name limit for the local computer network adapter card exceeded.";
			case 69:
				return "The network BIOS session limit exceeded.";
			case 70:
				return "The remote server is paused or is in the process of being started.";
			case 71:
				return "The network request was not accepted.";
			case 72:
				return "The specified printer or disk device has been paused.";
			case 80:
				return "The file exists.";
			case 82:
				return "The directory or file cannot be created.";
			case 83:
				return "Fail on INT 24.";
			case 84:
				return "Storage to process this request is not available.";
			case 85:
				return "The local device name is already in use.";
			case 86:
				return "The specified network password is incorrect.";
			case 87:
				return "The parameter is incorrect.";
			case 88:
				return "A write fault occurred on the network.";
			case 89:
				return "The system cannot start another process at this time.";
			case 100:
				return "Cannot create another system semaphore.";
			case 101:
				return "The exclusive semaphore is owned by another process.";
			case 102:
				return "The semaphore is set and cannot be closed.";
			case 103:
				return "The semaphore cannot be set again.";
			case 104:
				return "Cannot request exclusive semaphores at interrupt time.";
			case 105:
				return "The previous ownership of this semaphore has ended.";
			case 106:
				return "Insert the disk for drive 1.";
			case 107:
				return "Program stopped because alternate disk was not inserted.";
			case 108:
				return "The disk is in use or locked by another process.";
			case 109:
				return "The pipe was ended.";
			case 110:
				return "The system cannot open the specified device or file.";
			case 111:
				return "The file name is too long.";
			case 112:
				return "There is not enough space on the disk.";
			case 113:
				return "No more internal file identifiers available.";
			case 114:
				return "The target internal file identifier is incorrect.";
			case 117:
				return "The IOCTL call made by the application program is incorrect.";
			case 118:
				return "The verify-on-write switch parameter value is incorrect.";
			case 119:
				return "The system does not support the requested command.";
			case 120:
				return "The Application Program Interface (API) entered will only work in Windows/NT mode.";
			case 121:
				return "The semaphore timeout period has expired.";
			case 122:
				return "The data area passed to a system call is too small.";
			case 123:
				return "The file name, directory name, or volume label is syntactically incorrect.";
			case 124:
				return "The system call level is incorrect.";
			case 125:
				return "The disk has no volume label.";
			case 126:
				return "The specified module cannot be found.";
			case 127:
				return "The specified procedure could not be found.";
			case 128:
				return "There are no child processes to wait for.";
			case 129:
				return "The %1 application cannot be run in Windows mode.";
			case 130:
				return "Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.";
			case 131:
				return "An attempt was made to move the file pointer before the beginning of the file.";
			case 132:
				return "The file pointer cannot be set on the specified device or file.";
			case 133:
				return "A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.";
			case 134:
				return "An attempt was made to use a JOIN or SUBST command on a drive that is already joined.";
			case 135:
				return "An attempt was made to use a JOIN or SUBST command on a drive already substituted.";
			case 136:
				return "The system attempted to delete the JOIN of a drive not previously joined.";
			case 137:
				return "The system attempted to delete the substitution of a drive not previously substituted.";
			case 138:
				return "The system tried to join a drive to a directory on a joined drive.";
			case 139:
				return "The system attempted to substitute a drive to a directory on a substituted drive.";
			case 140:
				return "The system tried to join a drive to a directory on a substituted drive.";
			case 141:
				return "The system attempted to SUBST a drive to a directory on a joined drive.";
			case 142:
				return "The system cannot perform a JOIN or SUBST at this time.";
			case 143:
				return "The system cannot join or substitute a drive to or for a directory on the same drive.";
			case 144:
				return "The directory is not a subdirectory of the root directory.";
			case 145:
				return "The directory is not empty.";
			case 146:
				return "The path specified is being used in a substitute.";
			case 147:
				return "Not enough resources are available to process this command.";
			case 148:
				return "The specified path cannot be used at this time.";
			case 149:
				return "An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.";
			case 150:
				return "System trace information not specified in your CONFIG.SYS file, or tracing is not allowed.";
			case 151:
				return "The number of specified semaphore events is incorrect.";
			case 152:
				return "Too many semaphores are already set.";
			case 153:
				return "The list is not correct.";
			case 154:
				return "The volume label entered exceeds the 11 character limit. The first 11 characters were written to disk. Any characters that exceeded "
					   "the 11 character limit were automatically deleted.";
			case 155:
				return "Cannot create another thread.";
			case 156:
				return "The recipient process has refused the signal.";
			case 157:
				return "The segment is already discarded and cannot be locked.";
			case 158:
				return "The segment is already unlocked.";
			case 159:
				return "The address for the thread ID is incorrect.";
			case 160:
				return "The argument string passed to DosExecPgm is incorrect.";
			case 161:
				return "The specified path name is invalid.";
			case 162:
				return "A signal is already pending.";
			case 164:
				return "No more threads can be created in the system.";
			case 167:
				return "Attempt to lock a region of a file failed.";
			case 170:
				return "The requested resource is in use.";
			case 173:
				return "A lock request was not outstanding for the supplied cancel region.";
			case 174:
				return "The file system does not support atomic changing of the lock type.";
			case 180:
				return "The system detected a segment number that is incorrect.";
			case 182:
				return "The operating system cannot run %1.";
			case 183:
				return "Attempt to create file that already exists.";
			case 186:
				return "The flag passed is incorrect.";
			case 187:
				return "The specified system semaphore name was not found.";
			case 188:
				return "The operating system cannot run %1.";
			case 189:
				return "The operating system cannot run %1.";
			case 190:
				return "The operating system cannot run %1.";
			case 191:
				return "%1 cannot be run in Windows/NT mode.";
			case 192:
				return "The operating system cannot run %1.";
			case 193:
				return "%1 is not a valid Windows-based application.";
			case 194:
				return "The operating system cannot run %1.";
			case 195:
				return "The operating system cannot run %1.";
			case 196:
				return "The operating system cannot run this application program.";
			case 197:
				return "The operating system is not presently configured to run this application.";
			case 198:
				return "The operating system cannot run %1.";
			case 199:
				return "The operating system cannot run this application program.";
			case 200:
				return "The code segment cannot be greater than or equal to 64KB.";
			case 201:
				return "The operating system cannot run %1.";
			case 202:
				return "The operating system cannot run %1.";
			case 203:
				return "The system could not find the environment option entered.";
			case 205:
				return "No process in the command subtree has a signal handler.";
			case 206:
				return "The file name or extension is too long.";
			case 207:
				return "The ring 2 stack is in use.";
			case 208:
				return "The global filename characters * or ? are entered incorrectly, or too many global filename characters are specified.";
			case 209:
				return "The signal being posted is incorrect.";
			case 210:
				return "The signal handler cannot be set.";
			case 212:
				return "The segment is locked and cannot be reallocated.";
			case 214:
				return "Too many dynamic link modules are attached to this program or dynamic link module.";
			case 215:
				return "Can't nest calls to LoadModule.";
			case 230:
				return "The pipe state is invalid.";
			case 231:
				return "All pipe instances busy.";
			case 232:
				return "Pipe close in progress.";
			case 233:
				return "No process on other end of pipe.";
			case 234:
				return "More data is available.";
			case 240:
				return "The session was canceled.";
			case 254:
				return "The specified EA name is invalid.";
			case 255:
				return "The EAs are inconsistent.";
			case 259:
				return "No more data is available.";
			case 266:
				return "The Copy API cannot be used.";
			case 267:
				return "The directory name is invalid.";
			case 275:
				return "The EAs did not fit in the buffer.";
			case 276:
				return "The EA file on the mounted file system is damaged.";
			case 277:
				return "The EA table in the EA file on the mounted file system is full.";
			case 278:
				return "The specified EA handle is invalid.";
			case 282:
				return "The mounted file system does not support extended attributes.";
			case 288:
				return "Attempt to release mutex not owned by caller.";
			case 298:
				return "Too many posts made to a semaphore.";
			case 317:
				return "The system cannot find message for message number 0x%1 in message file for %2.";
			case 487:
				return "Attempt to access invalid address.";
			case 534:
				return "Arithmetic result exceeded 32-bits.";
			case 535:
				return "There is a process on other end of the pipe.";
			case 536:
				return "Waiting for a process to open the other end of the pipe.";
			case 994:
				return "Access to the EA is denied.";
			case 995:
				return "The I/O operation was aborted due to either thread exit or application request.";
			case 996:
				return "Overlapped IO event not in signaled state.";
			case 997:
				return "Overlapped IO operation in progress.";
			case 998:
				return "Invalid access to memory location.";
			case 999:
				return "ERROR accessing paging file.";
			case 1001:
				return "Recursion too deep, stack overflowed.";
			case 1002:
				return "Window can't handle sent message.";
			case 1003:
				return "Cannot complete function for some reason.";
			case 1004:
				return "The flags are invalid.";
			case 1005:
				return "The volume does not contain a recognized file system. Make sure that all required file system drivers are loaded and the volume is not "
					   "damaged.";
			case 1006:
				return "The volume for a file was externally altered and the opened file is no longer valid.";
			case 1007:
				return "The requested operation cannot be performed in full-screen mode.";
			case 1008:
				return "An attempt was made to reference a token that does not exist.";
			case 1009:
				return "The configuration registry database is damaged.";
			case 1010:
				return "The configuration registry key is invalid.";
			case 1011:
				return "The configuration registry key cannot be opened.";
			case 1012:
				return "The configuration registry key cannot be read.";
			case 1013:
				return "The configuration registry key cannot be written.";
			case 1014:
				return "One of the files containing the system's registry data had to be recovered by use of a log or alternate copy. The recovery succeeded.";
			case 1015:
				return "The registry is damaged. The structure of one of the files that contains registry data is damaged, or the system's in memory image of "
					   "the file is damaged, or the file could not be recovered because its alternate copy or log was absent or damaged.";
			case 1016:
				return "The registry initiated an I/O operation that had an unrecoverable failure. The registry could not read in, or write out, or flush, one "
					   "of the files that contain the system's image of the registry.";
			case 1017:
				return "The system attempted to load or restore a file into the registry, and the specified file is not in the format of a registry file.";
			case 1018:
				return "Illegal operation attempted on a registry key that has been marked for deletion.";
			case 1019:
				return "System could not allocate required space in a registry log.";
			case 1020:
				return "An attempt was made to create a symbolic link in a registry key that already has subkeys or values.";
			case 1021:
				return "An attempt was made to create a stable subkey under a volatile parent key.";
			case 1022:
				return "This indicates that a notify change request is being completed and the information is not being returned in the caller's buffer. The "
					   "caller now needs to enumerate the files to find the changes.";
			case 1051:
				return "A stop control has been sent to a service which other running services are dependent on.";
			case 1052:
				return "The requested control is not valid for this service";
			case 1053:
				return "The service did not respond to the start or control request in a timely fashion.";
			case 1054:
				return "A thread could not be created for the service.";
			case 1055:
				return "The service database is locked.";
			case 1056:
				return "An instance of the service is already running.";
			case 1057:
				return "The account name is invalid or does not exist.";
			case 1058:
				return "The specified service is disabled and cannot be started.";
			case 1059:
				return "Circular service dependency was specified.";
			case 1060:
				return "The specified service does not exist as an installed service.";
			case 1061:
				return "The service cannot accept control messages at this time.";
			case 1062:
				return "The service has not been started.";
			case 1063:
				return "The service process could not connect to the service controller.";
			case 1064:
				return "An exception occurred in the service when handling the control request.";
			case 1065:
				return "The database specified does not exist.";
			case 1066:
				return "The service has returned a service-specific ERROR code.";
			case 1067:
				return "The process terminated unexpectedly.";
			case 1068:
				return "The dependency service or group failed to start.";
			case 1069:
				return "The service did not start due to a logon failure.";
			case 1070:
				return "After starting, the service hung in a start-pending state.";
			case 1071:
				return "The specified service database lock is invalid.";
			case 1072:
				return "The specified service has been marked for deletion.";
			case 1073:
				return "The specified service already exists.";
			case 1074:
				return "The system is currently running with the last-known-good configuration.";
			case 1075:
				return "The dependency service does not exist or has been marked for deletion.";
			case 1076:
				return "The current boot has already been accepted for use as the last-known-good control set.";
			case 1077:
				return "No attempts to start the service have been made since the last boot.";
			case 1078:
				return "The name is already in use as either a service name or a service display name.";
			case 1100:
				return "End of tape mark was reached during an operation.";
			case 1101:
				return "A tape access reached a filemark.";
			case 1102:
				return "The beginning of the tape or partition was encountered.";
			case 1103:
				return "A tape access reached a setmark.";
			case 1104:
				return "During a tape access, the end of the data marker was reached.";
			case 1105:
				return "Tape could not be partitioned.";
			case 1106:
				return "When accessing a new tape of a multivolume partition, the current block size is incorrect.";
			case 1107:
				return "Tape partition information could not be found when loading a tape.";
			case 1108:
				return "Attempt to lock the eject media mechanism failed.";
			case 1109:
				return "Unload media failed.";
			case 1110:
				return "Media in drive may have changed.";
			case 1111:
				return "The I/O bus was reset.";
			case 1112:
				return "Tape query failed because of no media in drive.";
			case 1113:
				return "No mapping for the Unicode character exists in the target multi-byte code page.";
			case 1114:
				return "A DLL initialization routine failed.";
			case 1115:
				return "A system shutdown is in progress.";
			case 1116:
				return "An attempt to abort the shutdown of the system failed because no shutdown was in progress.";
			case 1117:
				return "The request could not be performed because of an I/O device ERROR.";
			case 1118:
				return "No serial device was successfully initialized. The serial driver will unload.";
			case 1119:
				return "Unable to open a device that was sharing an interrupt request (IRQ) with other devices. At least one other device that uses that IRQ "
					   "was already opened.";
			case 1120:
				return "A serial I/O operation was completed by another write to the serial port. (The IOCTL_SERIAL_XOFF_COUNTER reached zero).";
			case 1121:
				return "A serial I/O operation completed because the time-out period expired. (The IOCTL_SERIAL_XOFF_COUNTER did not reach zero.)";
			case 1122:
				return "No ID address mark was found on the floppy disk.";
			case 1123:
				return "Mismatch between the floppy disk sector ID field and the floppy disk controller track address.";
			case 1124:
				return "The floppy disk controller reported an ERROR that is not recognized by the floppy disk driver.";
			case 1125:
				return "The floppy disk controller returned inconsistent results in its registers.";
			case 1126:
				return "While accessing the hard disk, a recalibrate operation failed, even after retries.";
			case 1127:
				return "While accessing the hard disk, a disk operation failed even after retries.";
			case 1128:
				return "While accessing the hard disk, a disk controller reset was needed, but even that failed.";
			case 1129:
				return "Physical end of tape encountered.";
			case 1130:
				return "Not enough server storage is available to process this command.";
			case 1131:
				return "A potential deadlock condition has been detected.";
			case 1200:
				return "The specified device name is invalid.";
			case 1201:
				return "The device is not currently connected but is a remembered connection.";
			case 1202:
				return "An attempt was made to remember a device that was previously remembered.";
			case 1203:
				return "No network provider accepted the given network path.";
			case 1204:
				return "The specified network provider name is invalid.";
			case 1205:
				return "Unable to open the network connection profile.";
			case 1206:
				return "The network connection profile is damaged.";
			case 1207:
				return "Cannot enumerate a non-container.";
			case 1208:
				return "An extended ERROR has occurred.";
			case 1209:
				return "The format of the specified group name is invalid.";
			case 1210:
				return "The format of the specified computer name is invalid.";
			case 1211:
				return "The format of the specified event name is invalid.";
			case 1212:
				return "The format of the specified domain name is invalid.";
			case 1213:
				return "The format of the specified service name is invalid.";
			case 1214:
				return "The format of the specified network name is invalid.";
			case 1215:
				return "The format of the specified share name is invalid.";
			case 1216:
				return "The format of the specified password is invalid.";
			case 1217:
				return "The format of the specified message name is invalid.";
			case 1218:
				return "The format of the specified message destination is invalid.";
			case 1219:
				return "The credentials supplied conflict with an existing set of credentials.";
			case 1220:
				return "An attempt was made to establish a session to a LAN Manager server, but there are already too many sessions established to that "
					   "server.";
			case 1221:
				return "The workgroup or domain name is already in use by another computer on the network.";
			case 1222:
				return "The network is not present or not started.";
			case 1300:
				return "Indicates not all privileges referenced are assigned to the caller. This allows, for example, all privileges to be disabled without "
					   "having to know exactly which privileges are assigned.";
			case 1301:
				return "Some of the information to be mapped has not been translated.";
			case 1302:
				return "No system quota limits are specifically set for this account.";
			case 1303:
				return "A user session key was requested for a local RPC connection. The session key returned is a constant value and not unique to this "
					   "connection.";
			case 1304:
				return "The Windows NT password is too complex to be converted to a Windows-networking password. The Windows-networking password returned is a "
					   "NULL string.";
			case 1305:
				return "Indicates an encountered or specified revision number is not one known by the service. The service may not be aware of a more recent "
					   "revision.";
			case 1306:
				return "Indicates two revision levels are incompatible.";
			case 1307:
				return "Indicates a particular Security ID cannot be assigned as the owner of an object.";
			case 1308:
				return "Indicates a particular Security ID cannot be assigned as the primary group of an object.";
			case 1309:
				return "An attempt was made to operate on an impersonation token by a thread was not currently impersonating a client.";
			case 1310:
				return "A mandatory group cannot be disabled.";
			case 1311:
				return "There are currently no logon servers available to service the logon request.";
			case 1312:
				return "A specified logon session does not exist. It may already have been terminated.";
			case 1313:
				return "A specified privilege does not exist.";
			case 1314:
				return "A required privilege is not held by the client.";
			case 1315:
				return "The name provided is not a properly formed account name.";
			case 1316:
				return "The specified user already exists.";
			case 1317:
				return "The specified user does not exist.";
			case 1318:
				return "The specified group already exists.";
			case 1319:
				return "The specified group does not exist.";
			case 1320:
				return "The specified user account is already in the specified group account. Also used to indicate a group can not be deleted because it "
					   "contains a member.";
			case 1321:
				return "The specified user account is not a member of the specified group account.";
			case 1322:
				return "Indicates the requested operation would disable or delete the last remaining administration account. This is not allowed to prevent "
					   "creating a situation where the system will not be administrable.";
			case 1323:
				return "When trying to update a password, this return status indicates the value provided as the current password is incorrect.";
			case 1324:
				return "When trying to update a password, this return status indicates the value provided for the new password contains values not allowed in "
					   "passwords.";
			case 1325:
				return "When trying to update a password, this status indicates that some password update rule was violated. For example, the password may not "
					   "meet length criteria.";
			case 1326:
				return "The attempted logon is invalid. This is due to either a bad user name or authentication information.";
			case 1327:
				return "Indicates a referenced user name and authentication information are valid, but some user account restriction has prevented successful "
					   "authentication (such as time-of-day restrictions.";
			case 1328:
				return "The user account has time restrictions and cannot be logged onto at this time.";
			case 1329:
				return "The user account is restricted and cannot be used to log on from the source workstation.";
			case 1330:
				return "The user account's password has expired.";
			case 1331:
				return "The referenced account is currently disabled and cannot be logged on to.";
			case 1332:
				return "None of the information to be mapped has been translated.";
			case 1333:
				return "The number of LUID requested cannot be allocated with a single allocation.";
			case 1334:
				return "Indicates there are no more LUID to allocate.";
			case 1335:
				return "Indicates the sub-authority value is invalid for the particular use.";
			case 1336:
				return "Indicates the ACL structure is not valid.";
			case 1337:
				return "Indicates the SID structure is invalid.";
			case 1338:
				return "Indicates the SECURITY_DESCRIPTOR structure is invalid.";
			case 1340:
				return "Indicates that an attempt to build either an inherited ACL or ACE did not succeed. One of the more probable causes is the replacement "
					   "of a CreatorId with an SID that didn't fit into the ACE or ACL.";
			case 1341:
				return "The GUID allocation server is already disabled at the moment.";
			case 1342:
				return "The GUID allocation server is already enabled at the moment.";
			case 1343:
				return "The value provided is an invalid value for an identifier authority.";
			case 1344:
				return "When a block of memory is allotted for future updates, such as the memory allocated to hold discretionary access control and primary "
					   "group information, successive updates may exceed the amount of memory originally allotted. Since quota may already have been charged "
					   "to several processes that have handles of the object, it is not reasonable to alter the size of the allocated memory. Instead, a "
					   "request that requires more memory than has been allotted must fail and the ERROR_ALLOTTED_SPACE_EXCEEDED error returned.";
			case 1345:
				return "The specified attributes are invalid, or incompatible with the attributes for the group as a whole.";
			case 1346:
				return "A specified impersonation level is invalid. Also used to indicate a required impersonation level was not provided.";
			case 1347:
				return "An attempt was made to open an anonymous level token. Anonymous tokens cannot be opened.";
			case 1348:
				return "The requested validation information class is invalid.";
			case 1349:
				return "The type of token object is inappropriate for its attempted use.";
			case 1350:
				return "Indicates an attempt was made to operate on the security of an object that does not have security associated with it.";
			case 1351:
				return "Indicates a domain controller could not be contacted or that objects within the domain are protected and necessary information could "
					   "not be retrieved.";
			case 1352:
				return "Indicates the Sam Server was in the wrong state to perform the desired operation.";
			case 1353:
				return "Indicates the domain is in the wrong state to perform the desired operation.";
			case 1354:
				return "Indicates the requested operation cannot be completed with the domain in its present role.";
			case 1355:
				return "The specified domain does not exist.";
			case 1356:
				return "The specified domain already exists.";
			case 1357:
				return "An attempt to exceed the limit on the number of domains per server for this release.";
			case 1358:
				return "This ERROR indicates the requested operation cannot be completed due to a catastrophic media failure or on-disk data structure "
					   "corruption.";
			case 1359:
				return "This ERROR indicates the SAM server has encounterred an internal consistency error in its database. This catastrophic failure prevents "
					   "further operation of SAM.";
			case 1360:
				return "Indicates generic access types were contained in an access mask that should already be mapped to non-generic access types.";
			case 1361:
				return "Indicates a security descriptor is not in the required format (absolute or self-relative";
			case 1362:
				return "The requested action is restricted for use by logon processes only. The calling process has not registered as a logon process.";
			case 1363:
				return "An attempt was made to start a new session manager or LSA logon session with an ID already in use.";
			case 1364:
				return "A specified authentication package is unknown.";
			case 1365:
				return "The logon session is not in a state consistent with the requested operation.";
			case 1366:
				return "The logon session ID is already in use.";
			case 1367:
				return "Indicates an invalid value has been provided for LogonType has been requested.";
			case 1368:
				return "Indicates that an attempt was made to impersonate via a named pipe was not yet read from.";
			case 1369:
				return "Indicates that the transaction state of a registry sub-tree is incompatible with the requested operation. For example, a request has "
					   "been made to start a new transaction with one already in progress, or a request to apply a transaction when one is not currently in "
					   "progress. This status value is returned by the runtime library (RTL) registry transaction package (RXact).";
			case 1370:
				return "Indicates an ERROR occurred during a registry transaction commit. The database has been left in an unknown state. The state of the "
					   "registry transaction is left as COMMITTING. This status value is returned by the runtime library (RTL) registry transaction package "
					   "(RXact).";
			case 1371:
				return "Indicates an operation was attempted on a built-in (special) SAM account that is incompatible with built-in accounts. For example, "
					   "built-in accounts cannot be renamed or deleted.";
			case 1372:
				return "The requested operation cannot be performed on the specified group because it is a built-in special group.";
			case 1373:
				return "The requested operation cannot be performed on the specified user because it is a built-in special user.";
			case 1374:
				return "Indicates a member cannot be removed from a group because the group is currently the member's primary group.";
			case 1375:
				return "An attempt was made to establish a token for use as a primary token but the token is already in use. A token can only be the primary "
					   "token of one process at a time.";
			case 1376:
				return "The specified alias does not exist.";
			case 1377:
				return "The specified account name is not a member of the alias.";
			case 1378:
				return "The specified account name is not a member of the alias.";
			case 1379:
				return "The specified alias already exists.";
			case 1380:
				return "A requested type of logon, such as Interactive, Network, or Service, is not granted by the target system's local security policy. The "
					   "system administrator can grant the required form of logon.";
			case 1381:
				return "The maximum number of secrets that can be stored in a single system was exceeded. The length and number of secrets is limited to "
					   "satisfy the United States State Department export restrictions.";
			case 1382:
				return "The length of a secret exceeds the maximum length allowed. The length and number of secrets is limited to satisfy the United States "
					   "State Department export restrictions.";
			case 1383:
				return "The Local Security Authority (LSA) database contains in internal inconsistency.";
			case 1384:
				return "During a logon attempt, the user's security context accumulated too many security IDs. Remove the user from some groups or aliases to "
					   "reduce the number of security ids to incorporate into the security context.";
			case 1385:
				return "A user has requested a type of logon, such as interactive or network, that was not granted. An administrator has control over who may "
					   "logon interactively and through the network.";
			case 1386:
				return "An attempt was made to change a user password in the security account manager without providing the necessary NT cross-encrypted "
					   "password.";
			case 1387:
				return "A new member cannot be added to an alias because the member does not exist.";
			case 1388:
				return "A new member could not be added to an alias because the member has the wrong account type.";
			case 1389:
				return "Too many SIDs specified.";
			case 1390:
				return "An attempt was made to change a user password in the security account manager without providing the required LM cross-encrypted "
					   "password.";
			case 1391:
				return "Indicates an ACL contains no inheritable components.";
			case 1392:
				return "The file or directory is damaged and nonreadable.";
			case 1393:
				return "The disk structure is damaged and nonreadable.";
			case 1394:
				return "There is no user session key for the specified logon session.";
			case 1400:
				return "The window handle invalid.";
			case 1401:
				return "The menu handle is invalid.";
			case 1402:
				return "The cursor handle is invalid.";
			case 1403:
				return "Invalid accelerator-table handle.";
			case 1404:
				return "The hook handle is invalid.";
			case 1405:
				return "The DeferWindowPos handle is invalid.";
			case 1406:
				return "CreateWindow failed, creating top-level window with WS_CHILD style.";
			case 1407:
				return "Cannot find window class.";
			case 1408:
				return "Invalid window, belongs to other thread.";
			case 1409:
				return "Hotkey is already registered.";
			case 1410:
				return "Class already exists.";
			case 1411:
				return "Class does not exist.";
			case 1412:
				return "Class still has open windows.";
			case 1413:
				return "The index is invalid.";
			case 1414:
				return "The icon handle is invalid.";
			case 1415:
				return "Using private DIALOG window words.";
			case 1416:
				return "List box ID not found.";
			case 1417:
				return "No wildcard characters found.";
			case 1418:
				return "Thread doesn't have clipboard open.";
			case 1419:
				return "Hotkey is not registered.";
			case 1420:
				return "The window is not a valid dialog window.";
			case 1421:
				return "Control ID not found.";
			case 1422:
				return "Invalid Message, combo box doesn't have an edit control.";
			case 1423:
				return "The window is not a combo box.";
			case 1424:
				return "Height must be less than 256.";
			case 1425:
				return "Invalid HDC passed to ReleaseDC.";
			case 1426:
				return "The hook filter type is invalid.";
			case 1427:
				return "The filter proc is invalid.";
			case 1428:
				return "Cannot set non-local hook without an module handle.";
			case 1429:
				return "This hook can only be set globally.";
			case 1430:
				return "The journal hook is already installed.";
			case 1431:
				return "Hook is not installed.";
			case 1432:
				return "The message for single-selection list box is invalid.";
			case 1433:
				return "LB_SETCOUNT sent to non-lazy list box.";
			case 1434:
				return "This list box doesn't support tab stops.";
			case 1435:
				return "Cannot destroy object created by another thread.";
			case 1436:
				return "Child windows can't have menus.";
			case 1437:
				return "Window does not have system menu.";
			case 1438:
				return "The message box style is invalid.";
			case 1439:
				return "The SPI_* parameter is invalid.";
			case 1440:
				return "Screen already locked.";
			case 1442:
				return "Window is not a child window.";
			case 1443:
				return "The GW_* command is invalid.";
			case 1444:
				return "The thread ID is invalid.";
			case 1445:
				return "DefMDIChildProc called with a non-MDI child window.";
			case 1446:
				return "Pop-up menu already active.";
			case 1447:
				return "Window does not have scroll bars.";
			case 1448:
				return "Scrollbar range greater than 0x7FFF.";
			case 1449:
				return "The ShowWindow command is invalid.";
			case 1500:
				return "One of the Eventlog logfiles is damaged.";
			case 1501:
				return "No event log file could be opened, so the event logging service did not start.";
			case 1502:
				return "The event log file is full.";
			case 1503:
				return "The event log file has changed between reads.";
			case 1751:
				return "The entry is invalid.";
			case 1752:
				return "The operation cannot be performed.";
			case 1753:
				return "There are no more endpoints available from the endpoint mapper.";
			case 1784:
				return "The supplied user buffer is invalid for the requested operation.";
			case 1785:
				return "The disk media is not recognized. It may not be formatted.";
			case 1786:
				return "The workstation does not have a trust secret.";
			case 1787:
				return "The domain controller does not have an account for this workstation.";
			case 1788:
				return "The trust relationship between the primary domain and the trusted domain failed.";
			case 1789:
				return "The trust relationship between this workstation and the primary domain failed.";
			case 1790:
				return "The network logon failed.";
			case 1792:
				return "An attempt was made to logon, but the network logon service was not started.";
			case 1793:
				return "The user's account has expired.";
			case 1794:
				return "The redirector is in use and cannot be unloaded.";
			case 1795:
				return "The specified printer driver is already installed.";
			case 1796:
				return "The specified port is unknown.";
			case 1797:
				return "The printer driver is unknown.";
			case 1798:
				return "The print processor is unknown.";
			case 1799:
				return "The specified separator file is invalid.";
			case 1800:
				return "The specified priority is invalid.";
			case 1801:
				return "The printer name is invalid.";
			case 1802:
				return "The printer already exists.";
			case 1803:
				return "The printer command is invalid.";
			case 1804:
				return "The specified datatype is invalid.";
			case 1805:
				return "The Environment specified is invalid.";
			case 1807:
				return "The account used is an interdomain trust account. Use your normal user account or remote user account to access this server.";
			case 1808:
				return "The account used is a workstation trust account. Use your normal user account or remote user account to access this server.";
			case 1809:
				return "The account used is an server trust account. Use your normal user account or remote user account to access this server.";
			case 1810:
				return "The name or security ID (SID) of the domain specified is inconsistent with the trust information for that domain.";
			case 1811:
				return "The server is in use and cannot be unloaded.";
			case 1812:
				return "The specified image file did not contain a resource section.";
			case 1813:
				return "The specified resource type can not be found in the image file.";
			case 1814:
				return "The specified resource name can not be found in the image file.";
			case 1815:
				return "The specified resource language ID cannot be found in the image file.";
			case 1816:
				return "Not enough quota is available to process this command.";
			case 1899:
				return "The endpoint mapper database could not be created.";
			case 2202:
				return "The specified user name is invalid.";
			case 2250:
				return "This network connection does not exist.";
			case 2401:
				return "There are open files or requests pending on this connection.";
			case 2404:
				return "The device is in use by an active process and cannot be disconnected.";
			case 6118:
				return "The list of servers for this workgroup is not currently available";
			default:
				return "Unrecognized error value.";
		}
	}

	// FUNCTION: TOY2 0x0040C130
	void LogErrorNotSetLn(char* format, ...)
	{
		CHAR buffer[256];
		va_list va;

		va_start(va, format);
		buffer[0] = '\0';
		sprintf(buffer, format, va);

		lstrcatA(buffer, "\r\n");
		lstrcpyA(g_errorNotSet, buffer);

		Logger::Log(buffer);
	}

	// FUNCTION: TOY2 0x004ADFF0
	char* D3DAppErrorToString(HRESULT error)
	{
		switch (error)
		{
			case DD_OK:
				return "The request completed successfully.";
			case E_NOTIMPL:
				return "The operation is not supported.";
			case E_FAIL:
				return "There is an undefined error condition.";
			case CO_E_NOTINITIALIZED:
				return "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.";
			case E_OUTOFMEMORY:
				return "DirectDraw does not have enough memory to perform the operation.";
			case E_INVALIDARG:
				return "One or more of the parameters passed to the method are incorrect.";
			case DDERR_ALREADYINITIALIZED:
				return "The object has already been initialized.";
			case DDERR_CANNOTATTACHSURFACE:
				return "A surface cannot be attached to another requested surface.";
			case DDERR_CANNOTDETACHSURFACE:
				return "A surface cannot be detached from another requested surface.";
			case DDERR_CURRENTLYNOTAVAIL:
				return "No support is currently available.";
			case DDERR_EXCEPTION:
				return "An exception was encountered while performing the requested operation.";
			case DDERR_HEIGHTALIGN:
				return "The height of the provided rectangle is not a multiple of the required alignment.";
			case DDERR_INCOMPATIBLEPRIMARY:
				return "The primary surface creation request does not match with the existing primary surface.";
			case DDERR_INVALIDCAPS:
				return "One or more of the capability bits passed to the callback function are incorrect.";
			case DDERR_INVALIDCLIPLIST:
				return "DirectDraw does not support the provided clip list.";
			case DDERR_INVALIDMODE:
				return "DirectDraw does not support the requested mode.";
			case DDERR_INVALIDOBJECT:
				return "DirectDraw received a pointer that was an invalid DirectDraw object.";
			case DDERR_INVALIDPIXELFORMAT:
				return "The pixel format was invalid as specified.";
			case DDERR_INVALIDRECT:
				return "The provided rectangle was invalid.";
			case DDERR_LOCKEDSURFACES:
				return "One or more surfaces are locked, causing the failure of the requested operation.";
			case DDERR_NO3D:
				return "No 3-D hardware or emulation is present.";
			case DDERR_NOALPHAHW:
				return "No alpha acceleration hardware is present or available, causing the failure of the requested operation.";
			case DDERR_NOCLIPLIST:
				return "No clip list is available.";
			case DDERR_NOCOLORCONVHW:
				return "The operation cannot be carried out because no color-conversion hardware is present or available.";
			case DDERR_NOCOOPERATIVELEVELSET:
				return "A create function is called without the IDirectDraw4::SetCooperativeLevel method being called.";
			case DDERR_NOCOLORKEY:
				return "The surface does not currently have a color key.";
			case DDERR_NOCOLORKEYHW:
				return "The operation cannot be carried out because there is no hardware support for the destination color key.";
			case DDERR_NODIRECTDRAWSUPPORT:
				return "DirectDraw support is not possible with the current display driver.";
			case DDERR_NOEXCLUSIVEMODE:
				return "The operation requires the application to have exclusive mode, but the application does not have exclusive mode.";
			case DDERR_NOFLIPHW:
				return "Flipping visible surfaces is not supported.";
			case DDERR_NOGDI:
				return "No GDI is present.";
			case DDERR_NOMIRRORHW:
				return "The operation cannot be carried out because no mirroring hardware is present or available.";
			case DDERR_NOTFOUND:
				return "The requested item was not found.";
			case DDERR_NOOVERLAYHW:
				return "The operation cannot be carried out because no overlay hardware is present or available.";
			case DDERR_OVERLAPPINGRECTS:
				return "Operation could not be carried out because the source and destination rectangles are on the same surface and overlap each other.";
			case DDERR_NORASTEROPHW:
				return "The operation cannot be carried out because no appropriate raster operation hardware is present or available.";
			case DDERR_NOROTATIONHW:
				return "The operation cannot be carried out because no rotation hardware is present or available.";
			case DDERR_NOSTRETCHHW:
				return "The operation cannot be carried out because there is no hardware support for stretching.";
			case DDERR_NOT4BITCOLOR:
				return "The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette.";
			case DDERR_NOT4BITCOLORINDEX:
				return "The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index "
					   "palette.";
			case DDERR_NOT8BITCOLOR:
				return "The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette.";
			case DDERR_NOTEXTUREHW:
				return "The operation cannot be carried out because no texture-mapping hardware is present or available.";
			case DDERR_NOVSYNCHW:
				return "The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations.";
			case DDERR_NOZBUFFERHW:
				return "The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no "
					   "hardware support for z-buffers.";
			case DDERR_NOZOVERLAYHW:
				return "The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays.";
			case DDERR_OUTOFCAPS:
				return "The hardware needed for the requested operation has already been allocated.";
			case DDERR_OUTOFVIDEOMEMORY:
				return "DirectDraw does not have enough display memory to perform the operation.";
			case DDERR_OVERLAYCANTCLIP:
				return "The hardware does not support clipped overlays.";
			case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
				return "An attempt was made to have more than one color key active on an overlay.";
			case DDERR_PALETTEBUSY:
				return "Access to this palette is refused because the palette is locked by another thread.";
			case DDERR_COLORKEYNOTSET:
				return "No source color key is specified for this operation.";
			case DDERR_SURFACEALREADYATTACHED:
				return "An attempt was made to attach a surface to another surface to which it is already attached.";
			case DDERR_SURFACEALREADYDEPENDENT:
				return "An attempt was made to make a surface a dependency of another surface to which it is already dependent.";
			case DDERR_SURFACEBUSY:
				return "Access to the surface is refused because the surface is locked by another thread.";
			case DDERR_CANTLOCKSURFACE:
				return "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.";
			case DDERR_SURFACEISOBSCURED:
				return "Access to the surface is refused because the surface is obscured.";
			case DDERR_SURFACELOST:
				return "Access to the surface is refused because the surface memory is gone. Call the IDirectDrawSurface4::Restore method on this surface to "
					   "restore the memory associated with it.";
			case DDERR_SURFACENOTATTACHED:
				return "The requested surface is not attached.";
			case DDERR_TOOBIGHEIGHT:
				return "The height requested by DirectDraw is too large.";
			case DDERR_TOOBIGSIZE:
				return "The size requested by DirectDraw is too large. However, the individual height and width are valid sizes.";
			case DDERR_TOOBIGWIDTH:
				return "The width requested by DirectDraw is too large.";
			case DDERR_UNSUPPORTEDFORMAT:
				return "The FourCC format requested is not supported by DirectDraw.";
			case DDERR_UNSUPPORTEDMASK:
				return "The bitmask in the pixel format requested is not supported by DirectDraw.";
			case DDERR_INVALIDSTREAM:
				return "The specified stream contains invalid data.";
			case DDERR_VERTICALBLANKINPROGRESS:
				return "A vertical blank is in progress.";
			case DDERR_WASSTILLDRAWING:
				return "The previous blit operation that is transferring information to or from this surface is incomplete.";
			case DDERR_XALIGN:
				return "The provided rectangle was not horizontally aligned on a required boundary.";
			case DDERR_INVALIDDIRECTDRAWGUID:
				return "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.";
			case DDERR_DIRECTDRAWALREADYCREATED:
				return "A DirectDraw object representing this driver has already been created for this process.";
			case DDERR_NODIRECTDRAWHW:
				return "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.";
			case DDERR_PRIMARYSURFACEALREADYEXISTS:
				return "This process has already created a primary surface.";
			case DDERR_NOEMULATION:
				return "Software emulation is not available.";
			case DDERR_REGIONTOOSMALL:
				return "The region passed to the IDirectDrawClipper::GetClipList method is too small.";
			case DDERR_CLIPPERISUSINGHWND:
				return "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.";
			case DDERR_NOCLIPPERATTACHED:
				return "No DirectDrawClipper object is attached to the surface object.";
			case DDERR_NOHWND:
				return "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle.";
			case DDERR_HWNDSUBCLASSED:
				return "DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed.";
			case DDERR_HWNDALREADYSET:
				return "The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes "
					   "created.";
			case DDERR_NOPALETTEATTACHED:
				return "No palette object is attached to this surface.";
			case DDERR_NOPALETTEHW:
				return "There is no hardware support for 16- or 256-color palettes.";
			case DDERR_BLTFASTCANTCLIP:
				return "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface4::BltFast method.";
			case DDERR_NOBLTHW:
				return "No blitter hardware is present.";
			case DDERR_NODDROPSHW:
				return "No DirectDraw raster operation (ROP) hardware is available.";
			case DDERR_OVERLAYNOTVISIBLE:
				return "The IDirectDrawSurface4::GetOverlayPosition method is called on a hidden overlay.";
			case DDERR_NOOVERLAYDEST:
				return "The IDirectDrawSurface4::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface4::UpdateOverlay method has not "
					   "been called on to establish a destination.";
			case DDERR_INVALIDPOSITION:
				return "The position of the overlay on the destination is no longer legal.";
			case DDERR_NOTAOVERLAYSURFACE:
				return "An overlay component is called for a non-overlay surface.";
			case DDERR_EXCLUSIVEMODEALREADYSET:
				return "An attempt was made to set the cooperative level when it was already set to exclusive.";
			case DDERR_NOTFLIPPABLE:
				return "An attempt has been made to flip a surface that cannot be flipped.";
			case DDERR_CANTDUPLICATE:
				return "Primary and 3-D surfaces, or surfaces that are implicitly created, cannot be duplicated.";
			case DDERR_NOTLOCKED:
				return "An attempt is made to unlock a surface that was not locked.";
			case DDERR_CANTCREATEDC:
				return "Windows can not create any more device contexts (DCs), or a DC was requested for a palette-indexed surface when the surface had no "
					   "palette and the display mode was not palette-indexed (in this case DirectDraw cannot select a proper palette into the DC).";
			case DDERR_NODC:
				return "No DC has ever been created for this surface.";
			case DDERR_WRONGMODE:
				return "This surface cannot be restored because it was created in a different mode.";
			case DDERR_IMPLICITLYCREATED:
				return "The surface cannot be restored because it is an implicitly created surface.";
			case DDERR_NOTPALETTIZED:
				return "The surface being used is not a palette-based surface.";
			case DDERR_UNSUPPORTEDMODE:
				return "The display is currently in an unsupported mode.";
			case DDERR_NOMIPMAPHW:
				return "The operation cannot be carried out because no mipmap capable texture mapping hardware is present or available.";
			case DDERR_INVALIDSURFACETYPE:
				return "The requested operation could not be performed because the surface was of the wrong type.";
			case DDERR_NOOPTIMIZEHW:
				return "The device does not support optimized surfaces.";
			case DDERR_NOTLOADED:
				return "The surface is an optimized surface, but it has not yet been allocated any memory.";
			case DDERR_NOFOCUSWINDOW:
				return "An attempt was made to create or set a device window without first setting the focus window.";
			case DDERR_DCALREADYCREATED:
				return "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.";
			case DDERR_NONONLOCALVIDMEM:
				return "An attempt was made to allocate non-local video memory from a device that does not support non-local video memory.";
			case DDERR_CANTPAGELOCK:
				return "An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface.";
			case DDERR_CANTPAGEUNLOCK:
				return "An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface.";
			case DDERR_NOTPAGELOCKED:
				return "An attempt is made to page unlock a surface with no outstanding page locks.";
			case DDERR_MOREDATA:
				return "There is more data available than the specified buffer size can hold.";
			case DDERR_EXPIRED:
				return "The data has expired and is therefore no longer valid.";
			case DDERR_VIDEONOTACTIVE:
				return "The video port is not active.";
			case DDERR_DEVICEDOESNTOWNSURFACE:
				return "Surfaces created by one DirectDraw device cannot be used directly by another DirectDraw device.";
			case D3DERR_BADMAJORVERSION:
				return "D3DERR_BADMAJORVERSION";
			case D3DERR_BADMINORVERSION:
				return "D3DERR_BADMINORVERSION";
			case D3DERR_INVALID_DEVICE:
				return "D3DERR_INVALID_DEVICE";
			case D3DERR_INITFAILED:
				return "D3DERR_INITFAILED";
			case D3DERR_DEVICEAGGREGATED:
				return "D3DERR_DEVICEAGGREGATED";
			case D3DERR_EXECUTE_CREATE_FAILED:
				return "D3DERR_EXECUTE_CREATE_FAILED";
			case D3DERR_EXECUTE_DESTROY_FAILED:
				return "D3DERR_EXECUTE_DESTROY_FAILED";
			case D3DERR_EXECUTE_LOCK_FAILED:
				return "D3DERR_EXECUTE_LOCK_FAILED";
			case D3DERR_EXECUTE_UNLOCK_FAILED:
				return "D3DERR_EXECUTE_UNLOCK_FAILED";
			case D3DERR_EXECUTE_LOCKED:
				return "D3DERR_EXECUTE_LOCKED";
			case D3DERR_EXECUTE_NOT_LOCKED:
				return "D3DERR_EXECUTE_NOT_LOCKED";
			case D3DERR_EXECUTE_FAILED:
				return "D3DERR_EXECUTE_FAILED";
			case D3DERR_EXECUTE_CLIPPED_FAILED:
				return "D3DERR_EXECUTE_CLIPPED_FAILED";
			case D3DERR_TEXTURE_NO_SUPPORT:
				return "D3DERR_TEXTURE_NO_SUPPORT";
			case D3DERR_TEXTURE_CREATE_FAILED:
				return "D3DERR_TEXTURE_CREATE_FAILED";
			case D3DERR_TEXTURE_DESTROY_FAILED:
				return "D3DERR_TEXTURE_DESTROY_FAILED";
			case D3DERR_TEXTURE_LOCK_FAILED:
				return "D3DERR_TEXTURE_LOCK_FAILED";
			case D3DERR_TEXTURE_UNLOCK_FAILED:
				return "D3DERR_TEXTURE_UNLOCK_FAILED";
			case D3DERR_TEXTURE_LOAD_FAILED:
				return "D3DERR_TEXTURE_LOAD_FAILED";
			case D3DERR_TEXTURE_SWAP_FAILED:
				return "D3DERR_TEXTURE_SWAP_FAILED";
			case D3DERR_TEXTURE_LOCKED:
				return "D3DERR_TEXTURE_LOCKED";
			case D3DERR_TEXTURE_NOT_LOCKED:
				return "D3DERR_TEXTURE_NOT_LOCKED";
			case D3DERR_TEXTURE_GETSURF_FAILED:
				return "D3DERR_TEXTURE_GETSURF_FAILED";
			case D3DERR_MATRIX_CREATE_FAILED:
				return "D3DERR_MATRIX_CREATE_FAILED";
			case D3DERR_MATRIX_DESTROY_FAILED:
				return "D3DERR_MATRIX_DESTROY_FAILED";
			case D3DERR_MATRIX_SETDATA_FAILED:
				return "D3DERR_MATRIX_SETDATA_FAILED";
			case D3DERR_MATRIX_GETDATA_FAILED:
				return "D3DERR_MATRIX_GETDATA_FAILED";
			case D3DERR_SETVIEWPORTDATA_FAILED:
				return "D3DERR_SETVIEWPORTDATA_FAILED";
			case D3DERR_INVALIDCURRENTVIEWPORT:
				return "D3DERR_INVALIDCURRENTVIEWPORT";
			case D3DERR_INVALIDPRIMITIVETYPE:
				return "D3DERR_INVALIDPRIMITIVETYPE";
			case D3DERR_INVALIDVERTEXTYPE:
				return "D3DERR_INVALIDVERTEXTYPE";
			case D3DERR_TEXTURE_BADSIZE:
				return "D3DERR_TEXTURE_BADSIZE";
			case D3DERR_INVALIDRAMPTEXTURE:
				return "D3DERR_INVALIDRAMPTEXTURE";
			case D3DERR_MATERIAL_CREATE_FAILED:
				return "D3DERR_MATERIAL_CREATE_FAILED";
			case D3DERR_MATERIAL_DESTROY_FAILED:
				return "D3DERR_MATERIAL_DESTROY_FAILED";
			case D3DERR_MATERIAL_SETDATA_FAILED:
				return "D3DERR_MATERIAL_SETDATA_FAILED";
			case D3DERR_MATERIAL_GETDATA_FAILED:
				return "D3DERR_MATERIAL_GETDATA_FAILED";
			case D3DERR_INVALIDPALETTE:
				return "D3DERR_INVALIDPALETTE";
			case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
				return "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY";
			case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
				return "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY";
			case D3DERR_SURFACENOTINVIDMEM:
				return "D3DERR_SURFACENOTINVIDMEM";
			case D3DERR_LIGHT_SET_FAILED:
				return "D3DERR_LIGHT_SET_FAILED";
			case D3DERR_LIGHTHASVIEWPORT:
				return "D3DERR_LIGHTHASVIEWPORT";
			case D3DERR_LIGHTNOTINTHISVIEWPORT:
				return "D3DERR_LIGHTNOTINTHISVIEWPORT";
			case D3DERR_SCENE_IN_SCENE:
				return "D3DERR_SCENE_IN_SCENE";
			case D3DERR_SCENE_NOT_IN_SCENE:
				return "D3DERR_SCENE_NOT_IN_SCENE";
			case D3DERR_SCENE_BEGIN_FAILED:
				return "D3DERR_SCENE_BEGIN_FAILED";
			case D3DERR_SCENE_END_FAILED:
				return "D3DERR_SCENE_END_FAILED";
			case D3DERR_INBEGIN:
				return "D3DERR_INBEGIN";
			case D3DERR_NOTINBEGIN:
				return "D3DERR_NOTINBEGIN";
			case D3DERR_NOVIEWPORTS:
				return "D3DERR_NOVIEWPORTS";
			case D3DERR_VIEWPORTDATANOTSET:
				return "D3DERR_VIEWPORTDATANOTSET";
			case D3DERR_VIEWPORTHASNODEVICE:
				return "D3DERR_VIEWPORTHASNODEVICE";
			case D3DERR_NOCURRENTVIEWPORT:
				return "D3DERR_NOCURRENTVIEWPORT";
			case D3DERR_INVALIDVERTEXFORMAT:
				return "D3DERR_INVALIDVERTEXFORMAT";
			case D3DERR_COLORKEYATTACHED:
				return "D3DERR_COLORKEYATTACHED";
			case D3DERR_VERTEXBUFFEROPTIMIZED:
				return "D3DERR_VERTEXBUFFEROPTIMIZED";
			case D3DERR_VBUF_CREATE_FAILED:
				return "D3DERR_VBUF_CREATE_FAILED";
			case D3DERR_VERTEXBUFFERLOCKED:
				return "D3DERR_VERTEXBUFFERLOCKED";
			case D3DERR_ZBUFFER_NOTPRESENT:
				return "D3DERR_ZBUFFER_NOTPRESENT";
			case D3DERR_STENCILBUFFER_NOTPRESENT:
				return "D3DERR_STENCILBUFFER_NOTPRESENT";
			case D3DERR_WRONGTEXTUREFORMAT:
				return "D3DERR_WRONGTEXTUREFORMAT";
			case D3DERR_UNSUPPORTEDCOLOROPERATION:
				return "D3DERR_UNSUPPORTEDCOLOROPERATION";
			case D3DERR_UNSUPPORTEDCOLORARG:
				return "D3DERR_UNSUPPORTEDCOLORARG";
			case D3DERR_UNSUPPORTEDALPHAOPERATION:
				return "D3DERR_UNSUPPORTEDALPHAOPERATION";
			case D3DERR_UNSUPPORTEDALPHAARG:
				return "D3DERR_UNSUPPORTEDALPHAARG";
			case D3DERR_TOOMANYOPERATIONS:
				return "D3DERR_TOOMANYOPERATIONS";
			case D3DERR_CONFLICTINGTEXTUREFILTER:
				return "D3DERR_CONFLICTINGTEXTUREFILTER";
			case D3DERR_UNSUPPORTEDFACTORVALUE:
				return "D3DERR_UNSUPPORTEDFACTORVALUE";
			case D3DERR_CONFLICTINGRENDERSTATE:
				return "D3DERR_CONFLICTINGRENDERSTATE";
			case D3DERR_UNSUPPORTEDTEXTUREFILTER:
				return "D3DERR_UNSUPPORTEDTEXTUREFILTER";
			case D3DERR_TOOMANYPRIMITIVES:
				return "D3DERR_TOOMANYPRIMITIVES";
			case D3DERR_INVALIDMATRIX:
				return "D3DERR_INVALIDMATRIX";
			case D3DERR_CONFLICTINGTEXTUREPALETTE:
				return "D3DERR_CONFLICTINGTEXTUREPALETTE";
			default:
				return "Unknow value";
		}
	}

	// FUNCTION: TOY2 0x0040D2C0
	char* GetErrorNotSet() { return g_errorNotSet; }
}
