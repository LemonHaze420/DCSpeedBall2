#include	"SafeProgramming.h"
#include	"GraphicsClasses.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"BasicTypes.h"
#include	<windows.h>
#include	<string.h>

#ifdef ULTRA_FAST_OBJECT
	#pragma message("WARNING: You MUST use font data created with an equal level of optamisation!!!")
#endif

// ----
CFont::CFont()
{
	FontGraphics	= NULL;
}


// ----
CFont::~CFont()
{
	SAFELY_DELETE(FontGraphics);
}


// ----
// Init the object.
bool	CFont::Init(CPlatform*	_PlatformRef,
					CGraphics*	_GraphicsObject,
					CString*	_FileName)
{
	// Clear memory and release any old graphics data. 
	memset((void*)&FontData, 0 , sizeof(FontData));
	SAFELY_DELETE(FontGraphics);

	if (	(_PlatformRef)
		&&	(_GraphicsObject)
		&&	(_FileName) 
		&&	(_FileName->GetStringLength() > 0)
		)
	{
		FILE*	FH;
		CString	DataPath;
		DataPath = _PlatformRef->GetPath(EP_Fonts);
		DataPath += _FileName;
		DataPath += ".dat";

		CString	ImageData;
		ImageData = _PlatformRef->GetPath(EP_Fonts);
		ImageData += _FileName;

#ifdef _UNICODE
		FH = _wfopen(DataPath.GetString(), TEXT("rb"));
#else
		FH = fopen(DataPath.GetString(), TEXT("rb"));
#endif
		if (FH)
		{
			if (fread((void*)&FontData, sizeof(FontData), 1, FH) == 1)
			{
				fclose(FH);

				// Now for the graphics.
				NEW(FontGraphics, CGraphicsData());
				return SAFELY_RETURN(FontGraphics, Init(_GraphicsObject, &ImageData, NULL));
			}

			// Close file even if read incorrect number of bytes.
			fclose(FH);
		}
	}	// _FileName && _FileName->GetStringLength() > 0

	return false;
}


// ----
// Obtain the information require to draw a single character.
// Unknown characters are ignored, and the method will return false.
bool	CFont::GetCharData(	TCHAR			_Char,			// in
							CGraphicsData**	_GraphicsData,	// out
							RECT*			_Region,		// out
							int*			_Width)			// out
{
	UINT	Index = (UINT)_Char;

	if (Index < MAX_CHARS)
	{
#if HACKING_ABOUT_WITH_THE_FONTS
#define CHARWIDTH 12

		// This is a bit hacked to work with a test bitmap.
		_Region->top = 0;	
		_Region->bottom = 16;	

		if (Index == 32)
		{
			_Region->top	= 0;
			_Region->left	= 0;
			_Region->bottom	= 0;
			_Region->right	= 0; 
			*_GraphicsData	= NULL;
			*_Width			= CHARWIDTH;
			return true;
		}
		else if ((Index >= 48)  && (Index <= 58))
		{
			// Numeric char
			_Region->left = (25 + Index - 48) * 16;
		}
		else if ((Index >= 65)  && (Index <= 90))
		{
			// upper case
			_Region->left = (Index - 65) * 16;
		}
		else if ((Index >= 97)  && (Index <= 122))
		{
			// lower case
			_Region->left = (Index - 97) * 16;
		}
		else
		{
			_Region->top	= 0;
			_Region->left	= 0;
			_Region->bottom	= 0;
			_Region->right	= 0; 
			*_GraphicsData	= NULL;
			*_Width			= 0;
			return false;
		}

		_Region->right = _Region->left + CHARWIDTH;
		*_GraphicsData	= FontGraphics;
		*_Width = CHARWIDTH;

#else

		*_GraphicsData		= FontGraphics;
		SmallRect* RectPtr	= &(FontData.CharDataArray[Index].Region);
		_Region->top		= RectPtr->top;
		_Region->bottom		= RectPtr->bottom;
		_Region->left		= RectPtr->left;
		_Region->right		= RectPtr->right;
		*_Width				= FontData.CharDataArray[Index].Width;
#endif
		
		return true;	
	}

	// Otherwise, return everything as NULL or 0.
	*_GraphicsData	= NULL;
	_Region->top	= 0;
	_Region->bottom	= 0;
	_Region->left	= 0;
	_Region->right	= 0;
	*_Width			= 0;

	return false;
}


// ----
// Gets the width of the string text in pixels. 
// Unknown characters are ignored, and the method will return zero.
int		CFont::GetStringWidth(CString*	_String)
{
	int ret = 0;
	if (_String)
	{	
		for (int i = _String->GetStringLength(); i--;)
		{
			UINT Index = (UINT)(_String->GetString()[i]);
			if (Index < MAX_CHARS)
			{
				ret += FontData.CharDataArray[Index].Width;
			}
		}
	}
	return ret;
}


// ----
int		CFont::GetLineHeight()
{
	//return 16;
	return FontData.LineHeight;
}