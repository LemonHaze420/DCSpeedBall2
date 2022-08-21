#ifndef _MAPDATA_H_
#define _MAPDATA_H_

#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>


// Very incomplete class.
class CMapData : public CObject
{
public:
	CMapData();
	~CMapData();

	// ----
	// Init the object.
	virtual bool	Init();
	
protected:
	
};

#endif