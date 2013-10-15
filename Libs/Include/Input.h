// ==[ File ]===================================================================================
//
//  -Name     : Input.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CInput definition.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


#ifndef __INPUT_H
#define __INPUT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CInput
//
//  - Purpose   : Base abstract class for reading data from all kinds of
//                sources.
//
//  - Note      : Devices should support random access.
//
// ---------------------------------------------------------------------------
class CInput
{


public:


	// --[ Enumeration ]----------------------------------------------------------
	//
	//  - Name      : ESeekFrom
	//
	//  - Purpose   : Enumerate relative positioning modes for SetPosition.
	//
	// ---------------------------------------------------------------------------
	enum ESeekFrom
	{
		SEEKFROM_START,
		SEEKFROM_END,
		SEEKFROM_CURPOS
	};


	CInput()          { }
	virtual ~CInput() { }

	bool End() const { return GetPosition() >= GetLength(); }

	virtual bool Ready() const = 0;
	virtual bool Close() = 0;

	virtual bool ReadChar (char*  pchDest) = 0;
	virtual bool ReadInt  (int*   pnDest)  = 0;
	virtual bool ReadFloat(float* pfDest)  = 0;

	virtual int  ReadRawData   (int nNumBytes, int nNumTimes, void* pDestiny) = 0;
	virtual int  ReadStringZero(std::string *strpDestString)                  = 0;

	virtual int  GetLength()   const = 0;
	virtual int  GetPosition() const = 0;
	virtual bool SetPosition(int nPosition, ESeekFrom eSeekFrom) = 0;

};


#endif
