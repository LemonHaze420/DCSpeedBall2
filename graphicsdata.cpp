#include	"SafeProgramming.h"
#include	"GraphicsClasses.h"
#include	"Object.h"
#include	<windows.h>
#include	"GameDefines.h"
#include	"zlib.h"


CGraphicsData::CGraphicsData()
{
	Data = NULL;
	DataWidth = 0;
	DataHeight = 0;
	PixelSize = 0;
	// FileName is pre-initialised by its constructor.
}

CGraphicsData::~CGraphicsData()
{
	SAFELY_DELETE_ARRAY(Data);
}


// ----
// For loading a tga resource.
//#define IMAGE_EXTENSION			".tga"
//#define IMAGE_DATA_EXTENSION	".tgd"	// targa description.


// ----
// Load data / auto dither down to supported display depth.
// Passing in a valid CPlatform object instructs the method to prepend the images directory to the file name. 
// If you do not wish the file name to be modified, pass NULL instead. (This is the case for the CFonr::Init method).
bool	CGraphicsData::Init(CGraphics*		_GraphicsObject, 
							CString*		_FileName,
							CPlatform*		_PlatformRef)
{
#ifdef USE_GAPI
	FILE*			FH;
	char*			TempDataBuffer;
	char			gzPath[MAX_PATH];

	if (	(_GraphicsObject)
		&&	(_FileName) 
		&&	(_FileName->GetStringLength() > 0))
	{
		CString	ImagePath;

		if (_PlatformRef)
			ImagePath = _PlatformRef->GetPath(EP_ImageData);
		ImagePath				+= _FileName;
		ImagePath				+= ".cbd.gz";
		sprintf(gzPath, "%S", ImagePath.GetString());

		FH = gzopen(gzPath, "rb");
		// Load the data, etc.	
//#ifdef _UNICODE
//		FH = _wfopen(ImagePath.GetString(), TEXT("rb"));
//#else
//		FH = fopen(ImagePath.GetString(), TEXT("rb"));
//#endif
		if (FH)
		{
			gzread(FH, &ImageInfo, sizeof(ImageInfo));
			//fread(&ImageInfo, sizeof(ImageInfo), 1, FH);
		
			if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
			{
				int R, G, B;

				R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
				G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 2;
				B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

				ImageInfo.TransparentKey = (R << 11) + (G << 5) + B;
			}
			else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
			{
				int R, G, B;

				R = ((ImageInfo.TransparentKey >> 16) & 0xFF) >> 3;
				G = ((ImageInfo.TransparentKey >> 8	) & 0xFF) >> 3;
				B = ((ImageInfo.TransparentKey		) & 0xFF) >> 3;

				ImageInfo.TransparentKey = (R << 10) + (G << 5) + B;
			}
			else if (_GraphicsObject->GetDisplay()->cBPP == 8)
			{
				int R, G, B;

				R = (ImageInfo.TransparentKey >> 16	)	& 0xFF;
				G = (ImageInfo.TransparentKey >> 8	)	& 0xFF;
				B = (ImageInfo.TransparentKey		)	& 0xFF;

				if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
				{
					R = R >> 5;
					G = G >> 5;
					B = B >> 6;

					// Update for palettized display
					ImageInfo.TransparentKey = (R << 5) + (G << 2) + B;
				}
				else
				{
					COLORREF InColour;
					InColour = (B << 16) + (G << 8) + R;

					// Making the assumption that the palette is a maximum of 256 entries!!!
					ImageInfo.TransparentKey = _GraphicsObject->Match8BitPalette(InColour);//GetNearestPaletteIndex(_GraphicsObject->GetPalette(), InColour);
				}
			}
		
			// Read header
			TGAHeader	header;
			gzread(FH, &header, sizeof(header));
			//fread(&header, sizeof(header), 1, FH);

			// Store width and height info
			DataWidth	= header.mSizeX;
			DataHeight	= header.mSizeY;

			int	TotalCount;
			gzread(FH, &TotalCount, sizeof(TotalCount));
			//fread(&TotalCount, sizeof(TotalCount), 1, FH);
	
			// Read data into buffer, before processing.
			TempDataBuffer = new char[TotalCount * 4];
			if (TempDataBuffer)
			{
				gzread(FH, TempDataBuffer, TotalCount * 4);
				gzclose(FH);
				//fread(TempDataBuffer, TotalCount * 4, 1, FH);
				//fclose(FH);
					
				Data = NULL;
				switch (_GraphicsObject->GetDisplay()->cBPP)
				{
				case 8:
					Data = new char[DataWidth * DataHeight * 1];
					PixelSize = 1;
					break;
				case 16:
					Data = new char[DataWidth * DataHeight * 2];
					PixelSize = 2;
					break;
				case 24:
				case 32:
					Data = new char[DataWidth * DataHeight * 4]; // The data is stored in the format xRGB, reguardless of whether the screen is 24 or 32 bit.
					PixelSize = 4;
					break;
				default:
					Data = NULL;
					PixelSize = 0;
					return false;
				}

				if (Data)
				{
					// Right, got source and destination buffers. 
					// Time to process data.

					char*			Source	= TempDataBuffer;
					//Source	+= (DataHeight - 1) * DataWidth * 3;
					char*			Dest	= Data;
					unsigned char	R, G, B;
					unsigned char	Count;
					EMachine		MType;

					int ReadIn = 0;

					if (_GraphicsObject->GetPlatform())
						MType = _GraphicsObject->GetPlatform()->GetMachineType();
					else
						return false;

					for (int Index = TotalCount; Index--;)
					{
						// Read pixel.
						Count	= *Source;	Source++;
						R		= *Source;	Source++;
						G		= *Source;	Source++;
						B		= *Source;	Source++;

						ReadIn += Count;

						int Loop;

						// Process pixels.
						if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect555)
						{
							R = (R >> 3);
							G = (G >> 3);
							B = (B >> 3);
									
							WORD Value = (R << 10) + (G << 5) + B;
							for (Loop = Count; Loop--;)
							{
								*(WORD*)Dest = Value;
								Dest += sizeof(WORD);
							}
						}
						else if(_GraphicsObject->GetDisplay()->ffFormat & kfDirect565)
						{
							// 565 is in the format RRRRRxGGGGGBBBBB - (This 
							// information was not provided with the 
							// documentation, but is in an article on Microsoft's 
							// website). - Unless, of course, you don't stick 
							// to that format and use another one, such as 
							// Casio. They use RRRRRGGGGGGBBBBB. (Which is what
							// you would expect, but counter to what Microsoft
							// say on their website. God know which is correct.
							R = (R >> 3);
							G = (G >> 3);	// Don't really care about the lowest bit too much.
							B = (B >> 3);

							WORD Value = (R << 11) + (G << 6) + B;

							for (Loop = Count; Loop--;)
							{
								*(WORD*)Dest = Value;
								Dest += sizeof(WORD);
							}
						}
						else if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect888)
						{
							DWORD Value = (R << 16) + (G << 8) + B;

							for (Loop = Count; Loop--;)
							{
								*(DWORD*)Dest = Value;
								Dest += sizeof(DWORD);
							}
						}
						else if (_GraphicsObject->GetDisplay()->cBPP == 8)
						{
							if (_GraphicsObject->GetDisplay()->ffFormat & kfDirect)
							{
								// Direct 8 bit mode. 
								R = (R >> 5);
								G = (G >> 5);
								B = (B >> 6);

								unsigned char Value = (R << 5) + (G << 2) + B;
								for (Loop = Count; Loop--;)
								{
									*Dest = Value;	
									Dest++;
								}
							}
							else
							{
								// Palettized 8 bit mode.
								COLORREF InColour;
								InColour = (B << 16) + (G << 8) + R;

								// Making the assumption that the palette is a maximum of 256 entries!!!
								UINT mapcol = _GraphicsObject->Match8BitPalette(InColour);
									
								//if (mapcol != CLR_INVALID)
								{
									for (Loop = Count; Loop--;)
									{
										*(unsigned char*)Dest = (unsigned char)mapcol;
										Dest++;
									}
								}
								/*else
								{
									for (Loop = Count; Loop--;)
									{
										*Dest = 0;
										Dest++;
									}
								}*/
							}
						}
						else
						{
							// This display mode is currently not supported.
							return false;
						}
					}

					SAFELY_DELETE_ARRAY(TempDataBuffer);
					ErrorCode = OBJECT_NO_ERROR;

					ASSERT(ReadIn == (DataWidth * DataHeight));
					return true;
				}
				SAFELY_DELETE_ARRAY(TempDataBuffer);
			}
			else
			{
				fclose(FH);
			}
		}
		else
		{
			OutputDebugString(TEXT("Failed to load image: "));
			OutputDebugString(ImagePath.GetString());
			OutputDebugString(TEXT(".\n"));
		}
	}
	
	ErrorCode = GetLastError();
	return false;
#else
	return true;
#endif
}

// ----
// Will clip offsets to outer bounds of data.
char*	CGraphicsData::GetAddress(int _XOffset, int _OffsetY)
{
	if (Data)
		return (Data + (_OffsetY * DataWidth + _XOffset) * PixelSize);
	else
		return NULL;
}

// ----
DWORD	CGraphicsData::GetTransKey32()
{
	return (ImageInfo.TransparentKey);
}

// ----
WORD	CGraphicsData::GetTransKey16()
{
	return (ImageInfo.TransparentKey & 0xFFFF);
}

// ----
BYTE	CGraphicsData::GetTransKey8()
{
	return (ImageInfo.TransparentKey & 0xFF);
}
