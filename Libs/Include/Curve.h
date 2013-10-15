// ==[ File ]===================================================================================
//
//  -Name     : Curve.h
//  -Proyect  : DemoSystem
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CCurve definition.
//
// =============================================================================================

/*

	23/09/2002  -  File created.


*/


#ifndef __CURVE_H
#define __CURVE_H


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CCurve
//
//  - Purpose   : Base class for evaluatable key-set entities.
//                Given a time value they return a CVector3.
//
//  - Notes     : Key i must always have a time value less than key i+1. That
//                introduces some restrictions since SetKeyTime() must have
//                a time value in the range of (key[i-1].time , key[i+1].time)
//                Otherwise it would return false and key[i].time would
//                remain unchanged. Childs should use ValidNewTime() for this.
//
// ---------------------------------------------------------------------------
class CCurve : public CPlugin
{


public:


	// Methods

	CCurve()          { }
	virtual ~CCurve() { }

	static std::string	GETCLASSTYPE	();
	std::string			GetClassType	() const { return GETCLASSTYPE(); }

	// Virtual

	virtual bool Delete			()										= 0;

	virtual bool GetStartTime	(float* pfOut) const					= 0;
	virtual bool GetEndTime		(float* pfOut) const					= 0;

	virtual int  GetNumKeys		() const								= 0;
	virtual bool GetKeyTime		(int nIndex, float*    pfOut)  const	= 0;
	virtual bool GetKeyValue	(int nIndex, CVector3* pv3Out) const	= 0;
	virtual bool SetKeyTime		(int nIndex, float     fTime)			= 0;
	virtual bool SetKeyValue	(int nIndex, const CVector3& v3Value)	= 0;

	virtual bool Parse			(CAdvancedParser* pParser)				= 0;
	virtual bool WriteASCII		(CWriterASCII*    pWriter)				= 0;

	virtual bool Evaluate		(float fTime, CVector3* pv3Out) const	= 0; // starttime <= fT <= endtime

	bool ValidKeyIndex			(int nIndex) const
	{
		if(GetNumKeys() == 0)
		{
			return false;
		}

		if(nIndex < 0 || nIndex >= GetNumKeys())
		{
			return false;
		}

		return true;
	}
};


#endif