#include	"SafeProgramming.h"

#include	"GraphicsClasses.h"
#include	"Object.h"

#include	<windows.h>

#include	"MyString.h"
#include	"GameWindow.h"
#include	"Misc.h"

#ifdef USE_GAPI
	#include	"gx.h"
#endif


extern FILE	*DFH;

CGraphics::CGraphics()
{
	WindowHandle			= NULL;
	PlatformRef				= NULL;
	BackBuffer				= NULL;
	PrimaryBuffer			= NULL;
	ForceFullRedraw			= true;
	NumberColoursMatched	= 0;

	SystemPaletteCount		= 0;
	SoundSystemRef			= NULL;
}

CGraphics::~CGraphics()
{
	SAFELY_DELETE_ARRAY(BackBuffer);
	SAFELY_DELETE_ARRAY(PrimaryBuffer);
		
#ifdef USE_GAPI
	GXCloseDisplay();
#endif
}

// ----
// Init the object.
bool	CGraphics::Init(HWND _WindowHandle, HPALETTE _Palette, CGameData* _GameDataRef, CPlatform*	_PlatformRef)
{
	// Store.
	WindowHandle			= _WindowHandle;
	PlatformRef				= _PlatformRef;
	GameDataRef				= _GameDataRef;
	Palette					= _Palette;
	NumberColoursMatched	= 0;

	DEBUG_LOCALE("In Graphics::Init");

#ifdef USE_GAPI
	// Close display if already open.
	GXCloseDisplay();

	DEBUG_LOCALE("Closed GAPI display");

	bool ret = (GXOpenDisplay(WindowHandle, GX_FULLSCREEN) == 1);
	if (!ret)
		ErrorCode = GetLastError();
//	else
		DispProperties = GXGetDisplayProperties();

	DEBUG_LOCALE("Opened and got display properties");
	DEBUG_VALUE(DispProperties.cxWidth);
	DEBUG_VALUE(DispProperties.cyHeight);

	DEBUG_VALUE(DispProperties.cbxPitch);
	DEBUG_VALUE(DispProperties.cbyPitch);

	DEBUG_VALUE(DispProperties.cBPP);
	DEBUG_VALUE(DispProperties.ffFormat);

/*
	DWORD cxWidth;
	DWORD cyHeight;			// notice lack of 'th' in the word height.
	long cbxPitch;			// number of bytes to move right one x pixel - can be negative.
	long cbyPitch;			// number of bytes to move down one y pixel - can be negative.
	long cBPP;				// # of bits in each pixel
	DWORD ffFormat;	
  */

	// Set viewport. 
	if (!GXSetViewport(0, DispProperties.cyHeight, 0, 0))
		return false;

	DEBUG_LOCALE("Checking double buffering");
	if (GameDataRef->GetPreferences()->DoubleBufferingEnabled)
	{
		DEBUG_LOCALE("Double buffering is enabled");

		// My favourite device!
		// Needs double buffering, thus create buffer here and now.
		//char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];
		BackBuffer		= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
										abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];
		PrimaryBuffer	= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
										abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];
		//PrimaryBuffer	= new char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];
		ForceFullRedraw	= true;

		DEBUG_LOCALE("Allocated buffers");
	}
	else
	{
		DEBUG_LOCALE("Double buffering is disabled");

		BackBuffer		= NULL;
		PrimaryBuffer	= NULL;
		ForceFullRedraw	= false;
	}

	DEBUG_LOCALE("And out...");
	
	//	return ret;
		return true;
#else
	return true;
#endif
}

// ----
void	CGraphics::AssignPlatform(CPlatform* _Platform)
{
	PlatformRef = _Platform;
}

// ----
void	CGraphics::AssignSoundSystem(CSoundSystem* _SoundSystemRef)
{
	SoundSystemRef = _SoundSystemRef;
}


// ----
CPlatform*	CGraphics::GetPlatform()
{
	return PlatformRef;
}


// ----
#ifdef USE_GAPI
GXDisplayProperties*	CGraphics::GetDisplay()
{
	return &DispProperties;
}
#endif


// ----
bool	CGraphics::StartDraw()
{
	if (SoundSystemRef)
		SoundSystemRef->Process();

#ifdef USE_GAPI
	GraphicsMemAddress = GXBeginDraw();
	if (GraphicsMemAddress != NULL)
	{
	//	Sleep(1000 / 4);	
		return true;
	}
	
	ErrorCode = GetLastError();
	return false;
#else
	Sleep(1000 / 25);	// Frame limiter!! (a very basic one at that)
	InvalidateRect(WindowHandle, NULL, true);
	hdc = BeginPaint(WindowHandle, &ps);
	return true;
#endif
}

// ----
bool	CGraphics::EndDraw()
{
	if (SoundSystemRef)
		SoundSystemRef->Process();
#ifdef USE_GAPI
	void			*TempVoid;

	if (GraphicsMemAddress)
	{
		if (BackBuffer)
		{
			unsigned char	*WriteAddress				= (unsigned char*)GraphicsMemAddress;
			unsigned char	*WriteAddressLineFeed		= WriteAddress;

			// Read address.
			unsigned char	*ReadAddress				= NULL;
			if (DispProperties.cbyPitch < 0)
				ReadAddress = (unsigned char*)BackBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				ReadAddress = (unsigned char*)BackBuffer;
			unsigned char	*ReadAddressLineFeed		= ReadAddress;

			// Shadow buffer.
			unsigned char	*ShadowReadAddress			= NULL;
			if (DispProperties.cbyPitch < 0)
				ShadowReadAddress = (unsigned char*)PrimaryBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				ShadowReadAddress = (unsigned char*)PrimaryBuffer;
			unsigned char	*ShadowReadAddressLineFeed	= ShadowReadAddress;

			int				x, y;		

			switch (DispProperties.cBPP)
			{
			case 8:
				if (ForceFullRedraw)
				{
					for (y = DispProperties.cyHeight; y--;)
					{
						for (x = DispProperties.cxWidth; x--;)
						{
							*WriteAddress			= *ReadAddress;
													
							WriteAddress			+= DispProperties.cbxPitch;
							ReadAddress				+= DispProperties.cbxPitch;
						}									
															
						WriteAddressLineFeed		+= DispProperties.cbyPitch;
						WriteAddress				= WriteAddressLineFeed;
						ReadAddressLineFeed			+= DispProperties.cbyPitch;
						ReadAddress					= ReadAddressLineFeed;
					}
				}
				else
				{
					unsigned char	Value;

					if (GameDataRef->GetPreferences()->SlowVRAMWorkAround)
					{

						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*ReadAddress;

								if (Value != (*ShadowReadAddress))
									*WriteAddress		= Value;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
								ShadowReadAddress		+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
					}
					else
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*WriteAddress		= *ReadAddress;
								WriteAddress		+= DispProperties.cbxPitch;
								ReadAddress			+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
				}
				break;

			case 16:
				
				if (ForceFullRedraw)
				{
					for (y = DispProperties.cyHeight; y--;)
					{
						for (x = DispProperties.cxWidth; x--;)
						{
							*(WORD*)WriteAddress	= *(WORD*)ReadAddress;

							WriteAddress			+= DispProperties.cbxPitch;// <<1;
							ReadAddress				+= DispProperties.cbxPitch;// <<1;

						}							
													
						WriteAddressLineFeed		+= DispProperties.cbyPitch;
						WriteAddress				= WriteAddressLineFeed;
						ReadAddressLineFeed			+= DispProperties.cbyPitch;
						ReadAddress					= ReadAddressLineFeed;
					}
				}
				else
				{
					WORD	Value;
					int	Step = DispProperties.cbxPitch;// * 2;

					if (GameDataRef->GetPreferences()->SlowVRAMWorkAround)
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*(WORD*)ReadAddress;
								if (Value != (*(WORD*)ShadowReadAddress))
									*(WORD*)WriteAddress	= Value;

								WriteAddress			+= Step;//DispProperties.cbxPitch;
								ReadAddress				+= Step;//DispProperties.cbxPitch;
								ShadowReadAddress		+= Step;//DispProperties.cbxPitch;

							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
					}
					else
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*(WORD*)WriteAddress	= *(WORD*)ReadAddress;;
								WriteAddress			+= Step;//DispProperties.cbxPitch;
								ReadAddress				+= Step;//DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
				}
				break;

			case 32:
				DWORD	Value;

				if (DispProperties.cBPP == 3)
				{
					// Will write this one day!
				}
				else if (DispProperties.cBPP == 4)
				{
					if (ForceFullRedraw)
					{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								*(DWORD*)WriteAddress	= *(DWORD*)ReadAddress;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
							}							
														
							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
						}
					}
					else
					{
						if (GameDataRef->GetPreferences()->SlowVRAMWorkAround)
						{
						for (y = DispProperties.cyHeight; y--;)
						{
							for (x = DispProperties.cxWidth; x--;)
							{
								Value					= 	*(DWORD*)ReadAddress;

								if (Value != (*(DWORD*)ShadowReadAddress))
									*(DWORD*)WriteAddress	= Value;

								WriteAddress			+= DispProperties.cbxPitch;
								ReadAddress				+= DispProperties.cbxPitch;
								ShadowReadAddress		+= DispProperties.cbxPitch;
							}

							WriteAddressLineFeed		+= DispProperties.cbyPitch;
							WriteAddress				= WriteAddressLineFeed;
							ReadAddressLineFeed			+= DispProperties.cbyPitch;
							ReadAddress					= ReadAddressLineFeed;
							ShadowReadAddressLineFeed	+= DispProperties.cbyPitch;
							ShadowReadAddress			= ShadowReadAddressLineFeed;
						}
						}
						else
						{
							for (y = DispProperties.cyHeight; y--;)
							{
								for (x = DispProperties.cxWidth; x--;)
								{
									*(DWORD*)WriteAddress	= *(DWORD*)ReadAddress;

									WriteAddress			+= DispProperties.cbxPitch;
									ReadAddress				+= DispProperties.cbxPitch;
								}

								WriteAddressLineFeed		+= DispProperties.cbyPitch;
								WriteAddress				= WriteAddressLineFeed;
								ReadAddressLineFeed			+= DispProperties.cbyPitch;
								ReadAddress					= ReadAddressLineFeed;
							}
						}
					}
				}
				break;
			}
			
			if (ForceFullRedraw)
			{
				ForceFullRedraw	= false;
				if (GameDataRef->GetPreferences()->SlowVRAMWorkAround)
				{
					memcpy(PrimaryBuffer, BackBuffer, max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight),
															abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth)));
				}
			}

			if (GameDataRef->GetPreferences()->SlowVRAMWorkAround)
			{
				// Swap buffer handles
				TempVoid		= BackBuffer;
				BackBuffer		= PrimaryBuffer;
				PrimaryBuffer	= TempVoid;
			}
		
		}
		GraphicsMemAddress = NULL;
	
		// Check whether double buffering state has changed.
		if (GameDataRef->GetPreferences()->DoubleBufferingEnabled)
		{
			if (!BackBuffer)
			{
				BackBuffer		= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
												abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];
				ForceFullRedraw	= true;
			}

			if (!PrimaryBuffer)
			{
				PrimaryBuffer	= new char[max(	abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight), 
												abs((int)DispProperties.cbxPitch * (int)DispProperties.cxWidth))];//char[abs((int)DispProperties.cbyPitch * (int)DispProperties.cyHeight)];
				ForceFullRedraw	= true;
			}
		}
		else
		{
			SAFELY_DELETE_ARRAY(BackBuffer);
			SAFELY_DELETE_ARRAY(PrimaryBuffer);
		}

		bool ret = (GXEndDraw() == 1);
		if (!ret)
			ErrorCode = GetLastError();

		return ret;
	}
	return true;	// Already ended.
#else
	EndPaint(WindowHandle, &ps);
	return true;
#endif
}

// ----
// Calling this method will force the next EndDraw to 
// force a full update of the display. Useful when the app 
// is reactivated.
void	CGraphics::ForceRedraw()
{
	ForceFullRedraw = true;
}

// ----
bool	CGraphics::ClearScreen(UINT R, UINT G, UINT B)
{
#ifdef USE_GAPI
	RECT all;
	all.top = 0;
	all.left = 0;
	all.bottom = DispProperties.cyHeight;
	all.right = DispProperties.cxWidth;

	ForceFullRedraw = true;

	return DrawRect(&all, R, G, B);
#else
	return true;
#endif
}

// ----
bool	CGraphics::DrawRect(RECT* _Region, UINT R, UINT G, UINT B)
{
	if (SoundSystemRef)
		SoundSystemRef->Process();	
#ifdef USE_GAPI
	if (	(GraphicsMemAddress)				// No pointer to display memory.
		//&&	(DispProperties.ffFormat & kfDirect)// The display is direct mapped (not a palettised display).
		)
	{
		void*	WriteBuffer;
		if (BackBuffer)
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)BackBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = BackBuffer;
		}
		else
			WriteBuffer = GraphicsMemAddress;

		unsigned char*	WriteAddress			= (unsigned char*)WriteBuffer + _Region->top * DispProperties.cbyPitch + _Region->left * DispProperties.cbxPitch;
		unsigned char*	WriteAddressLineFeed	= WriteAddress;

		switch (DispProperties.cBPP)
		{
		case 32:
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					for (int y = _Region->top; y < _Region->bottom; y++)
					{
						for (int x = _Region->left; x < _Region->right; x++)
						{
							*(WriteAddress)		= R;
							*(WriteAddress + 1)	= G;
							*(WriteAddress + 2)	= B;
							WriteAddress += DispProperties.cbxPitch;
						}

						WriteAddressLineFeed += DispProperties.cbyPitch;
						WriteAddress = WriteAddressLineFeed;
					}

				}
				else if (DispProperties.cbxPitch == 4)
				{
					DWORD Value;
					Value = (R << 16) + (G << 8) + B;

					for (int y = _Region->top; y < _Region->bottom; y++)
					{
						for (int x = _Region->left; x < _Region->right; x++)
						{
							*(DWORD*)WriteAddress = Value;
							WriteAddress += DispProperties.cbxPitch;
						}

						WriteAddressLineFeed += DispProperties.cbyPitch;
						WriteAddress = WriteAddressLineFeed;
					}
				}

				return true;
			}
			break;

		case 16:
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565))
			{
				WORD Value;
				if (PlatformRef)
				{	
					if (DispProperties.ffFormat & kfDirect565)
					{
						R = R >> 3;
						G = G >> 2;
						B = B >> 3;

						Value = (R << 11) + 
								(G << 5) + 
								B;
					}
					else
					{
						R = R >> 3;
						G = G >> 3;
						B = B >> 3;

						Value = (R << 10) + 
								(G << 5) + 
								B;
					}
				}
				else
					return false;

				for (int y = _Region->top; y < _Region->bottom; y++)
				{
					for (int x = _Region->left; x < _Region->right; x++)
					{
						*(WORD*)WriteAddress = Value;
						WriteAddress += DispProperties.cbxPitch;
					}

					WriteAddressLineFeed += DispProperties.cbyPitch;
					WriteAddress = WriteAddressLineFeed;
				}

				return true;
			}
			break;

		case 8:
			// This is not going to work unless I set an appropriate
			// palette,	as the display is not direct address able.
			unsigned char Value;
			if (PlatformRef)
			{	
				if (DispProperties.ffFormat & kfDirect)
				{
					// Direct 8 bit mode. 
					R = (R >> 5);
					G = (G >> 5);
					B = (B >> 6);

					Value = (R << 5) + (G << 2) + B;
				}
				else
				{
					COLORREF InColour;
					InColour = (B << 16) + (G << 8) + R;

					// Making the assumption that the palette is a maximum of 256 entries!!!
					UINT mapcol = Match8BitPalette(InColour);//GetNearestPaletteIndex(Palette, InColour);
					if (mapcol != CLR_INVALID)
						Value = mapcol;
					else
						Value = 0;
				}

				for (int y = _Region->top; y < _Region->bottom; y++)
				{
					for (int x = _Region->left; x < _Region->right; x++)
					{
						*(unsigned char*)WriteAddress = Value;
						WriteAddress += DispProperties.cbxPitch;
					}

					WriteAddressLineFeed += DispProperties.cbyPitch;
					WriteAddress = WriteAddressLineFeed;
				}

				return true;
			}
			break;

		default:
			break;
		}
	}
	
	// Either error, or unsupported.
	return false;
#else
	return true;
#endif
}


// ----
bool	CGraphics::DrawSprite(	int				_XPos, 
								int				_YPos,
								CGraphicsData*	_GraphicsData,
								RECT&			_Region,
								int				_Flip)
{
	if (SoundSystemRef)
		SoundSystemRef->Process();

#ifdef USE_GAPI
	if (	(GraphicsMemAddress)				// No pointer to display memory.
		&&	(_GraphicsData)						// No data
		//&&	(DispProperties.ffFormat & kfDirect)// The display is direct mapped (not a palettised display).
		&&	((_XPos + _Region.right - _Region.left) > 0)	// Off left hand side
		&&	((_YPos + _Region.bottom - _Region.top) > 0)	// Off top of screen.
		&&	(_XPos < (int)DispProperties.cxWidth)	// Off right hand side
		&&	(_YPos < (int)DispProperties.cyHeight)	// Off bottom of screen.
		)
	{
		void*	WriteBuffer;
		if (BackBuffer)
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)BackBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = BackBuffer;
		}
		else
			WriteBuffer = GraphicsMemAddress;

		int		ReadOffsetX, ReadOffsetY;
		int		ReadDirectionX, ReadDirectionY;
		int		x, y;

		if (_Flip)
		{
			if (_Flip & SPRITE_ROTATE_180)
			{
				ReadOffsetX	= _Region.right -1;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = -1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_VERTICAL)
			{
				ReadOffsetX	= _Region.left;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = 1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_HORIZONTAL)
			{
				ReadOffsetX	= _Region.right - 1;
				ReadOffsetY	= _Region.top;

				ReadDirectionX = -1;
				ReadDirectionY = 1;
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
		else
		{
			ReadOffsetX	= _Region.left;
			ReadOffsetY	= _Region.top;
			ReadDirectionX = 1;
			ReadDirectionY = 1;
		}


		switch (DispProperties.cBPP)
		{
		case 32:	// 32 bit display.
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer 
														+ 	(_YPos * DispProperties.cbyPitch) 
														+ 	(_XPos * DispProperties.cbxPitch));
					DWORD			*ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

					int UpperX, UpperY;
					UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
					UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

					for (y = _YPos; y < UpperY; y++)
					{
						if (y >= 0)
						{
							for (x = _XPos; x < UpperX; x++)
							{
								if (	(x >= 0)
									&&	(*ReadAddress != _GraphicsData->GetTransKey32()))
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}

								WriteAddress += DispProperties.cbxPitch;
								ReadAddress += ReadDirectionX;
							}
						}

						// Prepare for next line.
						WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

						ReadOffsetY += ReadDirectionY;
						ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
					}
					return true;
				}
				else if (DispProperties.cbxPitch == 4) // 32 bit. (much easier)
				{
					unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
					DWORD			*ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

					int UpperX, UpperY;
					UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
					UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

					DWORD Trans = _GraphicsData->GetTransKey32();

					for (y = _YPos; y < UpperY; y++)
					{
						if (y >= 0)
						{
							for (x = _XPos; x < UpperX; x++)
							{
								if (	(x >= 0)
									&&	(*ReadAddress != Trans))
									*(DWORD*)WriteAddress = *ReadAddress;

								WriteAddress += DispProperties.cbxPitch;
								ReadAddress += ReadDirectionX;
							}
						}

						// Prepare for next line.
						WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

						ReadOffsetY += ReadDirectionY;
						ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
					}
					return true;
				}

			}
			break;

		case 16:	// 16 bit display.
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565)
				)
			{
				unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
				WORD			*ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

				int UpperX, UpperY;
				UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
				UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

				WORD Trans = _GraphicsData->GetTransKey16();
				for (y = _YPos; y < UpperY; y++)
				{
					if (y >= 0)
					{
						for (x = _XPos; x < UpperX; x++)
						{
							if (	(x >= 0)
								&&	(*ReadAddress != Trans))
								*(WORD*)WriteAddress = *ReadAddress;

							WriteAddress += DispProperties.cbxPitch;
							ReadAddress += ReadDirectionX;
						}
					}

					// Prepare for next line.
					WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

					ReadOffsetY += ReadDirectionY;
					ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
				}

				return true;
			}
			break;

		case 8:		// 8 bit display.
			unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
			unsigned char	*ReadAddress	= (unsigned char*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

			int UpperX, UpperY;
			UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
			UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

			unsigned char Trans = _GraphicsData->GetTransKey8();
			for (y = _YPos; y < UpperY; y++)
			{
				if (y >= 0)
				{
					for (x = _XPos; x < UpperX; x++)
					{
						if (	(x >= 0)
							&&	(*ReadAddress != Trans))
							*(unsigned char*)WriteAddress = *ReadAddress;

						WriteAddress += DispProperties.cbxPitch;
						ReadAddress += ReadDirectionX;
					}
				}

				// Prepare for next line.
				WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

				ReadOffsetY += ReadDirectionY;
				ReadAddress	= (unsigned char*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
			}
			return true;
		}
	}
	//OutputDebugString(TEXT("Attempting to draw sprite, when the graphics memory is not accessable!\n"));
	return false;
#else
	return (GraphicsMemAddress != NULL);
#endif
}

// ----
bool	CGraphics::DrawNonTransSprite(	int				_XPos, 
										int				_YPos,
										CGraphicsData*	_GraphicsData,
										RECT&			_Region,
										int				_Flip)
{
	if (SoundSystemRef)
		SoundSystemRef->Process();

	// Data is in _GraphicsData - no specific format or means of processing yet.
#ifdef USE_GAPI
	if (	(GraphicsMemAddress)				// No pointer to display memory.
		&&	(_GraphicsData)						// No data
		//&&	(DispProperties.ffFormat & kfDirect)// The display is direct mapped (not a palettised display).
		&&	(_XPos < (int)DispProperties.cxWidth)	// Off right hand side
		&&	(_YPos < (int)DispProperties.cyHeight)	// Off bottom of screen.
		)
	{
		void*	WriteBuffer;
		if (BackBuffer)
		{
			if (DispProperties.cbyPitch < 0)
				WriteBuffer = (char*)BackBuffer + ((DispProperties.cyHeight - 1) * (-DispProperties.cbyPitch));
			else
				WriteBuffer = BackBuffer;
		}
		else
			WriteBuffer = GraphicsMemAddress;

		int		ReadOffsetX, ReadOffsetY;
		int		ReadDirectionX, ReadDirectionY;
		int		x, y;

		if (_Flip)
		{
			if (_Flip & SPRITE_ROTATE_180)
			{
				ReadOffsetX	= _Region.right -1;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = -1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_VERTICAL)
			{
				ReadOffsetX	= _Region.left;
				ReadOffsetY	= _Region.bottom -1;
				ReadDirectionX = 1;
				ReadDirectionY = -1;
			}
			else if (_Flip & SPRITE_FLIP_HORIZONTAL)
			{
				ReadOffsetX	= _Region.right - 1;
				ReadOffsetY	= _Region.top;

				ReadDirectionX = -1;
				ReadDirectionY = 1;
			}
			else
			{
				ASSERT(0);
				return false;
			}
		}
		else
		{
			ReadOffsetX	= _Region.left;
			ReadOffsetY	= _Region.top;
			ReadDirectionX = 1;
			ReadDirectionY = 1;
		}


		switch (DispProperties.cBPP)
		{
		case 32:	// 32 bit display.
			if (DispProperties.ffFormat & kfDirect888)
			{
				if (DispProperties.cbxPitch == 3)	// 24 bit
				{
					unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer 
														+ 	(_YPos * DispProperties.cbyPitch) 
														+ 	(_XPos * DispProperties.cbxPitch));
					DWORD			*ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

					int UpperX, UpperY;
					UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
					UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

					for (y = _YPos; y < UpperY; y++)
					{
						if (y >= 0)
						{
							for (x = _XPos; x < UpperX; x++)
							{
								if (	(x >= 0)
									&&	(*ReadAddress != _GraphicsData->GetTransKey32()))
								{
									*WriteAddress		= ((*ReadAddress) >> 16);
									*(WriteAddress + 1)	= ((*ReadAddress) >> 8);
									*(WriteAddress + 2)	= (*ReadAddress);
								}

								WriteAddress += DispProperties.cbxPitch;
								ReadAddress += ReadDirectionX;
							}
						}

						// Prepare for next line.
						WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

						ReadOffsetY += ReadDirectionY;
						ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
					}
					return true;
				}
				else if (DispProperties.cbxPitch == 4) // 32 bit. (much easier)
				{
					unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
					DWORD			*ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

					int UpperX, UpperY;
					UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
					UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

					for (y = _YPos; y < UpperY; y++)
					{
						if (y >= 0)
						{
							for (x = _XPos; x < UpperX; x++)
							{
								if (x >= 0)
									*(DWORD*)WriteAddress = *ReadAddress;

								WriteAddress += DispProperties.cbxPitch;
								ReadAddress += ReadDirectionX;
							}
						}

						// Prepare for next line.
						WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

						ReadOffsetY += ReadDirectionY;
						ReadAddress	= (DWORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
					}
					return true;
				}

			}
			break;

		case 16:	// 16 bit display.
			if (	(DispProperties.ffFormat & kfDirect555)
				||	(DispProperties.ffFormat & kfDirect565)
				)
			{
				unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
				WORD			*ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

				int UpperX, UpperY;
				UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
				UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

				for (y = _YPos; y < UpperY; y++)
				{
					if (y >= 0)
					{
						for (x = _XPos; x < UpperX; x++)
						{
							if (x >= 0)
								*(WORD*)WriteAddress = *ReadAddress;

							WriteAddress += DispProperties.cbxPitch;
							ReadAddress += ReadDirectionX;
						}
					}

					// Prepare for next line.
					WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

					ReadOffsetY += ReadDirectionY;
					ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
				}

				return true;
			}
			break;

		case 8:		// 8 bit display.
			unsigned char	*WriteAddress	= ((unsigned char*)WriteBuffer + (_YPos * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));
			unsigned char	*ReadAddress	= (unsigned char*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);

			int UpperX, UpperY;
			UpperX = min(_XPos + _Region.right - _Region.left, (int)DispProperties.cxWidth);
			UpperY = min(_YPos + _Region.bottom - _Region.top, (int)DispProperties.cyHeight);

			for (y = _YPos; y < UpperY; y++)
			{
				if (y >= 0)
				{
					for (x = _XPos; x < UpperX; x++)
					{
						if (x >= 0)
							*WriteAddress = *ReadAddress;

						WriteAddress += DispProperties.cbxPitch;
						ReadAddress += ReadDirectionX;
					}
				}

				// Prepare for next line.
				WriteAddress	= ((unsigned char*)WriteBuffer + ((y + 1) * DispProperties.cbyPitch) + (_XPos * DispProperties.cbxPitch));

				ReadOffsetY += ReadDirectionY;
				ReadAddress	= (unsigned char*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
			}
			return true;
		}
	}
	return false;
#else
	return (GraphicsMemAddress != NULL);
#endif
}


// ----
// Draw string - this may need optamising later!
bool	CGraphics::DrawString(	CFont*		_Font,
								CString*	_String,
								int			_XPos,
								int			_YPos,
								DWORD		_StyleBits,
								POINT*		_Point,
								bool*		_PointIsInDrawRegion)
{
	if (SoundSystemRef)
		SoundSystemRef->Process();

#ifdef USE_GAPI

	int i, Width;
	RECT bob;
	RECT CharRegion;
	int	XDest, YDest;

	if (_Font)
	{
		CGraphicsData* FontGraphicHandle;
				
		switch (_StyleBits)
		{
		case TEXT_LEFT:	
		case (TEXT_RIGHT | TEXT_CENTRED):	// Both right and centre set
			// Text should use the x, y values as hard left values. 
			// That is, the text should appear to the right of the x, y values.
			bob.top		= _YPos;
			bob.left	= _XPos;
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos + _Font->GetStringWidth(_String);

			XDest = bob.left;
			YDest = bob.top;
			break;

		case TEXT_RIGHT:
			// x is hard right
			bob.top		= _YPos;
			bob.left	= _XPos - _Font->GetStringWidth(_String);
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos;

			XDest = bob.left;
			YDest = bob.top;
			break;

		case TEXT_CENTRED:
			bob.top		= _YPos;
			bob.left	= _XPos - (_Font->GetStringWidth(_String) >> 1);
			bob.bottom	= _YPos + _Font->GetLineHeight();
			bob.right	= _XPos + (_Font->GetStringWidth(_String) >> 1);
			
			XDest = bob.left;
			YDest = bob.top;
			break;

		default: 
			return false;
		}

		int OriginX = XDest;

		// Now we've worked out the start rendering positions 
		// we had better actually render the string.
		for (i = 0; i < (int)_String->GetStringLength(); i++)
		{
			_Font->GetCharData(_String->GetString()[i], &FontGraphicHandle, &CharRegion, &Width);
			if (_String->GetString()[i] == '\n') 
			{
				// New line
				XDest = OriginX;
				YDest += (_Font->GetLineHeight());
				bob.bottom += (_Font->GetLineHeight());	// Move the bottom bound down.
			}
			else
			{
				if (FontGraphicHandle)
					DrawSprite(XDest, YDest, FontGraphicHandle, CharRegion);
				XDest += Width;
			}
		}
		
		if (	(_Point)
			&&	(_PointIsInDrawRegion)
			)
		{
			if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)

				&&	(_Point->x > bob.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < bob.right) 
				&&	(_Point->y > bob.top) 
				&&	(_Point->y < bob.bottom) 
				)
				*_PointIsInDrawRegion = true;	// Yes.
			else
				*_PointIsInDrawRegion = false;	// No, to one of the above.
		}

		return true;//DrawRect(&bob, 0, 0, 0);
	}

	return false;

#else
	RECT		rt;
	
	if (GetForegroundWindow() == WindowHandle) 
	{
		GetClientRect(WindowHandle, &rt);
		rt.top = _YPos;
		rt.left = _XPos;

		if (!DrawText(	hdc, 
						_String->GetString(), 
						_String->GetStringLength(), 
						&rt, 
						DT_WORDBREAK | DT_NOPREFIX)//;//DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			)
		{
			SAFELY_PRINT_OBJECT(_String);
		}
		else
		{
			if (	(_Point)
				&&	(_PointIsInDrawRegion))
			{
			/*	if (

		//			BOOL GetTextExtentPoint( 
					//HDC hdc, 
						LPCTSTR lpString, 
int cbString, 
LPSIZE lpSize );
*/
			}
		}
	}

	return true;
#endif
}

#ifdef NOT_FINAL_BUILD
void CGraphics::SaveScreen(CString* _FileName)
{
	unsigned char	*ReadAddress	= ((unsigned char*)GraphicsMemAddress + (0 * DispProperties.cbyPitch) + (0 * DispProperties.cbxPitch));
	//WORD			*ReadAddress	= (WORD*)_GraphicsData->GetAddress(ReadOffsetX, ReadOffsetY);
	WORD			Value, RValue, GValue, BValue;
	unsigned char	R, G, B;

	TGAHeader	Header;
	/*
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
	*/
	
	memset((void*)&Header, 0, sizeof(Header));
	Header.mImageType	= 2;
	Header.mSizeX		= DispProperties.cxWidth;
	Header.mSizeY		= DispProperties.cyHeight;
	Header.mPixelDepth	= 24;
		
	// Allocate image memory.
	char	*OutputTGA	= new char[sizeof(Header) + 3 * DispProperties.cxWidth * DispProperties.cyHeight];
	memcpy((void*)OutputTGA, (void*)&Header, sizeof(Header));
	
	char	*WriteAddress = OutputTGA + sizeof(Header);
	for (int y = 0; y < (int)DispProperties.cyHeight; y++)
	{
		for (int x = 0; x < (int)DispProperties.cxWidth; x++)
		{
			Value = *(WORD*)ReadAddress;
			ReadAddress += DispProperties.cbxPitch;
			
			// Process data into 8 bit values
			RValue = (Value & 0xF800);	R = RValue >> 8;	// [11111 000] 000 00000
			GValue = (Value & 0x7E0);	G = GValue >> 3;	// [111111 00] 000
			BValue = (Value & 0x1F);	B = BValue << 3;	// [11111 (000)]

			*WriteAddress = B; WriteAddress++;
			*WriteAddress = G; WriteAddress++;
			*WriteAddress = R; WriteAddress++;
		}
		
		// Prepare for next line.
		ReadAddress	= ((unsigned char*)GraphicsMemAddress + ((y + 1) * DispProperties.cbyPitch));
	}

#ifdef _UNICODE
	FILE* FH = _wfopen(_FileName->GetString(), TEXT("wb"));
#else
	FILE* FH = fopen(_FileName->GetString(), TEXT("wb"));
#endif
	if (FH)
	{
		fwrite(OutputTGA, sizeof(Header) + 3 * DispProperties.cxWidth * DispProperties.cyHeight, 1, FH);
		fclose(FH);
	}
	delete[] OutputTGA;
}

#endif


// ----
UINT		CGraphics::Match8BitPalette(COLORREF	_a)
{
	UINT	mapcol;
	for (int i = NumberColoursMatched; i--;)
	{
		if (ColourMatchArray[i].In == _a)
			return ColourMatchArray[i].Out;
	}

	mapcol = GetNearestPalette(_a); // My method!
	if (mapcol != CLR_INVALID)
	{
		if (NumberColoursMatched < 128)
		{
			ColourMatchArray[NumberColoursMatched].In = _a;
			ColourMatchArray[NumberColoursMatched].Out = mapcol;
			NumberColoursMatched++;
		}
		else
		{
			// Free the top end of the array.
			NumberColoursMatched = 16;
			ColourMatchArray[NumberColoursMatched].In = _a;
			ColourMatchArray[NumberColoursMatched].Out = mapcol;
			NumberColoursMatched++;
		}
		
		return mapcol;
	}
	return 0;
}

// ----
UINT	CGraphics::GetNearestPalette(COLORREF _Colour)
{
	int	i, ret;
	ret = 0;
	unsigned char	R, G, B;
	unsigned char	RD, GD, BD;

	if (	(!SystemPaletteCount) 
		||	(!SystemPaletteRef))
		return CLR_INVALID;

	R = (_Colour >> 0) & 0xFF;
	G = (_Colour >> 8) & 0xFF;
	B = (_Colour >> 16) & 0xFF;

	RD = abs((SystemPaletteRef + ret)->peRed - R);
	GD = abs((SystemPaletteRef + ret)->peGreen - G);
	BD = abs((SystemPaletteRef + ret)->peBlue - B);

	for (i = 0; i < 256; i++)
	{
		if (	(abs((SystemPaletteRef + i)->peRed - R) <= RD)
			&&	(abs((SystemPaletteRef + i)->peGreen - G) <= GD)
			&&	(abs((SystemPaletteRef + i)->peBlue - B) <= BD))
		{
			ret = i;
			RD = abs((SystemPaletteRef + ret)->peRed - R);
			GD = abs((SystemPaletteRef + ret)->peGreen - G);
			BD = abs((SystemPaletteRef + ret)->peBlue - B);
		}
	}
	return ret;
}