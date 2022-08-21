#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include	"SafeProgramming.h"
#include	"Object.h"
#include	<windows.h>
#include	"MapData.h"
#include	"MyString.h"
#include	"Platform.h"
#include	"GameData.h"
#include	"Misc.h"
#include	"BasicTypes.h"
#include	"SoundSystem.h"

#ifdef USE_GAPI
	#include	"gx.h"
#endif


// ----
// TGA file header format
struct TGAHeader
{
	BYTE	mIDLength;
	BYTE	mColourMapType;
	BYTE	mImageType;
	BYTE	mColourMapSpec[5];
	WORD	mOriginX;
	WORD	mOriginY;
	WORD	mSizeX;
	WORD	mSizeY;
	BYTE	mPixelDepth;
	BYTE	mImageDescriptor;
};

struct ColourMatch8
{
	COLORREF	In;
	UINT		Out;
};


// ----
// Class declarations.
class CGraphicsData;
class CFont;

// These modes should be mutually exclusive - if they are 
// not used as such then the default (left) shall be adopted.
#define	TEXT_LEFT				0		// Default
#define	TEXT_RIGHT				(1 << 0)	
#define	TEXT_CENTRED			(1 << 1)
								
#define	SPRITE_NORMAL			(0 << 12)
#define SPRITE_FLIP_VERTICAL	(1 << 12)
#define	SPRITE_FLIP_HORIZONTAL	(2 << 12)
#define	SPRITE_ROTATE_180		(4 << 12)

class CGraphics : public CObject
{
public:
	CGraphics();
	~CGraphics();

	// ----
	// Init the object.
	bool	Init(HWND _WindowHandle, HPALETTE _Palette, CGameData* _GameDataRef, CPlatform*	_PlatformRef);

	// ----
	void	AssignPlatform(CPlatform* _Platform);

	// ----
	void	AssignSysPalette(PALETTEENTRY		*SystemPalette, int PaletteCount) {SystemPaletteRef = SystemPalette; SystemPaletteCount = PaletteCount;}
	UINT	GetNearestPalette(COLORREF _Colour);

	// ----
	// If you assign a sound system object then the class 
	// will update the sound buffer everytime a graphics 
	// method is called. This is 'nice' as it should 
	// prevent audio stutter when drawing a large amount 
	// of graphics.
	void	AssignSoundSystem(CSoundSystem*	_SoundSystemRef);

	// ----
	CPlatform*	CGraphics::GetPlatform();

	// ----
#ifdef USE_GAPI
	GXDisplayProperties*	GetDisplay();
#endif

	// ----
	bool	StartDraw();
	bool	EndDraw();

	// ----
	void	ForceRedraw();

	// ----
	bool	ClearScreen(UINT R, UINT G, UINT B);

	// ----
	bool	DrawRect(RECT* _Region, UINT R, UINT G, UINT B);

	// ----
	bool	DrawSprite(	int				_XPos, 
						int				_YPos,
						CGraphicsData*	_GraphicsData, 
						RECT&			_Region,
						int				_Flip = SPRITE_NORMAL);


	// ----
	// Draw a sprite - with no check for transparency.
	bool	DrawNonTransSprite(	int				_XPos, 
								int				_YPos,
								CGraphicsData*	_GraphicsData,
								RECT&			_Region,
								int				_Flip = SPRITE_NORMAL);


	// ----
	// Draw string - this may need optamising later!
	bool	DrawString(	CFont*		_Font, 
						CString*	_String,
						int			_XPos,
						int			_YPos,
						DWORD		_StyleBits				= TEXT_LEFT,
						POINT*		_Point					= NULL,
						bool*		_PointIsInDrawRegion	= NULL);

#ifdef NOT_FINAL_BUILD
	void CGraphics::SaveScreen(CString* _FileName);
#endif

	UINT		Match8BitPalette(COLORREF	_a);


	// ----
	HPALETTE	GetPalette() {return Palette;}

protected:
	HWND				WindowHandle;
	HPALETTE			Palette;
	void*				GraphicsMemAddress;
	void*				BackBuffer;
	void*				PrimaryBuffer;
	bool				ForceFullRedraw;
	CPlatform*			PlatformRef;
	CGameData*			GameDataRef;

	CSoundSystem*		SoundSystemRef;
	PALETTEENTRY		*SystemPaletteRef;
	int					SystemPaletteCount;
	ColourMatch8		ColourMatchArray[1024];
	int					NumberColoursMatched;

#ifdef USE_GAPI
	GXDisplayProperties	DispProperties;
#else
	HDC					hdc;
	PAINTSTRUCT			ps;
#endif
};


// ----
struct ImageDataStruct
{
	DWORD TransparentKey;
};

class CGraphicsData : public CObject
{
public:
	CGraphicsData();
	~CGraphicsData();

	// ----
	// Load data / auto dither down to supported display depth.
	virtual bool	Init(	CGraphics*		_GraphicsObject, 
							CString*		_FileName,
							CPlatform*		_PlatformRef);
	
	// ----
	// Will clip offsets to outer bounds of data.
	virtual char*	GetAddress(int _XOffset, int _OffsetY);

	// ----
	virtual	DWORD	GetTransKey32();
	virtual	WORD	GetTransKey16();
	virtual	BYTE	GetTransKey8();

protected:
	char*			Data;
	CString			FileName;		// Might not need this - would save memory usage if removed.
	int				DataHeight, DataWidth;
	int				PixelSize;
	ImageDataStruct	ImageInfo;
};



#define	MAX_CHARS		256

// ----
// Store data for a single char
class	CCharData
{
public:
	SmallRect	Region;
	UINT		Width;	// So we can take the width of an item into account, even if we have no graphical data. - i.e. the space char.
};


// ----
// Hold data about 256 chars.
class	CFontData : public CObject
{
public:
	int			LineHeight;
	CCharData	CharDataArray[MAX_CHARS];
};


// ----
class CFont : public CObject
{
public:
	CFont();
	~CFont();

	// ----
	// Init the object.
	bool	Init(	CPlatform*	_PlatformRef,
					CGraphics*	_GraphicsObject,
					CString*	_FileName);

	// ----
	// Obtain the information require to draw a single character.
	// Unknown characters are ignored, and the method will return false.
	bool	GetCharData(TCHAR			_Char,			// in
						CGraphicsData**	_GraphicsData,	// out
						RECT*			_Region,		// out
						int*			_Width);		// out

	// ----
	// Gets the width of the string text in pixels. 
	// Unknown characters are ignored, and the method will return zero.
	int		GetStringWidth(CString*	_String);
	

	// ----
	int		GetLineHeight();

protected:

	CFontData		FontData;
	CGraphicsData*	FontGraphics;
};


#endif
