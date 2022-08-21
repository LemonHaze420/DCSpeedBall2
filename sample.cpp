#include	"Sample.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	"Platform.h"
#include	"MyString.h"
#include	"Misc.h"
#include	<windows.h>

#define UNSUPPORTED_NUMBER_OF_BITS_PER_SAMPLE	(1001)

// ----
CSample::CSample()
{
	PlatformRef	= NULL;
	Data		= NULL;
	DataLength	= 0;

	LoopStart	= NULL;
	LoopEnd		= NULL;
}

CSample::~CSample()
{
	SAFELY_DELETE_ARRAY(Data);
}

// ----
// WARNING: This code is NOT fully functional. 
// Some of the over sampling / under sampling code is most definately buggy!
// It works with the current data on the current batch of devices.
// Will revise later!
bool	CSample::Init(	CPlatform*	_PlatformRef,
						CString*	_FileName, 
						bool		_PrependPath)
{
	CString		FilePath;

	// Store imported value.1
	PlatformRef = _PlatformRef;	

	// Find out where the file resides.
	if (_PrependPath && PlatformRef)
	{
		FilePath = PlatformRef->GetPath(EP_AudioData);
	}
	FilePath += _FileName;

	// Load data.

	/*
	struct { 
		WORD  wFormatTag; 
		WORD  nChannels; 
		DWORD nSamplesPerSec; 
		DWORD nAvgBytesPerSec; 
		WORD  nBlockAlign; 
		WORD  wBitsPerSample; 
		WORD  cbSize; 
	} WAVEFORMATEX; 
	*/
	WAVEFORMATEX	WavHeader;
	FILE			*FH;
	int				LoadedDataLength;
	char*			LoadedData;

#ifdef _UNICODE
	FH = _wfopen(FilePath.GetString(), TEXT("rb"));
#else
	FH = fopen(FilePath.GetString(), TEXT("rb"));
#endif

	if (FH)
	{
		bool ReadFile;
		// Seek passed the RIFF data (Actually, this is 
		// little more than guess work based on the 
		// observation of wav files - documentation is a 
		// little lacking, or miss leading).
		fseek(FH, 20, SEEK_SET);

		ZeroMemory(&WavHeader, sizeof(WavHeader));
		ReadFile = (fread(&WavHeader, sizeof(WavHeader), 1, FH) == 1);
		
		if (ReadFile)
		{
			if (WavHeader.wFormatTag == WAVE_FORMAT_PCM) // Hope...
			{
				// Move in to position to collect data length info.
				fseek(FH, 40, SEEK_SET); 
				if (fread(&LoadedDataLength, sizeof(DWORD), 1, FH) != 1)
					return false;

				// Prepare buffer.
				SAFELY_DELETE_ARRAY(Data);
				if (LoadedDataLength > 0)
				{
					LoadedData = new char[LoadedDataLength];
		
					// Read, close and return
					if (fread(LoadedData, sizeof(char) * LoadedDataLength, 1, FH) == 1)
					{	
						fclose(FH);
						// Got the data in original format, now 
						// process sample to 11025Hz, 16 bit mono.
						
						// Start with existing data length and perform 
						// conversion calculations.
						int NumberOfSamples = LoadedDataLength;
						NumberOfSamples /= WavHeader.nChannels;					// Reduce multichannel data to mono.
						//NumberOfSamples	*= 2;
						NumberOfSamples /= (WavHeader.nSamplesPerSec / 11025);	// Fix sample rate to 11025Hz

						NumberOfSamples *= (32 / WavHeader.wBitsPerSample);	
						NumberOfSamples /= 4;

						DataLength = NumberOfSamples;// * 2;

						// Create actual data buffer of correct size.
						Data = new short[NumberOfSamples];// * 2];
						short*	DataIndex = Data;
						if (Data)
						{
							// Start with a pointer aligned to the start of the sample data.
							char*	LoadedIndex = LoadedData;

							// Perform processing then...
							for (int i = NumberOfSamples; i--;)
							{
								// A pointer to this particular sample (point)
								char*	SampleIndex	= LoadedIndex;

								// Sum together the over sampled data.
								long	SamplesSum = 0;
								for (int j = (WavHeader.nSamplesPerSec / 11025); j--;)
								{
									// Sum together channels
									long Sample = 0;
									for (int k = WavHeader.nChannels; k--;)
									{
										// Might need to convert from 8 bit sample first!
										switch (WavHeader.wBitsPerSample)
										{
										case 8:
											Sample += (*(char*)SampleIndex) << 8; // Up rate to 16 bit value.
											SampleIndex++;		// 1 byte
											break;
										case 16:
											Sample += *(short*)SampleIndex;
											SampleIndex += 2;	// 2 bytes of data read.
											break;
										default:
											delete[] Data;
											delete[] LoadedData;
											ErrorCode = UNSUPPORTED_NUMBER_OF_BITS_PER_SAMPLE;
											return false;
										}
									}

									Sample /= WavHeader.nChannels;
									SamplesSum += Sample; 
								}

								SamplesSum /= (WavHeader.nSamplesPerSec / 11025);

								// Left channel
								*DataIndex = SamplesSum;
								DataIndex++;

					//			// Right channel
					//			*DataIndex = SamplesSum;
					//			DataIndex++;
								
								// Move index to the next sample we are 
								// interested in, by multiple of number 
								// of channels, the number of bytes per 
								// sample and the magnitude of difference
								// in sampling frequencies.
								LoadedIndex	+= 
										WavHeader.nChannels 
									*	(WavHeader.wBitsPerSample >> 3)
									*	(WavHeader.nSamplesPerSec / 11025);
							}
							
							delete[] LoadedData;
							return true;
						}
						else
						{
							delete[] LoadedData;
							return false;
						}
					}
				}
			}
		}

		fclose(FH);
	}
	
	return false;
}


// ----
// This method is dumb. 
// It assumes it is reading 8bit, mono, signed data. 
// It converts it to 16 bit mono
// It doesn't care about the frequency - that is gonna be a problem later.

// some code here
bool	CSample::ModInit(	FILE*			_FH,
							unsigned int	_ReadBytes,
							int				_LoopStart,
							int				_LoopEnd)
{
	char	*LoadData;
	int		NumberOfSamples;	// Sample conversion, thing.
	int		i;
	
	LoadData = new char[_ReadBytes];
	if (!LoadData)
		return false;

	NumberOfSamples = _ReadBytes;
	NumberOfSamples *= 2;	// 16 bit. (as opposed to the 8 bit I'm reading in).

	DataLength = _ReadBytes + 100;// True sample length + run off space (to stop clippnig).
	Data = new UNALIGNED short[NumberOfSamples];// << 1];
	if (!Data)
		return false;

	short*	DataIndex = Data;

	if (fread(LoadData, _ReadBytes, 1, _FH) != 1)
		return false;	// Failed to read all data.

	// Need to puff out data.
	// Start with a pointer aligned to the start of the sample data.
	char*	LoadedIndex = LoadData;

	// Perform processing then... (Nice comment!).
	for (i = _ReadBytes; i--;)
	{
		// This won't work if the data is sampled at a rate higher than 11025

		// Read byte and up rate to 16 bit value.
		short Sample1 = (*(char*)LoadedIndex);
		Sample1 = Sample1 << 8; 
		LoadedIndex++;

		// Mono
		*DataIndex = Sample1; 

		DataIndex++;		
	}

	// Clear sample run off space.
	for (i = 100; i--;)
	{
		*DataIndex = 0;
		DataIndex++;		
	}

	if (_LoopEnd > 2) // A value less than two often indicates a 'silent' pause at the start of a sample.
	{
		DataLength = _LoopEnd;
	}

	// Copy over loop data.
	LoopStart = _LoopStart;
	LoopEnd = _LoopEnd;

	delete[] LoadData;
	return true;
}
