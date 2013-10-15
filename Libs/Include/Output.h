// ==[ File ]===================================================================================
//
//  -Name     : Output.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : COutput definition.
//
// =============================================================================================

/*

	26/09/2001  -  File created.


*/


#ifndef __OUTPUT_H
#define __OUTPUT_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : COutput
//
//  - Purpose   : Base abstract class to handle data output.
//
//  - Note      : Data output is limited to secuential order.
//
// ---------------------------------------------------------------------------
class COutput
{


public:


	COutput()          { }
	virtual ~COutput() { }

	virtual bool Ready() const = 0;
	virtual bool Close() = 0;

	virtual bool WriteChar (char chValue) = 0;
	virtual bool WriteInt  (int   nValue) = 0;
	virtual bool WriteFloat(float fValue) = 0;

	virtual int  WriteRawData   (int nNumBytes, int nNumTimes, const void *pSource) = 0;
	virtual int  WriteStringZero(const std::string& strStringZero)                  = 0;

	int WriteString(const std::string& strString)
	{
		return WriteRawData(strString.length(), 1, strString.data());
	}

};


#endif
