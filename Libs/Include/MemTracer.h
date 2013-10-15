// ==[ File ]===================================================================================
//
//  -Name     : MemTracer.h
//  -Proyect  : BaseLib
//  -Author   : Enrique Tromp Maseda A.K.A. Ithaqua^Stravaganza
//
//  -Contents : CMemTracer definition and some other utilities to trace all allocations,
//              deallocations and memory leaks in our program.
//              Redefined new/delete operators, and some macros.
//
// =============================================================================================

/*

	01/10/2001  -  File created.
	26/04/2004  -  Changed internal list from vector to hash table.


*/


#ifndef __MEMTRACER_H
#define __MEMTRACER_H


#ifdef _DEBUG
	#define USE_MEMTRACER // Only for the debug version
#endif


// ==[ Operators ]==============================================================================


#ifdef USE_MEMTRACER
	extern void* operator new     (size_t size);
	extern void* operator new[]   (size_t size);
	extern void* operator new     (size_t size, char *szFile, int nLine);
	extern void* operator new[]   (size_t size, char *szFile, int nLine);
	extern void  operator delete  (void *pPointer);
	extern void  operator delete[](void *pPointer);

	// Needed when new throws an exception
	extern void operator delete   (void *pPointer, char *szFile, int nLine);
	extern void operator delete[] (void *pPointer, char *szFile, int nLine);
#endif


// ==[ Macros ]=================================================================================


// This way we transform all dinamic memory request calls to our overloaded operators,
// giving us the possibility to trace all calls.

#ifdef USE_MEMTRACER
	#define new new(__FILE__, __LINE__)
#endif

// Safe deallocations

#define SAFE_DELETE(p)    if((p))  { delete(p);    (p) =NULL; }
#define SAFE_DELETE_A(pa) if((pa)) { delete[](pa); (pa)=NULL; }


// ==[ Class definitions ]======================================================================


// --[   Class    ]-----------------------------------------------------------
//
//  - Name      : CMemTracer
//
//  - Purpose   : Hold information of all new/delete calls.
//
// ---------------------------------------------------------------------------
class CMemTracer
{


public:


	~CMemTracer();

	static CMemTracer& Instance();
	static void ShareFrom	(CMemTracer& memTracer);

	int  CurAllocBytes		() const { return m_nCurAllocBytes; }
	int  MaxAllocBytes		() const { return m_nMaxAllocBytes; }

	void DumpEndProgramStats();

	void RegisterNewCall	(char *szFile, int nLine, int nNumBytes, void *pPointer);
	void RegisterDeleteCall	(void *pPointer);

	inline bool IsEnabled	()           const { return m_bEnabled;  }
	void       SetEnabled	(bool bTrue)       { m_bEnabled = bTrue; }


private:


	// Singleton:

	CMemTracer();

	bool m_bEnabled;

	struct SAllocRecord
	{
		void* pPointer;
		char* szFile;
		int   nLine;
		int   nSize;
	};

	typedef std::map<void*, SAllocRecord *> MAPCALLLIST;

	MAPCALLLIST m_mapCallList;

	int m_nNewCalls;
	int m_nDeleteCalls;
	int m_nCurAllocBytes;
	int m_nMaxAllocBytes;

	static CMemTracer* s_pSharedInstance;
};
	

#endif // __MEMTRACER_H