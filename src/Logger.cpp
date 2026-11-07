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
	uint16_t g_logResetPending = 1;

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
		printf("%s", buffer); // Addition
#endif

		{
			if (g_logResetPending)
			{
				g_logResetPending = 0;
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

	// FUNCTION: TOY2 0x004ADF50 [MATCHED]
	HRESULT _DbgOut(TCHAR* strFile, DWORD dwLine, HRESULT hr, TCHAR* strMsg)
	{
		TCHAR buffer[256];
		sprintf(buffer, "%s(%ld): ", strFile, dwLine);
		OutputDebugString(buffer);
		OutputDebugString(strMsg);

		if (hr)
		{
			sprintf(buffer, "(hr=%08lx)\n", hr);
			OutputDebugString(buffer);
		}

		OutputDebugString("\n");

		return hr;
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
			case DD_OK:
				return "No error.";
			case DDERR_NOMIPMAPHW:
				return "The operation cannot be carried out because no mipmap texture mapping hardware is present or available.";
			case DDERR_UNSUPPORTEDMODE:
				return "The FourCC format requested is not supported by DirectDraw. ";
			case DDERR_ALREADYINITIALIZED:
				return "This object is already initialized.";
			case DDERR_BLTFASTCANTCLIP:
				return "Return if a clipper object is attached to the source surface passed into a BltFast call.";
			case DDERR_CANNOTATTACHSURFACE:
				return "This surface can not be attached to the requested surface.";
			case DDERR_CANNOTDETACHSURFACE:
				return "This surface can not be detached from the requested surface.";
			case DDERR_CANTCREATEDC:
				return "Windows can not create any more DCs.";
			case DDERR_CANTDUPLICATE:
				return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.";
			case DDERR_CLIPPERISUSINGHWND:
				return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.";
			case DDERR_COLORKEYNOTSET:
				return "No src color key specified for this operation.";
			case DDERR_CURRENTLYNOTAVAIL:
				return "Support is currently not available.";
			case DDERR_DIRECTDRAWALREADYCREATED:
				return "A DirectDraw object representing this driver has already been created for this process.";
			case DDERR_EXCEPTION:
				return "An exception was encountered while performing the requested operation.";
			case DDERR_EXCLUSIVEMODEALREADYSET:
				return "An attempt was made to set the cooperative level when it was already set to exclusive.";
			case DDERR_GENERIC:
				return "Generic failure.";
			case DDERR_HEIGHTALIGN:
				return "Height of rectangle provided is not a multiple of reqd alignment.";
			case DDERR_HWNDALREADYSET:
				return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.";
			case DDERR_HWNDSUBCLASSED:
				return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.";
			case DDERR_IMPLICITLYCREATED:
				return "This surface can not be restored because it is an implicitly created surface.";
			case DDERR_INCOMPATIBLEPRIMARY:
				return "Unable to match primary surface creation request with existing primary surface.";
			case DDERR_INVALIDCAPS:
				return "One or more of the caps bits passed to the callback are incorrect.";
			case DDERR_INVALIDCLIPLIST:
				return "DirectDraw does not support the provided cliplist.";
			case DDERR_INVALIDDIRECTDRAWGUID:
				return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.";
			case DDERR_INVALIDMODE:
				return "DirectDraw does not support the requested mode.";
			case DDERR_INVALIDOBJECT:
				return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.";
			case DDERR_INVALIDPARAMS:
				return "One or more of the parameters passed to the function are incorrect.";
			case DDERR_INVALIDPIXELFORMAT:
				return "The pixel format was invalid as specified.";
			case DDERR_INVALIDPOSITION:
				return "Returned when the position of the overlay on the destination is no longer legal for that destination.";
			case DDERR_INVALIDRECT:
				return "Rectangle provided was invalid.";
			case DDERR_LOCKEDSURFACES:
				return "Operation could not be carried out because one or more surfaces are locked.";
			case DDERR_NO3D:
				return "There is no 3D present.";
			case DDERR_NOALPHAHW:
				return "Operation could not be carried out because there is no alpha accleration hardware present or available.";
			case DDERR_NOBLTHW:
				return "No blitter hardware present.";
			case DDERR_NOCLIPLIST:
				return "No cliplist available.";
			case DDERR_NOCLIPPERATTACHED:
				return "No clipper object attached to surface object.";
			case DDERR_NOCOLORCONVHW:
				return "Operation could not be carried out because there is no color conversion hardware present or available.";
			case DDERR_NOCOLORKEY:
				return "Surface doesn't currently have a color key";
			case DDERR_NOCOLORKEYHW:
				return "Operation could not be carried out because there is no hardware support of the destination color key.";
			case DDERR_NOCOOPERATIVELEVELSET:
				return "Create function called without DirectDraw object method SetCooperativeLevel being called.";
			case DDERR_NODC:
				return "No DC was ever created for this surface.";
			case DDERR_NODDROPSHW:
				return "No DirectDraw ROP hardware.";
			case DDERR_NODIRECTDRAWHW:
				return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.";
			case DDERR_NOEMULATION:
				return "Software emulation not available.";
			case DDERR_NOEXCLUSIVEMODE:
				return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.";
			case DDERR_NOFLIPHW:
				return "Flipping visible surfaces is not supported.";
			case DDERR_NOGDI:
				return "There is no GDI present.";
			case DDERR_NOHWND:
				return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.";
			case DDERR_NOMIRRORHW:
				return "Operation could not be carried out because there is no hardware present or available.";
			case DDERR_NOOVERLAYDEST:
				return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.";
			case DDERR_NOOVERLAYHW:
				return "Operation could not be carried out because there is no overlay hardware present or available.";
			case DDERR_NOPALETTEATTACHED:
				return "No palette object attached to this surface.";
			case DDERR_NOPALETTEHW:
				return "No hardware support for 16 or 256 color palettes.";
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
			case DDERR_NOTAOVERLAYSURFACE:
				return "Returned when an overlay member is called for a non-overlay surface.";
			case DDERR_NOTEXTUREHW:
				return "Operation could not be carried out because there is no texture mapping hardware present or available.";
			case DDERR_NOTFLIPPABLE:
				return "An attempt has been made to flip a surface that is not flippable.";
			case DDERR_NOTFOUND:
				return "Requested item was not found.";
			case DDERR_NOTLOCKED:
				return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.";
			case DDERR_NOTPALETTIZED:
				return "The surface being used is not a palette-based surface.";
			case DDERR_NOVSYNCHW:
				return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.";
			case DDERR_NOZBUFFERHW:
				return "Operation could not be carried out because there is no hardware support for zbuffer blitting.";
			case DDERR_NOZOVERLAYHW:
				return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.";
			case DDERR_OUTOFCAPS:
				return "The hardware needed for the requested operation has already been allocated.";
			case DDERR_OUTOFMEMORY:
				return "DirectDraw does not have enough memory to perform the operation.";
			case DDERR_OUTOFVIDEOMEMORY:
				return "DirectDraw does not have enough memory to perform the operation.";
			case DDERR_OVERLAYCANTCLIP:
				return "The hardware does not support clipped overlays.";
			case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
				return "Can only have ony color key active at one time for overlays.";
			case DDERR_OVERLAYNOTVISIBLE:
				return "Returned when GetOverlayPosition is called on a hidden overlay.";
			case DDERR_PALETTEBUSY:
				return "Access to this palette is being refused because the palette is already locked by another thread.";
			case DDERR_PRIMARYSURFACEALREADYEXISTS:
				return "This process already has created a primary surface.";
			case DDERR_REGIONTOOSMALL:
				return "Region passed to Clipper::GetClipList is too small.";
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
			case DDERR_UNSUPPORTED:
				return "Action not supported.";
			case DDERR_UNSUPPORTEDFORMAT:
				return "FOURCC format requested is unsupported by DirectDraw.";
			case DDERR_UNSUPPORTEDMASK:
				return "Bitmask in the pixel format requested is unsupported by DirectDraw.";
			case DDERR_VERTICALBLANKINPROGRESS:
				return "Vertical blank is in progress.";
			case DDERR_WASSTILLDRAWING:
				return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.";
			case DDERR_WRONGMODE:
				return "This surface can not be restored because it was created in a different mode.";
			case DDERR_XALIGN:
				return "Rectangle provided was not horizontally aligned on required boundary.";
			case E_NOINTERFACE:
				return "No such interface supported";
			case E_POINTER:
				return "Invalid pointer";
			case E_HANDLE:
				return "Invalid handle";
			case E_ABORT:
				return "Operation aborted";
			case E_ACCESSDENIED:
				return "General access denied error";
			case D3DERR_BADMINORVERSION:
				return "D3DERR_BADMINORVERSION";
			case D3DERR_DEVICEAGGREGATED:
				return "D3DERR_DEVICEAGGREGATED";
			case D3DERR_EXECUTE_CLIPPED_FAILED:
				return "D3DERR_EXECUTE_CLIPPED_FAILED";
			case D3DERR_EXECUTE_CREATE_FAILED:
				return "D3DERR_EXECUTE_CREATE_FAILED";
			case D3DERR_EXECUTE_DESTROY_FAILED:
				return "D3DERR_EXECUTE_DESTROY_FAILED";
			case D3DERR_EXECUTE_FAILED:
				return "D3DERR_EXECUTE_FAILED";
			case D3DERR_EXECUTE_LOCK_FAILED:
				return "D3DERR_EXECUTE_LOCK_FAILED";
			case D3DERR_EXECUTE_LOCKED:
				return "D3DERR_EXECUTE_LOCKED";
			case D3DERR_EXECUTE_NOT_LOCKED:
				return "D3DERR_EXECUTE_NOT_LOCKED";
			case D3DERR_EXECUTE_UNLOCK_FAILED:
				return "D3DERR_EXECUTE_UNLOCK_FAILED";
			case D3DERR_INITFAILED:
				return "D3DERR_INITFAILED";
			case D3DERR_INBEGIN:
				return "D3DERR_INBEGIN";
			case D3DERR_INVALID_DEVICE:
				return "D3DERR_INVALID_DEVICE";
			case D3DERR_INVALIDCURRENTVIEWPORT:
				return "D3DERR_INVALIDCURRENTVIEWPORT";
			case D3DERR_INVALIDPALETTE:
				return "D3DERR_INVALIDPALETTE";
			case D3DERR_INVALIDPRIMITIVETYPE:
				return "D3DERR_INVALIDPRIMITIVETYPE";
			case D3DERR_INVALIDRAMPTEXTURE:
				return "D3DERR_INVALIDRAMPTEXTURE";
			case D3DERR_INVALIDVERTEXTYPE:
				return "D3DERR_INVALIDVERTEXTYPE";
			case D3DERR_LIGHT_SET_FAILED:
				return "D3DERR_LIGHT_SET_FAILED";
			case D3DERR_LIGHTHASVIEWPORT:
				return "D3DERR_LIGHTHASVIEWPORT";
			case D3DERR_LIGHTNOTINTHISVIEWPORT:
				return "D3DERR_LIGHTNOTINTHISVIEWPORT";
			case D3DERR_MATERIAL_CREATE_FAILED:
				return "D3DERR_MATERIAL_CREATE_FAILED";
			case D3DERR_MATERIAL_DESTROY_FAILED:
				return "D3DERR_MATERIAL_DESTROY_FAILED";
			case D3DERR_MATERIAL_GETDATA_FAILED:
				return "D3DERR_MATERIAL_GETDATA_FAILED";
			case D3DERR_MATERIAL_SETDATA_FAILED:
				return "D3DERR_MATERIAL_SETDATA_FAILED";
			case D3DERR_MATRIX_CREATE_FAILED:
				return "D3DERR_MATRIX_CREATE_FAILED";
			case D3DERR_MATRIX_DESTROY_FAILED:
				return "D3DERR_MATRIX_DESTROY_FAILED";
			case D3DERR_MATRIX_GETDATA_FAILED:
				return "D3DERR_MATRIX_GETDATA_FAILED";
			case D3DERR_MATRIX_SETDATA_FAILED:
				return "D3DERR_MATRIX_SETDATA_FAILED";
			case D3DERR_NOCURRENTVIEWPORT:
				return "D3DERR_NOCURRENTVIEWPORT";
			case D3DERR_NOTINBEGIN:
				return "D3DERR_NOTINBEGIN";
			case D3DERR_NOVIEWPORTS:
				return "D3DERR_NOVIEWPORTS";
			case D3DERR_SCENE_BEGIN_FAILED:
				return "D3DERR_SCENE_BEGIN_FAILED";
			case D3DERR_SCENE_END_FAILED:
				return "D3DERR_SCENE_END_FAILED";
			case D3DERR_SCENE_IN_SCENE:
				return "D3DERR_SCENE_IN_SCENE";
			case D3DERR_SCENE_NOT_IN_SCENE:
				return "D3DERR_SCENE_NOT_IN_SCENE";
			case D3DERR_SETVIEWPORTDATA_FAILED:
				return "D3DERR_SETVIEWPORTDATA_FAILED";
			case D3DERR_SURFACENOTINVIDMEM:
				return "D3DERR_SURFACENOTINVIDMEM";
			case D3DERR_TEXTURE_BADSIZE:
				return "D3DERR_TEXTURE_BADSIZE";
			case D3DERR_TEXTURE_CREATE_FAILED:
				return "D3DERR_TEXTURE_CREATE_FAILED";
			case D3DERR_TEXTURE_DESTROY_FAILED:
				return "D3DERR_TEXTURE_DESTROY_FAILED";
			case D3DERR_TEXTURE_GETSURF_FAILED:
				return "D3DERR_TEXTURE_GETSURF_FAILED";
			case D3DERR_TEXTURE_LOAD_FAILED:
				return "D3DERR_TEXTURE_LOAD_FAILED";
			case D3DERR_TEXTURE_LOCK_FAILED:
				return "D3DERR_TEXTURE_LOCK_FAILED";
			case D3DERR_TEXTURE_LOCKED:
				return "D3DERR_TEXTURE_LOCKED";
			case D3DERR_TEXTURE_NO_SUPPORT:
				return "D3DERR_TEXTURE_NO_SUPPORT";
			case D3DERR_TEXTURE_NOT_LOCKED:
				return "D3DERR_TEXTURE_NOT_LOCKED";
			case D3DERR_TEXTURE_SWAP_FAILED:
				return "D3DERR_TEXTURE_SWAP_FAILED";
			case D3DERR_TEXTURE_UNLOCK_FAILED:
				return "D3DERR_TEXTURE_UNLOCK_FAILED";
			case D3DERR_VIEWPORTDATANOTSET:
				return "D3DERR_VIEWPORTDATANOTSET";
			case D3DERR_VIEWPORTHASNODEVICE:
				return "D3DERR_VIEWPORTHASNODEVICE";
			case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY:
				return "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY";
			case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY:
				return "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY";
			case EPT_S_CANT_CREATE:
				return "The endpoint mapper database could not be created.";
			case EPT_S_CANT_PERFORM_OP:
				return "The operation cannot be performed.";
			case EPT_S_INVALID_ENTRY:
				return "The entry is invalid.";
			case EPT_S_NOT_REGISTERED:
				return "There are no more endpoints available from the endpoint mapper.";
			case ERROR_ACCESS_DENIED:
				return "Access is denied.";
			case ERROR_ACCOUNT_DISABLED:
				return "The referenced account is currently disabled and cannot be logged on to.";
			case ERROR_ACCOUNT_EXPIRED:
				return "The user's account has expired.";
			case ERROR_ACCOUNT_RESTRICTION:
				return "Indicates a referenced user name and authentication information are valid, but some user account restriction has prevented successful "
					   "authentication (such as time-of-day restrictions.";
			case ERROR_ADAP_HDW_ERR:
				return "A network adapter hardware ERROR occurred.";
			case ERROR_ALIAS_EXISTS:
				return "The specified alias already exists.";
			case ERROR_ALLOTTED_SPACE_EXCEEDED:
				return "When a block of memory is allotted for future updates, such as the memory allocated to hold discretionary access control and primary "
					   "group information, successive updates may exceed the amount of memory originally allotted. Since quota may already have been charged "
					   "to several processes that have handles of the object, it is not reasonable to alter the size of the allocated memory. Instead, a "
					   "request that requires more memory than has been allotted must fail and the ERROR_ALLOTTED_SPACE_EXCEEDED error returned.";
			case ERROR_ALREADY_ASSIGNED:
				return "The local device name is already in use.";
			case ERROR_ALREADY_EXISTS:
				return "Attempt to create file that already exists.";
			case ERROR_ALREADY_RUNNING_LKG:
				return "The system is currently running with the last-known-good configuration.";
			case ERROR_ARENA_TRASHED:
				return "The storage control blocks were destroyed.";
			case ERROR_ARITHMETIC_OVERFLOW:
				return "Arithmetic result exceeded 32-bits.";
			case ERROR_ATOMIC_LOCKS_NOT_SUPPORTED:
				return "The file system does not support atomic changing of the lock type.";
			case ERROR_AUTODATASEG_EXCEEDS_64k:
				return "The operating system cannot run this application program.";
			case ERROR_BAD_ARGUMENTS:
				return "The argument string passed to DosExecPgm is incorrect.";
			case ERROR_BAD_COMMAND:
				return "The device does not recognize the command.";
			case ERROR_BAD_DESCRIPTOR_FORMAT:
				return "Indicates a security descriptor is not in the required format (absolute or self-relative";
			case ERROR_BAD_DEV_TYPE:
				return "The network resource type is incorrect.";
			case ERROR_BAD_DEVICE:
				return "The specified device name is invalid.";
			case ERROR_BAD_DRIVER_LEVEL:
				return "The system does not support the requested command.";
			case ERROR_BAD_ENVIRONMENT:
				return "The environment is incorrect.";
			case ERROR_BAD_EXE_FORMAT:
				return "%1 is not a valid Windows-based application.";
			case ERROR_BAD_FORMAT:
				return "An attempt was made to load a program with an incorrect format.";
			case ERROR_BAD_IMPERSONATION_LEVEL:
				return "A specified impersonation level is invalid. Also used to indicate a required impersonation level was not provided.";
			case ERROR_BAD_INHERITANCE_ACL:
				return "Indicates that an attempt to build either an inherited ACL or ACE did not succeed. One of the more probable causes is the replacement "
					   "of a CreatorId with an SID that didn't fit into the ACE or ACL.";
			case ERROR_BAD_LENGTH:
				return "The program issued a command but the command length is incorrect.";
			case ERROR_BAD_LOGON_SESSION_STATE:
				return "The logon session is not in a state consistent with the requested operation.";
			case ERROR_BAD_NET_NAME:
				return "The network name cannot be found.";
			case ERROR_BAD_NET_RESP:
				return "The specified server cannot perform the requested operation.";
			case ERROR_BAD_NETPATH:
				return "The network path was not found.";
			case ERROR_BAD_PATHNAME:
				return "The specified path name is invalid.";
			case ERROR_BAD_PIPE:
				return "The pipe state is invalid.";
			case ERROR_BAD_PROFILE:
				return "The network connection profile is damaged.";
			case ERROR_BAD_PROVIDER:
				return "The specified network provider name is invalid.";
			case ERROR_BAD_REM_ADAP:
				return "The remote adapter is not compatible.";
			case ERROR_BAD_THREADID_ADDR:
				return "The address for the thread ID is incorrect.";
			case ERROR_BAD_TOKEN_TYPE:
				return "The type of token object is inappropriate for its attempted use.";
			case ERROR_BAD_UNIT:
				return "The system cannot find the specified device.";
			case ERROR_BAD_USERNAME:
				return "The specified user name is invalid.";
			case ERROR_BAD_VALIDATION_CLASS:
				return "The requested validation information class is invalid.";
			case ERROR_BADDB:
				return "The configuration registry database is damaged.";
			case ERROR_BADKEY:
				return "The configuration registry key is invalid.";
			case ERROR_BEGINNING_OF_MEDIA:
				return "The beginning of the tape or partition was encountered.";
			case ERROR_BOOT_ALREADY_ACCEPTED:
				return "The current boot has already been accepted for use as the last-known-good control set.";
			case ERROR_BROKEN_PIPE:
				return "The pipe was ended.";
			case ERROR_BUFFER_OVERFLOW:
				return "The file name is too long.";
			case ERROR_BUS_RESET:
				return "The I/O bus was reset.";
			case ERROR_BUSY:
				return "The requested resource is in use.";
			case ERROR_BUSY_DRIVE:
				return "The system cannot perform a JOIN or SUBST at this time.";
			case ERROR_CALL_NOT_IMPLEMENTED:
				return "The Application Program Interface (API) entered will only work in Windows/NT mode.";
			case ERROR_CAN_NOT_COMPLETE:
				return "Cannot complete function for some reason.";
			case ERROR_CANCEL_VIOLATION:
				return "A lock request was not outstanding for the supplied cancel region.";
			case ERROR_CANNOT_COPY:
				return "The Copy API cannot be used.";
			case ERROR_CANNOT_FIND_WND_CLASS:
				return "Cannot find window class.";
			case ERROR_CANNOT_IMPERSONATE:
				return "Indicates that an attempt was made to impersonate via a named pipe was not yet read from.";
			case ERROR_CANNOT_MAKE:
				return "The directory or file cannot be created.";
			case ERROR_CANNOT_OPEN_PROFILE:
				return "Unable to open the network connection profile.";
			case ERROR_CANT_ACCESS_DOMAIN_INFO:
				return "Indicates a domain controller could not be contacted or that objects within the domain are protected and necessary information could "
					   "not be retrieved.";
			case ERROR_CANT_DISABLE_MANDATORY:
				return "A mandatory group cannot be disabled.";
			case ERROR_CANT_OPEN_ANONYMOUS:
				return "An attempt was made to open an anonymous level token. Anonymous tokens cannot be opened.";
			case ERROR_CANTOPEN:
				return "The configuration registry key cannot be opened.";
			case ERROR_CANTREAD:
				return "The configuration registry key cannot be read.";
			case ERROR_CANTWRITE:
				return "The configuration registry key cannot be written.";
			case ERROR_CHILD_MUST_BE_VOLATILE:
				return "An attempt was made to create a stable subkey under a volatile parent key.";
			case ERROR_CHILD_NOT_COMPLETE:
				return "The %1 application cannot be run in Windows mode.";
			case ERROR_CHILD_WINDOW_MENU:
				return "Child windows can't have menus.";
			case ERROR_CIRCULAR_DEPENDENCY:
				return "Circular service dependency was specified.";
			case ERROR_CLASS_ALREADY_EXISTS:
				return "Class already exists.";
			case ERROR_CLASS_DOES_NOT_EXIST:
				return "Class does not exist.";
			case ERROR_CLASS_HAS_WINDOWS:
				return "Class still has open windows.";
			case ERROR_CLIPBOARD_NOT_OPEN:
				return "Thread doesn't have clipboard open.";
			case ERROR_CONNECTION_UNAVAIL:
				return "The device is not currently connected but is a remembered connection.";
			case ERROR_CONTROL_ID_NOT_FOUND:
				return "Control ID not found.";
			case ERROR_COUNTER_TIMEOUT:
				return "A serial I/O operation completed because the time-out period expired. (The IOCTL_SERIAL_XOFF_COUNTER did not reach zero.)";
			case ERROR_CRC:
				return "Data ERROR (cyclic redundancy check)";
			case ERROR_CURRENT_DIRECTORY:
				return "The directory cannot be removed.";
			case ERROR_DATABASE_DOES_NOT_EXIST:
				return "The database specified does not exist.";
			case ERROR_DC_NOT_FOUND:
				return "Invalid HDC passed to ReleaseDC.";
			case ERROR_DEPENDENT_SERVICES_RUNNING:
				return "A stop control has been sent to a service which other running services are dependent on.";
			case ERROR_DESTROY_OBJECT_OF_OTHER_THREAD:
				return "Cannot destroy object created by another thread.";
			case ERROR_DEV_NOT_EXIST:
				return "The specified network resource is no longer available.";
			case ERROR_DEVICE_ALREADY_REMEMBERED:
				return "An attempt was made to remember a device that was previously remembered.";
			case ERROR_DEVICE_IN_USE:
				return "The device is in use by an active process and cannot be disconnected.";
			case ERROR_DEVICE_NOT_PARTITIONED:
				return "Tape partition information could not be found when loading a tape.";
			case ERROR_DIR_NOT_EMPTY:
				return "The directory is not empty.";
			case ERROR_DIR_NOT_ROOT:
				return "The directory is not a subdirectory of the root directory.";
			case ERROR_DIRECT_ACCESS_HANDLE:
				return "Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.";
			case ERROR_DIRECTORY:
				return "The directory name is invalid.";
			case ERROR_DISCARDED:
				return "The segment is already discarded and cannot be locked.";
			case ERROR_DISK_CHANGE:
				return "Program stopped because alternate disk was not inserted.";
			case ERROR_DISK_CORRUPT:
				return "The disk structure is damaged and nonreadable.";
			case ERROR_DISK_FULL:
				return "There is not enough space on the disk.";
			case ERROR_DISK_OPERATION_FAILED:
				return "While accessing the hard disk, a disk operation failed even after retries.";
			case ERROR_DISK_RECALIBRATE_FAILED:
				return "While accessing the hard disk, a recalibrate operation failed, even after retries.";
			case ERROR_DISK_RESET_FAILED:
				return "While accessing the hard disk, a disk controller reset was needed, but even that failed.";
			case ERROR_DLL_INIT_FAILED:
				return "A DLL initialization routine failed.";
			case ERROR_DOMAIN_EXISTS:
				return "The specified domain already exists.";
			case ERROR_DOMAIN_LIMIT_EXCEEDED:
				return "An attempt to exceed the limit on the number of domains per server for this release.";
			case ERROR_DOMAIN_TRUST_INCONSISTENT:
				return "The name or security ID (SID) of the domain specified is inconsistent with the trust information for that domain.";
			case ERROR_DRIVE_LOCKED:
				return "The disk is in use or locked by another process.";
			case ERROR_DUP_DOMAINNAME:
				return "The workgroup or domain name is already in use by another computer on the network.";
			case ERROR_DUP_NAME:
				return "A duplicate name exists on the network.";
			case ERROR_DUPLICATE_SERVICE_NAME:
				return "The name is already in use as either a service name or a service display name.";
			case ERROR_DYNLINK_FROM_INVALID_RING:
				return "The operating system cannot run this application program.";
			case ERROR_EA_ACCESS_DENIED:
				return "Access to the EA is denied.";
			case ERROR_EA_FILE_CORRUPT:
				return "The EA file on the mounted file system is damaged.";
			case ERROR_EA_LIST_INCONSISTENT:
				return "The EAs are inconsistent.";
			case ERROR_EA_TABLE_FULL:
				return "The EA table in the EA file on the mounted file system is full.";
			case ERROR_EAS_DIDNT_FIT:
				return "The EAs did not fit in the buffer.";
			case ERROR_EAS_NOT_SUPPORTED:
				return "The mounted file system does not support extended attributes.";
			case ERROR_END_OF_MEDIA:
				return "End of tape mark was reached during an operation.";
			case ERROR_ENVVAR_NOT_FOUND:
				return "The system could not find the environment option entered.";
			case ERROR_EOM_OVERFLOW:
				return "Physical end of tape encountered.";
			case ERROR_EVENTLOG_CANT_START:
				return "No event log file could be opened, so the event logging service did not start.";
			case ERROR_EVENTLOG_FILE_CHANGED:
				return "The event log file has changed between reads.";
			case ERROR_EVENTLOG_FILE_CORRUPT:
				return "One of the Eventlog logfiles is damaged.";
			case ERROR_EXCEPTION_IN_SERVICE:
				return "An exception occurred in the service when handling the control request.";
			case ERROR_EXCL_SEM_ALREADY_OWNED:
				return "The exclusive semaphore is owned by another process.";
			case ERROR_EXE_MARKED_INVALID:
				return "The operating system cannot run %1.";
			case ERROR_EXTENDED_ERROR:
				return "An extended ERROR has occurred.";
			case ERROR_FAIL_I24:
				return "Fail on INT 24.";
			case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
				return "The service process could not connect to the service controller.";
			case ERROR_FILE_CORRUPT:
				return "The file or directory is damaged and nonreadable.";
			case ERROR_FILE_EXISTS:
				return "The file exists.";
			case ERROR_FILE_INVALID:
				return "The volume for a file was externally altered and the opened file is no longer valid.";
			case ERROR_FILE_NOT_FOUND:
				return "The system cannot find the file specified.";
			case ERROR_FILEMARK_DETECTED:
				return "A tape access reached a filemark.";
			case ERROR_FILENAME_EXCED_RANGE:
				return "The file name or extension is too long.";
			case ERROR_FLOPPY_BAD_REGISTERS:
				return "The floppy disk controller returned inconsistent results in its registers.";
			case ERROR_FLOPPY_ID_MARK_NOT_FOUND:
				return "No ID address mark was found on the floppy disk.";
			case ERROR_FLOPPY_WRONG_CYLINDER:
				return "Mismatch between the floppy disk sector ID field and the floppy disk controller track address.";
			case ERROR_FLOPPY_UNKNOWN_ERROR:
				return "The floppy disk controller reported an ERROR that is not recognized by the floppy disk driver.";
			case ERROR_FULLSCREEN_MODE:
				return "The requested operation cannot be performed in full-screen mode.";
			case ERROR_GEN_FAILURE:
				return "A device attached to the system is not functioning.";
			case ERROR_GENERIC_NOT_MAPPED:
				return "Indicates generic access types were contained in an access mask that should already be mapped to non-generic access types.";
			case ERROR_GLOBAL_ONLY_HOOK:
				return "This hook can only be set globally.";
			case ERROR_GROUP_EXISTS:
				return "The specified group already exists.";
			case ERROR_HANDLE_DISK_FULL:
				return "The disk is full.";
			case ERROR_HANDLE_EOF:
				return "Reached End Of File.";
			case ERROR_HOOK_NEEDS_HMOD:
				return "Cannot set non-local hook without an module handle.";
			case ERROR_HOOK_NOT_INSTALLED:
				return "Hook is not installed.";
			case ERROR_HOTKEY_ALREADY_REGISTERED:
				return "Hotkey is already registered.";
			case ERROR_HOTKEY_NOT_REGISTERED:
				return "Hotkey is not registered.";
			case ERROR_ILL_FORMED_PASSWORD:
				return "When trying to update a password, this return status indicates the value provided for the new password contains values not allowed in "
					   "passwords.";
			case ERROR_INFLOOP_IN_RELOC_CHAIN:
				return "The operating system cannot run %1.";
			case ERROR_INSUFFICIENT_BUFFER:
				return "The data area passed to a system call is too small.";
			case ERROR_INTERNAL_DB_CORRUPTION:
				return "This ERROR indicates the requested operation cannot be completed due to a catastrophic media failure or on-disk data structure "
					   "corruption.";
			case ERROR_INTERNAL_DB_ERROR:
				return "The Local Security Authority (LSA) database contains in internal inconsistency.";
			case ERROR_INTERNAL_ERROR:
				return "This ERROR indicates the SAM server has encounterred an internal consistency error in its database. This catastrophic failure prevents "
					   "further operation of SAM.";
			case ERROR_INVALID_ACCEL_HANDLE:
				return "Invalid accelerator-table handle.";
			case ERROR_INVALID_ACCESS:
				return "The access code is invalid.";
			case ERROR_INVALID_ACCOUNT_NAME:
				return "The name provided is not a properly formed account name.";
			case ERROR_INVALID_ACL:
				return "Indicates the ACL structure is not valid.";
			case ERROR_INVALID_ADDRESS:
				return "Attempt to access invalid address.";
			case ERROR_INVALID_AT_INTERRUPT_TIME:
				return "Cannot request exclusive semaphores at interrupt time.";
			case ERROR_INVALID_BLOCK:
				return "The storage control block address is invalid.";
			case ERROR_INVALID_BLOCK_LENGTH:
				return "When accessing a new tape of a multivolume partition, the current block size is incorrect.";
			case ERROR_INVALID_CATEGORY:
				return "The IOCTL call made by the application program is incorrect.";
			case ERROR_INVALID_COMBOBOX_MESSAGE:
				return "Invalid Message, combo box doesn't have an edit control.";
			case ERROR_INVALID_COMPUTERNAME:
				return "The format of the specified computer name is invalid.";
			case ERROR_INVALID_CURSOR_HANDLE:
				return "The cursor handle is invalid.";
			case ERROR_INVALID_DATA:
				return "The data is invalid.";
			case ERROR_INVALID_DATATYPE:
				return "The specified datatype is invalid.";
			case ERROR_INVALID_DOMAIN_ROLE:
				return "Indicates the requested operation cannot be completed with the domain in its present role.";
			case ERROR_INVALID_DOMAIN_STATE:
				return "Indicates the domain is in the wrong state to perform the desired operation.";
			case ERROR_INVALID_DOMAINNAME:
				return "The format of the specified domain name is invalid.";
			case ERROR_INVALID_DRIVE:
				return "The system cannot find the specified drive.";
			case ERROR_INVALID_DWP_HANDLE:
				return "The DeferWindowPos handle is invalid.";
			case ERROR_INVALID_EA_HANDLE:
				return "The specified EA handle is invalid.";
			case ERROR_INVALID_EA_NAME:
				return "The specified EA name is invalid.";
			case ERROR_INVALID_EDIT_HEIGHT:
				return "Height must be less than 256.";
			case ERROR_INVALID_ENVIRONMENT:
				return "The Environment specified is invalid.";
			case ERROR_INVALID_EVENT_COUNT:
				return "The number of specified semaphore events is incorrect.";
			case ERROR_INVALID_EVENTNAME:
				return "The format of the specified event name is invalid.";
			case ERROR_INVALID_EXE_SIGNATURE:
				return "%1 cannot be run in Windows/NT mode.";
			case ERROR_INVALID_FILTER_PROC:
				return "The filter proc is invalid.";
			case ERROR_INVALID_FLAG_NUMBER:
				return "The flag passed is incorrect.";
			case ERROR_INVALID_FLAGS:
				return "The flags are invalid.";
			case ERROR_INVALID_FUNCTION:
				return "The function is incorrect.";
			case ERROR_INVALID_GROUP_ATTRIBUTES:
				return "The specified attributes are invalid, or incompatible with the attributes for the group as a whole.";
			case ERROR_INVALID_GROUPNAME:
				return "The format of the specified group name is invalid.";
			case ERROR_INVALID_GW_COMMAND:
				return "The GW_* command is invalid.";
			case ERROR_INVALID_HANDLE:
				return "The internal file identifier is incorrect.";
			case ERROR_INVALID_HOOK_FILTER:
				return "The hook filter type is invalid.";
			case ERROR_INVALID_HOOK_HANDLE:
				return "The hook handle is invalid.";
			case ERROR_INVALID_ICON_HANDLE:
				return "The icon handle is invalid.";
			case ERROR_INVALID_ID_AUTHORITY:
				return "The value provided is an invalid value for an identifier authority.";
			case ERROR_INVALID_INDEX:
				return "The index is invalid.";
			case ERROR_INVALID_LB_MESSAGE:
				return "The message for single-selection list box is invalid.";
			case ERROR_INVALID_LEVEL:
				return "The system call level is incorrect.";
			case ERROR_INVALID_LIST_FORMAT:
				return "The list is not correct.";
			case ERROR_INVALID_LOGON_HOURS:
				return "The user account has time restrictions and cannot be logged onto at this time.";
			case ERROR_INVALID_LOGON_TYPE:
				return "Indicates an invalid value has been provided for LogonType has been requested.";
			case ERROR_INVALID_MEMBER:
				return "A new member could not be added to an alias because the member has the wrong account type.";
			case ERROR_INVALID_MENU_HANDLE:
				return "The menu handle is invalid.";
			case ERROR_INVALID_MESSAGE:
				return "Window can't handle sent message.";
			case ERROR_INVALID_MESSAGEDEST:
				return "The format of the specified message destination is invalid.";
			case ERROR_INVALID_MESSAGENAME:
				return "The format of the specified message name is invalid.";
			case ERROR_INVALID_MINALLOCSIZE:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_MODULETYPE:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_MSGBOX_STYLE:
				return "The message box style is invalid.";
			case ERROR_INVALID_NAME:
				return "The file name, directory name, or volume label is syntactically incorrect.";
			case ERROR_INVALID_NETNAME:
				return "The format of the specified network name is invalid.";
			case ERROR_INVALID_ORDINAL:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_OWNER:
				return "Indicates a particular Security ID cannot be assigned as the owner of an object.";
			case ERROR_INVALID_PARAMETER:
				return "The parameter is incorrect.";
			case ERROR_INVALID_PASSWORD:
				return "The specified network password is incorrect.";
			case ERROR_INVALID_PASSWORDNAME:
				return "The format of the specified password is invalid.";
			case ERROR_INVALID_PRIMARY_GROUP:
				return "Indicates a particular Security ID cannot be assigned as the primary group of an object.";
			case ERROR_INVALID_PRINTER_COMMAND:
				return "The printer command is invalid.";
			case ERROR_INVALID_PRINTER_NAME:
				return "The printer name is invalid.";
			case ERROR_INVALID_PRIORITY:
				return "The specified priority is invalid.";
			case ERROR_INVALID_SCROLLBAR_RANGE:
				return "Scrollbar range greater than 0x7FFF.";
			case ERROR_INVALID_SECURITY_DESCR:
				return "Indicates the SECURITY_DESCRIPTOR structure is invalid.";
			case ERROR_INVALID_SEGDPL:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_SEGMENT_NUMBER:
				return "The system detected a segment number that is incorrect.";
			case ERROR_INVALID_SEPARATOR_FILE:
				return "The specified separator file is invalid.";
			case ERROR_INVALID_SERVER_STATE:
				return "Indicates the Sam Server was in the wrong state to perform the desired operation.";
			case ERROR_INVALID_SERVICE_ACCOUNT:
				return "The account name is invalid or does not exist.";
			case ERROR_INVALID_SERVICE_CONTROL:
				return "The requested control is not valid for this service";
			case ERROR_INVALID_SERVICE_LOCK:
				return "The specified service database lock is invalid.";
			case ERROR_INVALID_SERVICENAME:
				return "The format of the specified service name is invalid.";
			case ERROR_INVALID_SHARENAME:
				return "The format of the specified share name is invalid.";
			case ERROR_INVALID_SHOWWIN_COMMAND:
				return "The ShowWindow command is invalid.";
			case ERROR_INVALID_SID:
				return "Indicates the SID structure is invalid.";
			case ERROR_INVALID_SIGNAL_NUMBER:
				return "The signal being posted is incorrect.";
			case ERROR_INVALID_SPI_VALUE:
				return "The SPI_* parameter is invalid.";
			case ERROR_INVALID_STACKSEG:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_STARTING_CODESEG:
				return "The operating system cannot run %1.";
			case ERROR_INVALID_SUB_AUTHORITY:
				return "Indicates the sub-authority value is invalid for the particular use.";
			case ERROR_INVALID_TARGET_HANDLE:
				return "The target internal file identifier is incorrect.";
			case ERROR_INVALID_THREAD_ID:
				return "The thread ID is invalid.";
			case ERROR_INVALID_USER_BUFFER:
				return "The supplied user buffer is invalid for the requested operation.";
			case ERROR_INVALID_VERIFY_SWITCH:
				return "The verify-on-write switch parameter value is incorrect.";
			case ERROR_INVALID_WINDOW_HANDLE:
				return "The window handle invalid.";
			case ERROR_INVALID_WORKSTATION:
				return "The user account is restricted and cannot be used to log on from the source workstation.";
			case ERROR_IO_DEVICE:
				return "The request could not be performed because of an I/O device ERROR.";
			case ERROR_IO_INCOMPLETE:
				return "Overlapped IO event not in signaled state.";
			case ERROR_IO_PENDING:
				return "Overlapped IO operation in progress.";
			case ERROR_IOPL_NOT_ENABLED:
				return "The operating system is not presently configured to run this application.";
			case ERROR_IRQ_BUSY:
				return "Unable to open a device that was sharing an interrupt request (IRQ) with other devices. At least one other device that uses that IRQ "
					   "was already opened.";
			case ERROR_IS_JOIN_PATH:
				return "Not enough resources are available to process this command.";
			case ERROR_IS_JOIN_TARGET:
				return "A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.";
			case ERROR_IS_JOINED:
				return "An attempt was made to use a JOIN or SUBST command on a drive that is already joined.";
			case ERROR_IS_SUBST_PATH:
				return "The path specified is being used in a substitute.";
			case ERROR_IS_SUBST_TARGET:
				return "An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.";
			case ERROR_IS_SUBSTED:
				return "An attempt was made to use a JOIN or SUBST command on a drive already substituted.";
			case ERROR_ITERATED_DATA_EXCEEDS_64k:
				return "The operating system cannot run %1.";
			case ERROR_JOIN_TO_JOIN:
				return "The system tried to join a drive to a directory on a joined drive.";
			case ERROR_JOIN_TO_SUBST:
				return "The system tried to join a drive to a directory on a substituted drive.";
			case ERROR_JOURNAL_HOOK_SET:
				return "The journal hook is already installed.";
			case ERROR_KEY_DELETED:
				return "Illegal operation attempted on a registry key that has been marked for deletion.";
			case ERROR_KEY_HAS_CHILDREN:
				return "An attempt was made to create a symbolic link in a registry key that already has subkeys or values.";
			case ERROR_LABEL_TOO_LONG:
				return "The volume label entered exceeds the 11 character limit. The first 11 characters were written to disk. Any characters that exceeded "
					   "the 11 character limit were automatically deleted.";
			case ERROR_LAST_ADMIN:
				return "Indicates the requested operation would disable or delete the last remaining administration account. This is not allowed to prevent "
					   "creating a situation where the system will not be administrable.";
			case ERROR_LB_WITHOUT_TABSTOPS:
				return "This list box doesn't support tab stops.";
			case ERROR_LISTBOX_ID_NOT_FOUND:
				return "List box ID not found.";
			case ERROR_LM_CROSS_ENCRYPTION_REQUIRED:
				return "An attempt was made to change a user password in the security account manager without providing the required LM cross-encrypted "
					   "password.";
			case ERROR_LOCAL_USER_SESSION_KEY:
				return "A user session key was requested for a local RPC connection. The session key returned is a constant value and not unique to this "
					   "connection.";
			case ERROR_LOCK_FAILED:
				return "Attempt to lock a region of a file failed.";
			case ERROR_LOCK_VIOLATION:
				return "The process cannot access the file because another process has locked a portion of the file.";
			case ERROR_LOCKED:
				return "The segment is locked and cannot be reallocated.";
			case ERROR_LOG_FILE_FULL:
				return "The event log file is full.";
			case ERROR_LOGON_FAILURE:
				return "The attempted logon is invalid. This is due to either a bad user name or authentication information.";
			case ERROR_LOGON_NOT_GRANTED:
				return "A requested type of logon, such as Interactive, Network, or Service, is not granted by the target system's local security policy. The "
					   "system administrator can grant the required form of logon.";
			case ERROR_LOGON_SESSION_COLLISION:
				return "The logon session ID is already in use.";
			case ERROR_LOGON_SESSION_EXISTS:
				return "An attempt was made to start a new session manager or LSA logon session with an ID already in use.";
			case ERROR_LOGON_TYPE_NOT_GRANTED:
				return "A user has requested a type of logon, such as interactive or network, that was not granted. An administrator has control over who may "
					   "logon interactively and through the network.";
			case ERROR_LUIDS_EXHAUSTED:
				return "Indicates there are no more LUID to allocate.";
			case ERROR_MAX_THRDS_REACHED:
				return "No more threads can be created in the system.";
			case ERROR_MEDIA_CHANGED:
				return "Media in drive may have changed.";
			case ERROR_MEMBER_IN_ALIAS:
				return "The specified account name is not a member of the alias.";
			case ERROR_MEMBER_IN_GROUP:
				return "The specified user account is already in the specified group account. Also used to indicate a group can not be deleted because it "
					   "contains a member.";
			case ERROR_MEMBER_NOT_IN_ALIAS:
				return "The specified account name is not a member of the alias.";
			case ERROR_MEMBER_NOT_IN_GROUP:
				return "The specified user account is not a member of the specified group account.";
			case ERROR_MEMBERS_PRIMARY_GROUP:
				return "Indicates a member cannot be removed from a group because the group is currently the member's primary group.";
			case ERROR_META_EXPANSION_TOO_LONG:
				return "The global filename characters * or ? are entered incorrectly, or too many global filename characters are specified.";
			case ERROR_MOD_NOT_FOUND:
				return "The specified module cannot be found.";
			case ERROR_MORE_DATA:
				return "More data is available.";
			case ERROR_MORE_WRITES:
				return "A serial I/O operation was completed by another write to the serial port. (The IOCTL_SERIAL_XOFF_COUNTER reached zero).";
			case ERROR_MR_MID_NOT_FOUND:
				return "The system cannot find message for message number 0x%1 in message file for %2.";
			case ERROR_NEGATIVE_SEEK:
				return "An attempt was made to move the file pointer before the beginning of the file.";
			case ERROR_NESTING_NOT_ALLOWED:
				return "Can't nest calls to LoadModule.";
			case ERROR_NET_WRITE_FAULT:
				return "A write fault occurred on the network.";
			case ERROR_NETLOGON_NOT_STARTED:
				return "An attempt was made to logon, but the network logon service was not started.";
			case ERROR_NETNAME_DELETED:
				return "The specified network name is no longer available.";
			case ERROR_NETWORK_ACCESS_DENIED:
				return "Network access is denied.";
			case ERROR_NETWORK_BUSY:
				return "The network is busy.";
			case ERROR_NO_BROWSER_SERVERS_FOUND:
				return "The list of servers for this workgroup is not currently available";
			case ERROR_NO_DATA:
				return "Pipe close in progress.";
			case ERROR_NO_DATA_DETECTED:
				return "During a tape access, the end of the data marker was reached.";
			case ERROR_NO_IMPERSONATION_TOKEN:
				return "An attempt was made to operate on an impersonation token by a thread was not currently impersonating a client.";
			case ERROR_NO_INHERITANCE:
				return "Indicates an ACL contains no inheritable components.";
			case ERROR_NO_LOG_SPACE:
				return "System could not allocate required space in a registry log.";
			case ERROR_NO_LOGON_SERVERS:
				return "There are currently no logon servers available to service the logon request.";
			case ERROR_NO_MEDIA_IN_DRIVE:
				return "Tape query failed because of no media in drive.";
			case ERROR_NO_MORE_FILES:
				return "There are no more files.";
			case ERROR_NO_MORE_ITEMS:
				return "No more data is available.";
			case ERROR_NO_MORE_SEARCH_HANDLES:
				return "No more internal file identifiers available.";
			case ERROR_NO_NET_OR_BAD_PATH:
				return "No network provider accepted the given network path.";
			case ERROR_NO_NETWORK:
				return "The network is not present or not started.";
			case ERROR_NO_PROC_SLOTS:
				return "The system cannot start another process at this time.";
			case ERROR_NO_QUOTAS_FOR_ACCOUNT:
				return "No system quota limits are specifically set for this account.";
			case ERROR_NO_SCROLLBARS:
				return "Window does not have scroll bars.";
			case ERROR_NO_SECURITY_ON_OBJECT:
				return "Indicates an attempt was made to operate on the security of an object that does not have security associated with it.";
			case ERROR_NO_SHUTDOWN_IN_PROGRESS:
				return "An attempt to abort the shutdown of the system failed because no shutdown was in progress.";
			case ERROR_NO_SIGNAL_SENT:
				return "No process in the command subtree has a signal handler.";
			case ERROR_NO_SPOOL_SPACE:
				return "Space to store the file waiting to be printed is not available on the server.";
			case ERROR_NO_SUCH_ALIAS:
				return "The specified alias does not exist.";
			case ERROR_NO_SUCH_DOMAIN:
				return "The specified domain does not exist.";
			case ERROR_NO_SUCH_GROUP:
				return "The specified group does not exist.";
			case ERROR_NO_SUCH_LOGON_SESSION:
				return "A specified logon session does not exist. It may already have been terminated.";
			case ERROR_NO_SUCH_MEMBER:
				return "A new member cannot be added to an alias because the member does not exist.";
			case ERROR_NO_SUCH_PACKAGE:
				return "A specified authentication package is unknown.";
			case ERROR_NO_SUCH_PRIVILEGE:
				return "A specified privilege does not exist.";
			case ERROR_NO_SUCH_USER:
				return "The specified user does not exist.";
			case ERROR_NO_SYSTEM_MENU:
				return "Window does not have system menu.";
			case ERROR_NO_TOKEN:
				return "An attempt was made to reference a token that does not exist.";
			case ERROR_NO_TRUST_LSA_SECRET:
				return "The workstation does not have a trust secret.";
			case ERROR_NO_TRUST_SAM_ACCOUNT:
				return "The domain controller does not have an account for this workstation.";
			case ERROR_NO_UNICODE_TRANSLATION:
				return "No mapping for the Unicode character exists in the target multi-byte code page.";
			case ERROR_NO_USER_SESSION_KEY:
				return "There is no user session key for the specified logon session.";
			case ERROR_NO_VOLUME_LABEL:
				return "The disk has no volume label.";
			case ERROR_NO_WILDCARD_CHARACTERS:
				return "No wildcard characters found.";
			case ERROR_NOACCESS:
				return "Invalid access to memory location.";
			case ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT:
				return "The account used is an interdomain trust account. Use your normal user account or remote user account to access this server.";
			case ERROR_NOLOGON_SERVER_TRUST_ACCOUNT:
				return "The account used is an server trust account. Use your normal user account or remote user account to access this server.";
			case ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT:
				return "The account used is a workstation trust account. Use your normal user account or remote user account to access this server.";
			case ERROR_NON_MDICHILD_WINDOW:
				return "DefMDIChildProc called with a non-MDI child window.";
			case ERROR_NONE_MAPPED:
				return "None of the information to be mapped has been translated.";
			case ERROR_NOT_ALL_ASSIGNED:
				return "Indicates not all privileges referenced are assigned to the caller. This allows, for example, all privileges to be disabled without "
					   "having to know exactly which privileges are assigned.";
			case ERROR_NOT_CHILD_WINDOW:
				return "Window is not a child window.";
			case ERROR_NOT_CONNECTED:
				return "This network connection does not exist.";
			case ERROR_NOT_CONTAINER:
				return "Cannot enumerate a non-container.";
			case ERROR_NOT_DOS_DISK:
				return "The specified disk cannot be accessed.";
			case ERROR_NOT_ENOUGH_MEMORY:
				return "Not enough storage is available to process this command.";
			case ERROR_NOT_ENOUGH_QUOTA:
				return "Not enough quota is available to process this command.";
			case ERROR_NOT_ENOUGH_SERVER_MEMORY:
				return "Not enough server storage is available to process this command.";
			case ERROR_NOT_JOINED:
				return "The system attempted to delete the JOIN of a drive not previously joined.";
			case ERROR_NOT_LOCKED:
				return "The segment is already unlocked.";
			case ERROR_NOT_LOGON_PROCESS:
				return "The requested action is restricted for use by logon processes only. The calling process has not registered as a logon process.";
			case ERROR_NOT_OWNER:
				return "Attempt to release mutex not owned by caller.";
			case ERROR_NOT_READY:
				return "The drive is not ready.";
			case ERROR_NOT_REGISTRY_FILE:
				return "The system attempted to load or restore a file into the registry, and the specified file is not in the format of a registry file.";
			case ERROR_NOT_SAME_DEVICE:
				return "The system cannot move the file to a different disk drive.";
			case ERROR_NOT_SUBSTED:
				return "The system attempted to delete the substitution of a drive not previously substituted.";
			case ERROR_NOT_SUPPORTED:
				return "The network request is not supported.";
			case ERROR_NOTIFY_ENUM_DIR:
				return "This indicates that a notify change request is being completed and the information is not being returned in the caller's buffer. The "
					   "caller now needs to enumerate the files to find the changes.";
			case ERROR_NT_CROSS_ENCRYPTION_REQUIRED:
				return "An attempt was made to change a user password in the security account manager without providing the necessary NT cross-encrypted "
					   "password.";
			case ERROR_NULL_LM_PASSWORD:
				return "The Windows NT password is too complex to be converted to a Windows-networking password. The Windows-networking password returned is a "
					   "NULL string.";
			case ERROR_OPEN_FAILED:
				return "The system cannot open the specified device or file.";
			case ERROR_OPEN_FILES:
				return "There are open files or requests pending on this connection.";
			case ERROR_OPERATION_ABORTED:
				return "The I/O operation was aborted due to either thread exit or application request.";
			case ERROR_OUT_OF_PAPER:
				return "The printer is out of paper.";
			case ERROR_OUT_OF_STRUCTURES:
				return "Storage to process this request is not available.";
			case ERROR_OUTOFMEMORY:
				return "Not enough storage is available to complete this operation.";
			case ERROR_PARTITION_FAILURE:
				return "Tape could not be partitioned.";
			case ERROR_PASSWORD_EXPIRED:
				return "The user account's password has expired.";
			case ERROR_PASSWORD_RESTRICTION:
				return "When trying to update a password, this status indicates that some password update rule was violated. For example, the password may not "
					   "meet length criteria.";
			case ERROR_PATH_BUSY:
				return "The specified path cannot be used at this time.";
			case ERROR_PATH_NOT_FOUND:
				return "The system cannot find the specified path.";
			case ERROR_PIPE_BUSY:
				return "All pipe instances busy.";
			case ERROR_PIPE_CONNECTED:
				return "There is a process on other end of the pipe.";
			case ERROR_PIPE_LISTENING:
				return "Waiting for a process to open the other end of the pipe.";
			case ERROR_PIPE_NOT_CONNECTED:
				return "No process on other end of pipe.";
			case ERROR_POPUP_ALREADY_ACTIVE:
				return "Pop-up menu already active.";
			case ERROR_POSSIBLE_DEADLOCK:
				return "A potential deadlock condition has been detected.";
			case ERROR_PRINT_CANCELLED:
				return "File waiting to be printed was deleted.";
			case ERROR_PRINTER_ALREADY_EXISTS:
				return "The printer already exists.";
			case ERROR_PRINTER_DRIVER_ALREADY_INSTALLED:
				return "The specified printer driver is already installed.";
			case ERROR_PRINTQ_FULL:
				return "The printer queue is full.";
			case ERROR_PRIVATE_DIALOG_INDEX:
				return "Using private DIALOG window words.";
			case ERROR_PRIVILEGE_NOT_HELD:
				return "A required privilege is not held by the client.";
			case ERROR_PROC_NOT_FOUND:
				return "The specified procedure could not be found.";
			case ERROR_PROCESS_ABORTED:
				return "The process terminated unexpectedly.";
			case ERROR_READ_FAULT:
				return "The system cannot read from the specified device.";
			case ERROR_REDIR_PAUSED:
				return "The specified printer or disk device has been paused.";
			case ERROR_REDIRECTOR_HAS_OPEN_HANDLES:
				return "The redirector is in use and cannot be unloaded.";
			case ERROR_REGISTRY_CORRUPT:
				return "The registry is damaged. The structure of one of the files that contains registry data is damaged, or the system's in memory image of "
					   "the file is damaged, or the file could not be recovered because its alternate copy or log was absent or damaged.";
			case ERROR_REGISTRY_IO_FAILED:
				return "The registry initiated an I/O operation that had an unrecoverable failure. The registry could not read in, or write out, or flush, one "
					   "of the files that contain the system's image of the registry.";
			case ERROR_REGISTRY_RECOVERED:
				return "One of the files containing the system's registry data had to be recovered by use of a log or alternate copy. The recovery succeeded.";
			case ERROR_RELOC_CHAIN_XEEDS_SEGLIM:
				return "The operating system cannot run %1.";
			case ERROR_REM_NOT_LIST:
				return "The remote computer is not available.";
			case ERROR_REMOTE_SESSION_LIMIT_EXCEEDED:
				return "An attempt was made to establish a session to a LAN Manager server, but there are already too many sessions established to that "
					   "server.";
			case ERROR_REQ_NOT_ACCEP:
				return "The network request was not accepted.";
			case ERROR_RESOURCE_DATA_NOT_FOUND:
				return "The specified image file did not contain a resource section.";
			case ERROR_RESOURCE_LANG_NOT_FOUND:
				return "The specified resource language ID cannot be found in the image file.";
			case ERROR_RESOURCE_NAME_NOT_FOUND:
				return "The specified resource name can not be found in the image file.";
			case ERROR_RESOURCE_TYPE_NOT_FOUND:
				return "The specified resource type can not be found in the image file.";
			case ERROR_REVISION_MISMATCH:
				return "Indicates two revision levels are incompatible.";
			case ERROR_RING2_STACK_IN_USE:
				return "The ring 2 stack is in use.";
			case ERROR_RING2SEG_MUST_BE_MOVABLE:
				return "The code segment cannot be greater than or equal to 64KB.";
			case ERROR_RXACT_COMMIT_FAILURE:
				return "Indicates an ERROR occurred during a registry transaction commit. The database has been left in an unknown state. The state of the "
					   "registry transaction is left as COMMITTING. This status value is returned by the runtime library (RTL) registry transaction package "
					   "(RXact).";
			case ERROR_RXACT_INVALID_STATE:
				return "Indicates that the transaction state of a registry sub-tree is incompatible with the requested operation. For example, a request has "
					   "been made to start a new transaction with one already in progress, or a request to apply a transaction when one is not currently in "
					   "progress. This status value is returned by the runtime library (RTL) registry transaction package (RXact).";
			case ERROR_SAME_DRIVE:
				return "The system cannot join or substitute a drive to or for a directory on the same drive.";
			case ERROR_SCREEN_ALREADY_LOCKED:
				return "Screen already locked.";
			case ERROR_SECRET_TOO_LONG:
				return "The length of a secret exceeds the maximum length allowed. The length and number of secrets is limited to satisfy the United States "
					   "State Department export restrictions.";
			case ERROR_SECTOR_NOT_FOUND:
				return "The drive cannot find the requested sector.";
			case ERROR_SEEK:
				return "The drive cannot locate a specific area or track on the disk.";
			case ERROR_SEEK_ON_DEVICE:
				return "The file pointer cannot be set on the specified device or file.";
			case ERROR_SEM_IS_SET:
				return "The semaphore is set and cannot be closed.";
			case ERROR_SEM_NOT_FOUND:
				return "The specified system semaphore name was not found.";
			case ERROR_SEM_OWNER_DIED:
				return "The previous ownership of this semaphore has ended.";
			case ERROR_SEM_TIMEOUT:
				return "The semaphore timeout period has expired.";
			case ERROR_SEM_USER_LIMIT:
				return "Insert the disk for drive 1.";
			case ERROR_SERIAL_NO_DEVICE:
				return "No serial device was successfully initialized. The serial driver will unload.";
			case ERROR_SERVER_DISABLED:
				return "The GUID allocation server is already disabled at the moment.";
			case ERROR_SERVER_HAS_OPEN_HANDLES:
				return "The server is in use and cannot be unloaded.";
			case ERROR_SERVER_NOT_DISABLED:
				return "The GUID allocation server is already enabled at the moment.";
			case ERROR_SERVICE_ALREADY_RUNNING:
				return "An instance of the service is already running.";
			case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
				return "The service cannot accept control messages at this time.";
			case ERROR_SERVICE_DATABASE_LOCKED:
				return "The service database is locked.";
			case ERROR_SERVICE_DEPENDENCY_DELETED:
				return "The dependency service does not exist or has been marked for deletion.";
			case ERROR_SERVICE_DEPENDENCY_FAIL:
				return "The dependency service or group failed to start.";
			case ERROR_SERVICE_DISABLED:
				return "The specified service is disabled and cannot be started.";
			case ERROR_SERVICE_DOES_NOT_EXIST:
				return "The specified service does not exist as an installed service.";
			case ERROR_SERVICE_EXISTS:
				return "The specified service already exists.";
			case ERROR_SERVICE_LOGON_FAILED:
				return "The service did not start due to a logon failure.";
			case ERROR_SERVICE_MARKED_FOR_DELETE:
				return "The specified service has been marked for deletion.";
			case ERROR_SERVICE_NEVER_STARTED:
				return "No attempts to start the service have been made since the last boot.";
			case ERROR_SERVICE_NO_THREAD:
				return "A thread could not be created for the service.";
			case ERROR_SERVICE_NOT_ACTIVE:
				return "The service has not been started.";
			case ERROR_SERVICE_REQUEST_TIMEOUT:
				return "The service did not respond to the start or control request in a timely fashion.";
			case ERROR_SERVICE_SPECIFIC_ERROR:
				return "The service has returned a service-specific ERROR code.";
			case ERROR_SERVICE_START_HANG:
				return "After starting, the service hung in a start-pending state.";
			case ERROR_SESSION_CREDENTIAL_CONFLICT:
				return "The credentials supplied conflict with an existing set of credentials.";
			case ERROR_SETCOUNT_ON_BAD_LB:
				return "LB_SETCOUNT sent to non-lazy list box.";
			case ERROR_SETMARK_DETECTED:
				return "A tape access reached a setmark.";
			case ERROR_SHARING_BUFFER_EXCEEDED:
				return "Too many files opened for sharing.";
			case ERROR_SHARING_PAUSED:
				return "The remote server is paused or is in the process of being started.";
			case ERROR_SHARING_VIOLATION:
				return "The process cannot access the file because it is being used by another process.";
			case ERROR_SHUTDOWN_IN_PROGRESS:
				return "A system shutdown is in progress.";
			case ERROR_SIGNAL_PENDING:
				return "A signal is already pending.";
			case ERROR_SIGNAL_REFUSED:
				return "The recipient process has refused the signal.";
			case ERROR_SOME_NOT_MAPPED:
				return "Some of the information to be mapped has not been translated.";
			case ERROR_SPECIAL_ACCOUNT:
				return "Indicates an operation was attempted on a built-in (special) SAM account that is incompatible with built-in accounts. For example, "
					   "built-in accounts cannot be renamed or deleted.";
			case ERROR_SPECIAL_GROUP:
				return "The requested operation cannot be performed on the specified group because it is a built-in special group.";
			case ERROR_SPECIAL_USER:
				return "The requested operation cannot be performed on the specified user because it is a built-in special user.";
			case ERROR_STACK_OVERFLOW:
				return "Recursion too deep, stack overflowed.";
			case ERROR_SUBST_TO_JOIN:
				return "The system attempted to SUBST a drive to a directory on a joined drive.";
			case ERROR_SUBST_TO_SUBST:
				return "The system attempted to substitute a drive to a directory on a substituted drive.";
			case ERROR_SWAPERROR:
				return "ERROR accessing paging file.";
			case ERROR_SYSTEM_TRACE:
				return "System trace information not specified in your CONFIG.SYS file, or tracing is not allowed.";
			case ERROR_THREAD_1_INACTIVE:
				return "The signal handler cannot be set.";
			case ERROR_TLW_WITH_WSCHILD:
				return "CreateWindow failed, creating top-level window with WS_CHILD style.";
			case ERROR_TOKEN_ALREADY_IN_USE:
				return "An attempt was made to establish a token for use as a primary token but the token is already in use. A token can only be the primary "
					   "token of one process at a time.";
			case ERROR_TOO_MANY_CMDS:
				return "The network BIOS command limit has been reached.";
			case ERROR_TOO_MANY_CONTEXT_IDS:
				return "During a logon attempt, the user's security context accumulated too many security IDs. Remove the user from some groups or aliases to "
					   "reduce the number of security ids to incorporate into the security context.";
			case ERROR_TOO_MANY_LUIDS_REQUESTED:
				return "The number of LUID requested cannot be allocated with a single allocation.";
			case ERROR_TOO_MANY_MODULES:
				return "Too many dynamic link modules are attached to this program or dynamic link module.";
			case ERROR_TOO_MANY_MUXWAITERS:
				return "Too many semaphores are already set.";
			case ERROR_TOO_MANY_NAMES:
				return "The name limit for the local computer network adapter card exceeded.";
			case ERROR_TOO_MANY_OPEN_FILES:
				return "The system cannot open the file.";
			case ERROR_TOO_MANY_POSTS:
				return "Too many posts made to a semaphore.";
			case ERROR_TOO_MANY_SECRETS:
				return "The maximum number of secrets that can be stored in a single system was exceeded. The length and number of secrets is limited to "
					   "satisfy the United States State Department export restrictions.";
			case ERROR_TOO_MANY_SEM_REQUESTS:
				return "The semaphore cannot be set again.";
			case ERROR_TOO_MANY_SEMAPHORES:
				return "Cannot create another system semaphore.";
			case ERROR_TOO_MANY_SESS:
				return "The network BIOS session limit exceeded.";
			case ERROR_TOO_MANY_SIDS:
				return "Too many SIDs specified.";
			case ERROR_TOO_MANY_TCBS:
				return "Cannot create another thread.";
			case ERROR_TRUST_FAILURE:
				return "The network logon failed.";
			case ERROR_TRUSTED_DOMAIN_FAILURE:
				return "The trust relationship between the primary domain and the trusted domain failed.";
			case ERROR_TRUSTED_RELATIONSHIP_FAILURE:
				return "The trust relationship between this workstation and the primary domain failed.";
			case ERROR_UNABLE_TO_LOCK_MEDIA:
				return "Attempt to lock the eject media mechanism failed.";
			case ERROR_UNABLE_TO_UNLOAD_MEDIA:
				return "Unload media failed.";
			case ERROR_UNEXP_NET_ERR:
				return "An unexpected network ERROR occurred.";
			case ERROR_UNKNOWN_PORT:
				return "The specified port is unknown.";
			case ERROR_UNKNOWN_PRINTPROCESSOR:
				return "The print processor is unknown.";
			case ERROR_UNKNOWN_PRINTER_DRIVER:
				return "The printer driver is unknown.";
			case ERROR_UNKNOWN_REVISION:
				return "Indicates an encountered or specified revision number is not one known by the service. The service may not be aware of a more recent "
					   "revision.";
			case ERROR_UNRECOGNIZED_MEDIA:
				return "The disk media is not recognized. It may not be formatted.";
			case ERROR_UNRECOGNIZED_VOLUME:
				return "The volume does not contain a recognized file system. Make sure that all required file system drivers are loaded and the volume is not "
					   "damaged.";
			case ERROR_USER_EXISTS:
				return "The specified user already exists.";
			case ERROR_VC_DISCONNECTED:
				return "The session was canceled.";
			case ERROR_WAIT_NO_CHILDREN:
				return "There are no child processes to wait for.";
			case ERROR_WINDOW_NOT_COMBOBOX:
				return "The window is not a combo box.";
			case ERROR_WINDOW_NOT_DIALOG:
				return "The window is not a valid dialog window.";
			case ERROR_WINDOW_OF_OTHER_THREAD:
				return "Invalid window, belongs to other thread.";
			case ERROR_WRITE_FAULT:
				return "The system cannot write to the specified device.";
			case ERROR_WRITE_PROTECT:
				return "The media is write protected.";
			case ERROR_WRONG_DISK:
				return "The wrong disk is in the drive. Insert %2 (Volume Serial Number: %3) into drive %1.";
			case ERROR_WRONG_PASSWORD:
				return "When trying to update a password, this return status indicates the value provided as the current password is incorrect.";
			case LZERROR_BADINHANDLE:
				return "Invalid input handle.";
			case LZERROR_BADOUTHANDLE:
				return "Invalid output handle.";
			case LZERROR_BADVALUE:
				return "Input parameter out of acceptable range.";
			case LZERROR_GLOBALLOC:
				return "Insufficient memory for LZFile structure.";
			case LZERROR_GLOBLOCK:
				return "Bad global handle.";
			case LZERROR_READ:
				return "Corrupt compressed file format.";
			case LZERROR_UNKNOWNALG:
				return "Compression algorithm not recognized.";
			case LZERROR_WRITE:
				return "Out of space for output file.";
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
